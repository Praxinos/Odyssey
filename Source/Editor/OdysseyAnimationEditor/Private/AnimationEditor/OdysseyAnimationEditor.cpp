// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditor.h"

#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "OdysseyLayerFunctionLibrary.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationLayerImageRaster.h"
#include "LayerStack/Layers/LayerImageVector/OdysseyAnimationLayerImageVector.h"
#include "LayerStack/Cells/CellImageRaster/OdysseyAnimationCellImageRaster.h"
#include "LayerStack/Layers/LayerImageRaster/OdysseyAnimationCellsMutator.h"
#include "OdysseyPaintEngine.h"
#include "OdysseyBlendParameters.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationTexture.h"
#include "OdysseyAnimationEditorSource.h"
#include "OdysseyMediaRaster.h"
#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"
#include "Abilities/IOdysseyAnimationMediaAbility.h"
#include "ULISLoaderModule.h"


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
	mGUI(nullptr),
	mAnimation(nullptr),
	mPlaybackFramesPerSecond(0)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditor::OnSourceInactivated()
{
	if (!mAnimation)
		return;

	//TODO: CurrentFrame should not be in the animation itself, it's an editor specific data
	mAnimation->OnCurrentFrameChanged().RemoveAll(this);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().RemoveAll(this);
	UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
	IOdysseyAnimationMediaAbility::OnChanged().RemoveAll(this);

	mAnimation = nullptr;
}

void
FOdysseyAnimationEditor::OnSourceActivated()
{
	mAnimation = Animation();
	if (!mAnimation)
		return;

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = mAnimation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(mAnimation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	mPlaybackFramesPerSecond = mAnimation->GetFramesPerSecond();

	//Set Media player and Animation callbacks
	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentFrameChanged);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().AddRaw(this, &FOdysseyAnimationEditor::OnImageRenderingCompositionCommited);
	IOdysseyAnimationMediaAbility::OnChanged().AddRaw(this, &FOdysseyAnimationEditor::OnLayerStackElementMediaChanged);
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentLayerChanged);

	SelectDefaultTool();
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyAnimation*
FOdysseyAnimationEditor::Animation() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
	if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
		return nullptr;

	TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);
	return animationSource->GetAnimation();
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditor::LayerStack() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
	if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
		return nullptr;

	TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);
	return Cast<UOdysseyAnimationLayerStack>(animationSource->GetLayerStack());
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditor::Player() const
{
	TSharedPtr<FOdysseyPainterEditorSource> source = GetSource();
	if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
		return nullptr;

	TSharedPtr<FOdysseyAnimationEditorSource> animationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);
	return animationSource->GetAnimationPlayer();
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

void
FOdysseyAnimationEditor::OnSelectedToolChanged()
{
	UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(LayerStack()->CurrentLayer.Get());
	if (!currentLayer)
		return;

	UClass* layerClass = currentLayer->GetClass();
	if (!mCurrentToolPerLayerClass.Contains(layerClass))
		mCurrentToolPerLayerClass.Add(layerClass, nullptr);

	mCurrentToolPerLayerClass[layerClass] = mSelectedTool;
}

UOdysseyPainterEditorTool*
FOdysseyAnimationEditor::FindDefaultToolForCurrentLayer()
{
	for (UOdysseyPainterEditorTool* tool : mTools)
	{
		if ( !tool->IsActivable() )
			continue;

		return tool;
	}
	return nullptr;
}

void
FOdysseyAnimationEditor::SelectDefaultTool()
{
	UOdysseyAnimationLayer* currentLayer = Cast<UOdysseyAnimationLayer>(LayerStack()->CurrentLayer.Get());
	if (!currentLayer)
	{
		SetSelectedTool(nullptr);
		return;
	}

	UOdysseyPainterEditorTool* tool = nullptr;

	UClass* layerClass = currentLayer->GetClass();
	if (mCurrentToolPerLayerClass.Contains(layerClass))
		tool = mCurrentToolPerLayerClass[layerClass];
	
	if (!tool || !tool->IsActivable())
		tool = FindDefaultToolForCurrentLayer();

	SetSelectedTool(tool);
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
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditor::OnLayerStackElementMediaChanged()
{
	if (!mAnimation)
		return;

	SelectDefaultTool(); //Refresh the current tool
}

void
FOdysseyAnimationEditor::OnImageRenderingCompositionCommited(const FGuid& iFrameId)
{
	if (!mAnimation)
		return;

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
FOdysseyAnimationEditor::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;
	//TODO: Maybe this should be done differently later, but we don't have time for that now
	SelectDefaultTool(); //Refresh the current tool when we change layer
    Timeline()->SetSelectedFrames(FInt32Range()); //Clear Selected frames when changing layer
}

#undef LOCTEXT_NAMESPACE