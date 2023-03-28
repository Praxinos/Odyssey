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
#include "OdysseyAnimationPlayer.h"
#include "OdysseyAnimationTexture.h"


#define LOCTEXT_NAMESPACE "OdysseyAnimationEditor"

/////////////////////////////////////////////////////
// FOdysseyAnimationEditor
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyAnimationEditor::~FOdysseyAnimationEditor()
{
	mPlayer->OnStop().RemoveAll(this);
	mAnimation->OnCurrentFrameChanged().RemoveAll(this);
	mAnimation->OnRenderImageChanged().RemoveAll(this);
}

FOdysseyAnimationEditor::FOdysseyAnimationEditor() :
	FOdysseyPainterEditor(),
	mAnimation(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mVectorPrimitiveDrawingTool(nullptr),
	mVectorPathDrawingTool(nullptr),
	mVectorPathEditTool(nullptr),
	mVectorPathCutTool(nullptr),
	mVectorObjectPickTool(nullptr),
	mVectorObjectMoveTool(nullptr),
	mVectorObjectRotateTool(nullptr),
	mVectorObjectScaleTool(nullptr),
	mVectorSceneScaleTool(nullptr),
	mVectorScenePanTool(nullptr),
	mVectorEraserTool(nullptr),
	mVectorPathPushTool(nullptr),
	mVectorPathWidthTool(nullptr),
	mVectorPathSmoothTool(nullptr),
	mVectorPathKnotTool(nullptr),
	mPaintBucketTool(nullptr),
	mColorPickerTool(nullptr),
	mVectorGridTool(nullptr),
	mPlayer(nullptr),
    mTexture(),
	mPlaybackFramesPerSecond(0)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditor::InitData(UObject* iEditedObject)
{
	mAnimation = Cast<UOdysseyAnimation>(iEditedObject);
	mLayerStackPreloadHandle = mAnimation->Preload(mAnimation->CurrentFrame);
	mPlaybackFramesPerSecond = mAnimation->GetFramesPerSecond();

	//Configure a Media player and media texture to be able to display and play the animation
	mPlayer = NewObject<UOdysseyAnimationPlayer>();
    mTexture = NewObject<UOdysseyAnimationTexture>();

	mPlayer->SetAnimation(mAnimation);
	mTexture->SetPlayer(mPlayer);
	mTexture->UpdateResource();

	//Seek at current frame 
    mPlayer->SeekToFrame(mAnimation->CurrentFrame);

	//Set Media player and Animation callbacks
	mPlayer->OnStop().AddRaw(this, &FOdysseyAnimationEditor::OnPlayerStop);
	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentFrameChanged);
	mAnimation->OnRenderImageChanged().AddRaw(this, &FOdysseyAnimationEditor::OnRenderImageChanged);

	FOdysseyPainterEditor::InitData(iEditedObject);
}

