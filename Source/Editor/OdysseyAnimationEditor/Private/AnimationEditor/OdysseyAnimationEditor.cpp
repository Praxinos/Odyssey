// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditor.h"

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationTexture.h"
#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"


#define LOCTEXT_NAMESPACE "OdysseyAnimationEditor"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditor::~FOdysseyAnimationEditor()
{
	mPlayer->OnStop().RemoveAll(this);
	mPlayer->Stop();
	mAnimation->OnCurrentFrameChanged().RemoveAll(this);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().RemoveAll(this);
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
}

FOdysseyAnimationEditor::FOdysseyAnimationEditor() :
	FOdysseyPainterEditor(),
	mAnimation(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mPaintBucketTool(nullptr),
	mColorPickerTool(nullptr),
	mPlayer(nullptr),
    mTexture(),
	mPlaybackFramesPerSecond(0)
{
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentLayerChanged);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditor::InitData(UObject* iEditedObject)
{
	mAnimation = Cast<UOdysseyAnimation>(iEditedObject);

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	mPlaybackFramesPerSecond = mAnimation->GetFramesPerSecond();

	//Configure a Media player and media texture to be able to display and play the animation
	mPlayer = NewObject<UOdysseyAnimationPlayer>();
    mTexture = NewObject<UOdysseyAnimationTexture>();

	mPlayer->SetAnimation(mAnimation);
	mPlayer->SetRenderType(IOdysseyImageRenderer::eRenderType::Editor);
	mTexture->SetPlayer(mPlayer);
	mTexture->UpdateResource();

	//Seek at current frame 
    mPlayer->SeekToFrame(mAnimation->CurrentFrame);

	//Set Media player and Animation callbacks
	mPlayer->OnStop().AddRaw(this, &FOdysseyAnimationEditor::OnPlayerStop);
	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentFrameChanged);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().AddRaw(this, &FOdysseyAnimationEditor::OnImageRenderingCompositionCommited);

	FOdysseyPainterEditor::InitData(iEditedObject);
}

void
FOdysseyAnimationEditor::InitTools()
{
	mRasterDrawingTool = NewObject<UOdysseyAnimationEditorRasterDrawingTool>();
	mPaintBucketTool = NewObject<UOdysseyAnimationEditorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyAnimationEditorColorPickerTool>();

	mRasterDrawingTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
	mTools.Add(mPaintBucketTool);
	mTools.Add(mColorPickerTool);
	//mAnimationRasterDrawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
}

void
FOdysseyAnimationEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
	mColorPickerTool->BindShortcuts(iToolkit);
}

void
FOdysseyAnimationEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
	mColorPickerTool->ExtendMenu(iOwner, iMenuName);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyAnimation*
FOdysseyAnimationEditor::Animation() const
{
	return mAnimation;
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditor::LayerStack() const
{
	return mAnimation->GetLayerStack();
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditor::Player() const
{
	return mPlayer;
}

FOdysseyAnimationEditorTimeline*
FOdysseyAnimationEditor::Timeline()
{
	return &mTimeline;
}

float
FOdysseyAnimationEditor::PlaybackFramesPerSecond() const
{
	return mPlaybackFramesPerSecond;
}

UTexture*
FOdysseyAnimationEditor::DisplayTexture() const
{
	return mTexture;
}

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyAnimationEditor::GetDisplayBlock()
{
	//TODO: this is used only for picking a color in PainterEditor's viewport tab
	//Find a way to do it without having that method

	return nullptr;
}

UOdysseyAnimationEditorRasterDrawingTool*
FOdysseyAnimationEditor::GetRasterDrawingTool() const
{
	return mRasterDrawingTool;
}

UOdysseyAnimationEditorPaintBucketTool*
FOdysseyAnimationEditor::GetPaintBucketTool() const
{
	return mPaintBucketTool;
}

UOdysseyAnimationEditorColorPickerTool*
FOdysseyAnimationEditor::GetColorPickerTool() const
{
    return mColorPickerTool;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

FOdysseyAnimationEditorGUI*
FOdysseyAnimationEditor::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyAnimationEditorGUI(this));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyAnimationEditor::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyAnimationEditor", "Odyssey Animation2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------- FGCObject implementation

void
FOdysseyAnimationEditor::AddReferencedObjects(FReferenceCollector& Collector)
{
	FOdysseyPainterEditor::AddReferencedObjects(Collector);
	Collector.AddReferencedObject(mRasterDrawingTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);

	Collector.AddReferencedObject(mPlayer);
	Collector.AddReferencedObject(mTexture);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditor::OnPlayerStop()
{
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
}

void
FOdysseyAnimationEditor::OnImageRenderingCompositionCommited(const FGuid& iFrameId)
{
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	TArray<FGuid> imageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

void
FOdysseyAnimationEditor::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
	if (iAnimation != mAnimation)
		return;

	//Preload the new current frame for edition
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);

	//Display the new current frame
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
	mPlayer->Stop();

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

void
FOdysseyAnimationEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	//TODO: Maybe this should be done differently later, but we don't have time for that now
	if (iLayerStack == LayerStack())
		SelectDefaultTool(); //Refresh the current tool when we change layer
}

#undef LOCTEXT_NAMESPACE