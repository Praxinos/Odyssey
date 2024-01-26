// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "AnimationEditor/OdysseyAnimationEditorExtension.h"

#include "PainterEditor/OdysseyPainterEditor.h"
#include "OdysseyAnimation.h"
#include "OdysseyLayer.h"
#include "AnimationEditor/OdysseyAnimationEditorGUI.h"
#include "AnimationEditor/OdysseyAnimationEditorSource.h"
#include "Tools/RasterPaintBucketTool/OdysseyAnimationEditorRasterPaintBucketToolSourceProvider.h"
#include "BrushContext/OdysseyLayerStackEditorBrushContext.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyAnimationEditorExtension::~FOdysseyAnimationEditorExtension()
{
}

FOdysseyAnimationEditorExtension::FOdysseyAnimationEditorExtension(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorExtension(iEditor)
	, mAnimationSource(nullptr)
	, mGUI(nullptr)
	, mTimeline(this)
	, mPlaybackFramesPerSecond(0)
	, mLayerStackBrushEditorContext(MakeShared<FOdysseyLayerStackEditorBrushContext>(nullptr))
{
}

void
FOdysseyAnimationEditorExtension::Initialize()
{
    GetEditor()->GetShortcuts().Add(MakeShared<FOdysseyAnimationGlobalShortcuts>(AsShared()));

    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnSourceChanged);
	mGUI = MakeShareable(new FOdysseyAnimationEditorGUI(this));
	mGUI->Initialize();
}

void
FOdysseyAnimationEditorExtension::Finalize()
{
	mGUI->Finalize();
    GetEditor()->OnSourceChanged().RemoveAll(this);

	//Ensure all deleates are removed
	mAnimationSource = nullptr;
	UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll(this);
	FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);
	UOdysseyLayer::OnMediaChanged().RemoveAll(this);
}

void
FOdysseyAnimationEditorExtension::BuildLayout(FOdysseyEditorLayoutBuilder& iBuilder)
{
	mGUI->BuildLayout(iBuilder);
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
		GetEditor()->GetBrushContexts().Remove(mLayerStackBrushEditorContext.Get());
		mAnimationSource = nullptr;
		mTimeline.Finalize();
		UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll(this);
		FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);
        UOdysseyLayer::OnMediaChanged().RemoveAll(this);
		return;
	}

	mAnimationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);

	mTimeline.Initialize();

    UOdysseyAnimation* animation = Animation();
	mImageRenderingComposition = animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
	mPlaybackFramesPerSecond = animation->GetFramesPerSecond();

	//Set Media player and Animation callbacks
	UOdysseyAnimation::OnCurrentFrameChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnCurrentFrameChanged);
	FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().AddRaw(this, &FOdysseyAnimationEditorExtension::OnImageRenderingChanged);
	UOdysseyLayer::OnMediaChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnLayerMediaChanged);

	GetEditor()->GetBrushContexts().Add(mLayerStackBrushEditorContext.Get());
	mLayerStackBrushEditorContext->SetLayerStack(mAnimationSource->GetLayerStack());

	ConfigureTools();
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
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditorExtension::OnLayerMediaChanged()
{
	GetEditor()->RefreshCurrentTool(); //Refresh the current tool
}

void
FOdysseyAnimationEditorExtension::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
	if (iEvent.IsInteractive() || iEvent.GetType() != FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
		return;

    UOdysseyAnimation* animation = Animation();
	if (!animation)
		return;

	TArray<FGuid> imageRenderingComposition = animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
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
	TArray<FGuid> imageRenderingComposition = animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
	if ( imageRenderingComposition == mImageRenderingComposition )
		return;

	mImageRenderingComposition = imageRenderingComposition;
	GetEditor()->RefreshCurrentTool();
}

void
FOdysseyAnimationEditorExtension::ConfigureTools()
{
	TSharedPtr<FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider> provider = MakeShared<FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider>(this);
	GetEditor()->GetRasterPaintBucketTool()->SetSourceProvider(provider);
}
