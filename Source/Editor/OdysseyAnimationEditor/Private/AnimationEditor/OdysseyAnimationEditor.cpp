// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditor.h"

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/OdysseyAnimationLayerImageRaster.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyAnimation.h"
#include "MediaPlayer.h"
#include "MediaTexture.h"


#define LOCTEXT_NAMESPACE "OdysseyAnimationEditor"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditor::~FOdysseyAnimationEditor()
{
}

FOdysseyAnimationEditor::FOdysseyAnimationEditor() :
	FOdysseyPainterEditor(),
	mAnimation(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mPaintBucketTool(nullptr),
	mMediaPlayer(nullptr),
    mMediaTexture()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditor::InitData(UObject* iEditedObject)
{
	mAnimation = Cast<UOdysseyAnimation>(iEditedObject);
	mLayerStackPreloadHandle = mAnimation->Preload(mAnimation->CurrentFrame);

	mMediaPlayer = NewObject<UMediaPlayer>();
    mMediaTexture = NewObject<UMediaTexture>();

	mMediaPlayer->OpenSource(mAnimation);
	mMediaPlayer->SetLooping(true);
	mMediaPlayer->Play();

	mMediaTexture->SetMediaPlayer(mMediaPlayer);
	mMediaTexture->UpdateResource();

	FOdysseyPainterEditor::InitData(iEditedObject);
}

void
FOdysseyAnimationEditor::InitTools()
{
	mRasterDrawingTool = NewObject<UOdysseyAnimationEditorRasterDrawingTool>();
	mPaintBucketTool = NewObject<UOdysseyAnimationEditorPaintBucketTool>();

	mRasterDrawingTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
	mTools.Add(mPaintBucketTool);
	//mAnimationRasterDrawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
}

void
FOdysseyAnimationEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
}

void
FOdysseyAnimationEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
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

UTexture*
FOdysseyAnimationEditor::DisplayTexture() const
{
	//TODO: return the media texture used for reading the animation

	//return mDisplaySurface->Texture();
	return mMediaTexture;
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

	Collector.AddReferencedObject(mMediaPlayer);
	Collector.AddReferencedObject(mMediaTexture);
}
#undef LOCTEXT_NAMESPACE