void
FOdysseyAnimationEditor::InitTools()
{
	mRasterDrawingTool = NewObject<UOdysseyAnimationEditorRasterDrawingTool>();
	mVectorPrimitiveDrawingTool = NewObject<UOdysseyAnimationEditorVectorPrimitiveDrawingTool>();
	mVectorPathDrawingTool = NewObject<UOdysseyAnimationEditorVectorPathDrawingTool>();
	mVectorPathEditTool = NewObject<UOdysseyAnimationEditorVectorPathEditTool>();
	mVectorPathCutTool = NewObject<UOdysseyAnimationEditorVectorPathCutTool>();
	mVectorObjectPickTool = NewObject<UOdysseyAnimationEditorVectorObjectPickTool>();
	mVectorObjectMoveTool = NewObject<UOdysseyAnimationEditorVectorObjectMoveTool>();
	mVectorObjectRotateTool = NewObject<UOdysseyAnimationEditorVectorObjectRotateTool>();
	mVectorObjectScaleTool = NewObject<UOdysseyAnimationEditorVectorObjectScaleTool>();
    mVectorSceneScaleTool = NewObject<UOdysseyAnimationEditorVectorSceneScaleTool>();
    mVectorScenePanTool = NewObject<UOdysseyAnimationEditorVectorScenePanTool>();
    mVectorEraserTool = NewObject<UOdysseyAnimationEditorVectorEraserTool>();
    mVectorPathPushTool = NewObject<UOdysseyAnimationEditorVectorPathPushTool>();
    mVectorPathWidthTool = NewObject<UOdysseyAnimationEditorVectorPathWidthTool>();
    mVectorPathSmoothTool = NewObject<UOdysseyAnimationEditorVectorPathSmoothTool>();
    mVectorPathKnotTool = NewObject<UOdysseyAnimationEditorVectorPathKnotTool>();
	mPaintBucketTool = NewObject<UOdysseyAnimationEditorPaintBucketTool>();
	mColorPickerTool = NewObject<UOdysseyAnimationEditorColorPickerTool>();
	mVectorGridTool = NewObject<UOdysseyAnimationEditorVectorGridTool>();

	mRasterDrawingTool->SetEditor(this);
    mVectorPrimitiveDrawingTool->SetEditor(this);
    mVectorPathDrawingTool->SetEditor(this);
    mVectorPathEditTool->SetEditor(this);
    mVectorPathCutTool->SetEditor(this);
    mVectorObjectPickTool->SetEditor(this);
    mVectorObjectMoveTool->SetEditor(this);
    mVectorObjectRotateTool->SetEditor(this);
    mVectorObjectScaleTool->SetEditor(this);
    mVectorSceneScaleTool->SetEditor(this);
    mVectorScenePanTool->SetEditor(this);
    mVectorEraserTool->SetEditor(this);
    mVectorPathPushTool->SetEditor(this);
    mVectorPathWidthTool->SetEditor(this);
    mVectorPathSmoothTool->SetEditor(this);
    mVectorPathKnotTool->SetEditor(this);
	mPaintBucketTool->SetEditor(this);
	mColorPickerTool->SetEditor(this);
	mVectorGridTool->SetEditor(this);
	mRasterDrawingTool->SetBrushContexts(mBrushContexts);

	mTools.Add(mRasterDrawingTool);
	mTools.Add(mPaintBucketTool);
    mTools.Add(mVectorPrimitiveDrawingTool);
    mTools.Add(mVectorPathDrawingTool);
    mTools.Add(mVectorPathEditTool);
    mTools.Add(mVectorPathCutTool);
    mTools.Add(mVectorObjectPickTool);
    mTools.Add(mVectorObjectMoveTool);
    mTools.Add(mVectorObjectRotateTool);
    mTools.Add(mVectorObjectScaleTool);
    mTools.Add(mVectorSceneScaleTool);
    mTools.Add(mVectorScenePanTool);
    mTools.Add(mVectorEraserTool);
    mTools.Add(mVectorPathPushTool);
    mTools.Add(mVectorPathWidthTool);
    mTools.Add(mVectorPathSmoothTool);
    mTools.Add(mVectorPathKnotTool);
	mTools.Add(mPaintBucketTool);
	mTools.Add(mColorPickerTool);
	mTools.Add(mVectorGridTool);
	//mAnimationRasterDrawingTool->OnApplyOverridesDelegate().AddRaw(this, &FOdysseyPainterEditor::OnApplyOverrides);
}

void
FOdysseyAnimationEditor::BindShortcuts(FBaseToolkit* iToolkit)
{
	FOdysseyPainterEditor::BindShortcuts(iToolkit);
	mRasterDrawingTool->BindShortcuts(iToolkit);
	mVectorPrimitiveDrawingTool->BindShortcuts(iToolkit);
	mVectorPathDrawingTool->BindShortcuts(iToolkit);
	mVectorPathEditTool->BindShortcuts(iToolkit);
	mVectorPathCutTool->BindShortcuts(iToolkit);
	mVectorObjectPickTool->BindShortcuts(iToolkit);
	mVectorObjectMoveTool->BindShortcuts(iToolkit);
	mVectorObjectRotateTool->BindShortcuts(iToolkit);
	mVectorObjectScaleTool->BindShortcuts(iToolkit);
	mVectorSceneScaleTool->BindShortcuts(iToolkit);
	mVectorScenePanTool->BindShortcuts(iToolkit);
	mVectorEraserTool->BindShortcuts(iToolkit);
	mVectorPathPushTool->BindShortcuts(iToolkit);
	mVectorPathWidthTool->BindShortcuts(iToolkit);
	mVectorPathSmoothTool->BindShortcuts(iToolkit);
	mVectorPathKnotTool->BindShortcuts(iToolkit);
	mPaintBucketTool->BindShortcuts(iToolkit);
	mColorPickerTool->BindShortcuts(iToolkit);
	mVectorGridTool->BindShortcuts(iToolkit);
}

