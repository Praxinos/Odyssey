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
	mMediaPlayer->OnMediaEvent().RemoveAll(this);
	mAnimation->OnCurrentFrameChanged().RemoveAll(this);
}

FOdysseyAnimationEditor::FOdysseyAnimationEditor() :
	FOdysseyPainterEditor(),
	mAnimation(nullptr),
	mGUI(nullptr),
	mRasterDrawingTool(nullptr),
	mPaintBucketTool(nullptr),
	mMediaPlayer(nullptr),
    mMediaTexture(),
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
	mMediaPlayer = NewObject<UMediaPlayer>();
    mMediaTexture = NewObject<UMediaTexture>();

	mMediaPlayer->PlayOnOpen = false;
	mMediaPlayer->OpenSource(mAnimation);
	mMediaTexture->SetMediaPlayer(mMediaPlayer);
	mMediaTexture->UpdateResource();

	//Seek at current frame 
	FTimespan time = FTimespan::FromSeconds(mAnimation->CurrentFrame / mAnimation->GetFramesPerSecond());
    mMediaPlayer->Seek(time);

	//Set Media player and Animation callbacks
	mMediaPlayer->OnMediaEvent().AddRaw(this, &FOdysseyAnimationEditor::OnMediaEvent);
	mAnimation->OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditor::OnCurrentFrameChanged);

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

UMediaPlayer*
FOdysseyAnimationEditor::MediaPlayer() const
{
	return mMediaPlayer;
}

float
FOdysseyAnimationEditor::PlaybackFramesPerSecond() const
{
	return mPlaybackFramesPerSecond;
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

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
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
			FTimespan time = FTimespan::FromSeconds(mAnimation->CurrentFrame / mAnimation->GetFramesPerSecond());	
			mMediaPlayer->Seek(time);
		}
		break;

		default:
			break;
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
	FTimespan time = FTimespan::FromSeconds(mAnimation->CurrentFrame / mAnimation->GetFramesPerSecond());	
	mMediaPlayer->Seek(time);

	//Reload the tool
	//TODO: we should maybe do this in a different way, it feels a bit weird to unselect and reselect the whole tool
	UOdysseyPainterEditorTool* tool = GetSelectedTool();
	SetSelectedTool(nullptr);
	SetSelectedTool(tool);
}

#undef LOCTEXT_NAMESPACE