// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"
#include "Abilities/IOdysseyAnimationImageRenderingAbility.h"
#include "Abilities/IOdysseyAnimationMediaAbility.h"
#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"

#define LOCTEXT_NAMESPACE "OdysseyAnimationEditorExtension"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyAnimationEditorExtension::~FOdysseyAnimationEditorExtension()
{
}

FOdysseyAnimationEditorExtension::FOdysseyAnimationEditorExtension(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorExtension(iEditor)
	, mAnimationSource(nullptr)
	, mGUI(nullptr)
	, mPlaybackFramesPerSecond(0)
{
}

void
FOdysseyAnimationEditorExtension::Initialize()
{
    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnSourceChanged);
}

void
FOdysseyAnimationEditorExtension::Finalize()
{
    GetEditor()->OnSourceChanged().RemoveAll(this);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization

void
FOdysseyAnimationEditorExtension::OnSourceChanged()
{
    //Is the source an animation
    TSharedPtr<FOdysseyPainterEditorSource> source = GetEditor()->GetSource();
	if (!source || source->Id() != FOdysseyAnimationEditorSource::StaticId())
	{
		mAnimationSource = nullptr;
		UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll(this);
        IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().RemoveAll(this);
        UOdysseyLayerStack::OnCurrentLayerChanged().RemoveAll(this);
        IOdysseyAnimationMediaAbility::OnChanged().RemoveAll(this);
		return;
	}

	mAnimationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);
    
    UOdysseyAnimation* animation = Animation();
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	mImageRenderingComposition = imageRenderAbility->GetComposition(animation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	mPlaybackFramesPerSecond = animation->GetFramesPerSecond();

	//Set Media player and Animation callbacks
	UOdysseyAnimation::OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnCurrentFrameChanged);
	IOdysseyAnimationImageRenderingAbility::OnCompositionCommited().AddRaw(this, &FOdysseyAnimationEditorExtension::OnImageRenderingCompositionCommited);
	IOdysseyAnimationMediaAbility::OnChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnLayerStackElementMediaChanged);
	UOdysseyLayerStack::OnCurrentLayerChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnCurrentLayerChanged);
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyAnimation*
FOdysseyAnimationEditorExtension::Animation() const
{
    if (!mAnimationSource)
        return nullptr;

    return mAnimationSource->GetAnimation();
}

UOdysseyAnimationLayerStack*
FOdysseyAnimationEditorExtension::LayerStack() const
{
    if (!mAnimationSource)
        return nullptr;

    return mAnimationSource->GetLayerStack();
}

UOdysseyAnimationPlayer*
FOdysseyAnimationEditorExtension::Player() const
{
	if (!mAnimationSource)
        return nullptr;

    return mAnimationSource->GetAnimationPlayer();
}

FOdysseyAnimationEditorTimeline*
FOdysseyAnimationEditorExtension::Timeline()
{
	return &mTimeline;
}

float
FOdysseyAnimationEditorExtension::PlaybackFramesPerSecond() const
{
	return mPlaybackFramesPerSecond;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Overrides

/* FOdysseyAnimationEditorGUI*
FOdysseyAnimationEditorExtension::GetGUI()
{
	if (!mGUI)
		mGUI = MakeShareable(new FOdysseyAnimationEditorGUI(GetEditor()));
	return mGUI.Get();
}

TSharedPtr<FWorkspaceItem>
FOdysseyAnimationEditorExtension::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
    TSharedPtr<FWorkspaceItem> workspaceMenuCategory = iTabManager->AddLocalWorkspaceMenuCategory(LOCTEXT("WorkspaceMenu_OdysseyAnimationEditor", "Odyssey Animation2D Editor"));
	TSharedRef<FWorkspaceItem> workspaceMenuCategoryRef = workspaceMenuCategory.ToSharedRef();
	GetGUI()->RegisterTabSpawners(iTabManager, workspaceMenuCategoryRef);
	return workspaceMenuCategory;
} */

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditorExtension::OnLayerStackElementMediaChanged()
{
	GetEditor()->RefreshCurrentTool(); //Refresh the current tool
}

void
FOdysseyAnimationEditorExtension::OnImageRenderingCompositionCommited(const FGuid& iFrameId)
{
    UOdysseyAnimation* animation = Animation();
	if (!animation)
		return;

	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	TArray<FGuid> imageRenderingComposition = imageRenderAbility->GetComposition(animation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;

	GetEditor()->RefreshCurrentTool();
}

void
FOdysseyAnimationEditorExtension::OnCurrentFrameChanged(UOdysseyAnimation* iAnimation)
{
    UOdysseyAnimation* animation = Animation();
	if (iAnimation != animation)
		return;

	//Preload the new current frame for edition
	TSharedPtr<IOdysseyAnimationImageRenderingAbility> imageRenderAbility = animation->GetAbility<IOdysseyAnimationImageRenderingAbility>();
	TArray<FGuid> imageRenderingComposition = imageRenderAbility->GetComposition(animation->CurrentFrame, IOdysseyImageRenderer::eRenderType::Render);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;
	GetEditor()->RefreshCurrentTool();
}

void
FOdysseyAnimationEditorExtension::OnCurrentLayerChanged(UOdysseyLayerStack* iLayerStack)
{
	if ( iLayerStack != LayerStack() )
		return;

    Timeline()->SetSelectedFrames(FInt32Range()); //Clear Selected frames when changing layer
}

#undef LOCTEXT_NAMESPACE