void
FOdysseyAnimationEditor::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
	FOdysseyPainterEditor::ExtendMenu(iOwner, iMenuName);
	mRasterDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPrimitiveDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathDrawingTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathEditTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathCutTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectPickTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectMoveTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectRotateTool->ExtendMenu(iOwner, iMenuName);
	mVectorObjectScaleTool->ExtendMenu(iOwner, iMenuName);
	mVectorSceneScaleTool->ExtendMenu(iOwner, iMenuName);
	mVectorScenePanTool->ExtendMenu(iOwner, iMenuName);
	mVectorEraserTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathPushTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathWidthTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathSmoothTool->ExtendMenu(iOwner, iMenuName);
	mVectorPathKnotTool->ExtendMenu(iOwner, iMenuName);
	mPaintBucketTool->ExtendMenu(iOwner, iMenuName);
	mColorPickerTool->ExtendMenu(iOwner, iMenuName);
	mVectorGridTool->ExtendMenu(iOwner, iMenuName);
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

UOdysseyAnimationEditorVectorPrimitiveDrawingTool*
FOdysseyAnimationEditor::GetVectorPrimitiveDrawingTool() const
{
	return mVectorPrimitiveDrawingTool;
}

UOdysseyAnimationEditorVectorPathDrawingTool*
FOdysseyAnimationEditor::GetVectorPathDrawingTool() const
{
	return mVectorPathDrawingTool;
}

UOdysseyAnimationEditorVectorPathEditTool*
FOdysseyAnimationEditor::GetVectorPathEditTool() const
{
    return mVectorPathEditTool;
}

UOdysseyAnimationEditorVectorPathCutTool*
FOdysseyAnimationEditor::GetVectorPathCutTool() const
{
    return mVectorPathCutTool;
}

UOdysseyAnimationEditorVectorObjectPickTool*
FOdysseyAnimationEditor::GetVectorObjectPickTool() const
{
    return mVectorObjectPickTool;
}

UOdysseyAnimationEditorVectorObjectMoveTool*
FOdysseyAnimationEditor::GetVectorObjectMoveTool() const
{
    return mVectorObjectMoveTool;
}

UOdysseyAnimationEditorVectorObjectRotateTool*
FOdysseyAnimationEditor::GetVectorObjectRotateTool() const
{
    return mVectorObjectRotateTool;
}

UOdysseyAnimationEditorVectorObjectScaleTool*
FOdysseyAnimationEditor::GetVectorObjectScaleTool() const
{
    return mVectorObjectScaleTool;
}

UOdysseyAnimationEditorVectorSceneScaleTool*
FOdysseyAnimationEditor::GetVectorSceneScaleTool() const
{
    return mVectorSceneScaleTool;
}

UOdysseyAnimationEditorVectorScenePanTool*
FOdysseyAnimationEditor::GetVectorScenePanTool() const
{
    return mVectorScenePanTool;
}

UOdysseyAnimationEditorVectorEraserTool*
FOdysseyAnimationEditor::GetVectorEraserTool() const
{
    return mVectorEraserTool;
}

UOdysseyAnimationEditorVectorPathSmoothTool*
FOdysseyAnimationEditor::GetVectorPathSmoothTool() const
{
    return mVectorPathSmoothTool;
}

UOdysseyAnimationEditorVectorPathPushTool*
FOdysseyAnimationEditor::GetVectorPathPushTool() const
{
    return mVectorPathPushTool;
}

UOdysseyAnimationEditorVectorPathWidthTool*
FOdysseyAnimationEditor::GetVectorPathWidthTool() const
{
    return mVectorPathWidthTool;
}

UOdysseyAnimationEditorVectorPathKnotTool*
FOdysseyAnimationEditor::GetVectorPathKnotTool() const
{
    return mVectorPathKnotTool;
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

UOdysseyAnimationEditorVectorGridTool*
FOdysseyAnimationEditor::GetVectorGridTool() const
{
    return mVectorGridTool;
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
	Collector.AddReferencedObject(mVectorPrimitiveDrawingTool);
	Collector.AddReferencedObject(mVectorPathDrawingTool);
	Collector.AddReferencedObject(mVectorPathEditTool);
	Collector.AddReferencedObject(mVectorPathCutTool);
	Collector.AddReferencedObject(mVectorObjectPickTool);
	Collector.AddReferencedObject(mVectorObjectMoveTool);
	Collector.AddReferencedObject(mVectorObjectRotateTool);
	Collector.AddReferencedObject(mVectorObjectScaleTool);
    Collector.AddReferencedObject(mVectorSceneScaleTool);
    Collector.AddReferencedObject(mVectorScenePanTool);
    Collector.AddReferencedObject(mVectorEraserTool);
    Collector.AddReferencedObject(mVectorPathPushTool);
    Collector.AddReferencedObject(mVectorPathWidthTool);
    Collector.AddReferencedObject(mVectorPathSmoothTool);
    Collector.AddReferencedObject(mVectorPathKnotTool);
	Collector.AddReferencedObject(mPaintBucketTool);
	Collector.AddReferencedObject(mColorPickerTool);
	Collector.AddReferencedObject(mVectorGridTool);

	Collector.AddReferencedObject(mPlayer);
	Collector.AddReferencedObject(mTexture);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

/* void
FOdysseyAnimationEditor::OnMediaEvent(EMediaEvent iEvent)
{
	switch(iEvent)
	{
		case EMediaEvent::SeekCompleted:
		{
			UE_LOG(LogTemp, Warning, TEXT("Seeked at time = %lf"), mMediaPlayer->GetTime().GetTotalSeconds());
		}
		break;

		case EMediaEvent::PlaybackSuspended:
		{
			if (mMediaPlayer->IsPlaying())
				return;

			//ensure to display the currentframe once, the playback has stopped
			FTimespan time = FTimespan::FromSeconds((mAnimation->CurrentFrame+0.5f) / mAnimation->GetFramesPerSecond());
			mMediaPlayer->Seek(time);
			mMediaPlayer->Pause();
		}
		break;

		default:
			break;
	}
} */

void
FOdysseyAnimationEditor::OnPlayerStop()
{
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
}

void
FOdysseyAnimationEditor::OnRenderImageChanged(UOdysseyAnimation* iAnimation, const TRange<int>& iRange, const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive)
{
	if ( iAnimation != mAnimation )
		return;

	FString frameId = mAnimation->GetFrameId(mAnimation->CurrentFrame);
	if ( frameId != mCurrentFrameId )
	{
		mCurrentFrameId = frameId;
		//Preload the new current frame for edition
		mLayerStackPreloadHandle = mAnimation->Preload(mAnimation->CurrentFrame);

		//Reload the tool
		//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
		UOdysseyPainterEditorTool* tool = GetSelectedTool();
		SetSelectedTool(nullptr);
		SetSelectedTool(tool);
	}
}

void
FOdysseyAnimationEditor::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
	if (iAnimation != mAnimation)
		return;

	//Preload the new current frame for edition
	mLayerStackPreloadHandle = mAnimation->Preload(mAnimation->CurrentFrame);

	//Display the new current frame
	mPlayer->SeekToFrame(mAnimation->CurrentFrame);
	mPlayer->Stop();

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

#undef LOCTEXT_NAMESPACE