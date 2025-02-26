// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAnimationEditorExtension.h"

#include "OdysseyAnimationEditorGUI.h"
#include "OdysseyAnimationEditorSource.h"
#include "OdysseyLayerStackEditorBrushContext.h"
#include "Framework/Application/SlateApplication.h"
#include "OdysseyAnimation.h"
#include "OdysseyAnimationEditorFlipSystem.h"
#include "OdysseyLayer.h"
#include "OdysseyPainterEditor.h"
#include "Shortcuts/Global/OdysseyAnimationGlobalShortcuts.h"
#include "Tools/RasterPaintBucketTool/OdysseyAnimationEditorRasterPaintBucketToolSourceProvider.h"
#include "Tools/OutOfPegsTool/OdysseyAnimationEditorOutOfPegsTool.h"
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/RasterPaintBucketTool/OdysseyPainterEditorRasterPaintBucketTool.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction

FOdysseyAnimationEditorExtension::~FOdysseyAnimationEditorExtension()
{
}

FOdysseyAnimationEditorExtension::FOdysseyAnimationEditorExtension(FOdysseyPainterEditor* iEditor)
    : FOdysseyPainterEditorExtension(iEditor)
    , mAnimationSource(nullptr)
    , mGUI(nullptr)
    , mTimelinePosition(MakeShared<FOdysseyAnimationEditorTimelinePosition>())
    , mFlipSystem(MakeShared<FOdysseyAnimationEditorFlipSystem>(this))
    , mPlaybackFramesPerSecond(0)
    , mLayerStackBrushEditorContext(MakeShared<FOdysseyLayerStackEditorBrushContext>(nullptr))
    , mOutOfPegsTool(nullptr)
{
}

void
FOdysseyAnimationEditorExtension::Initialize()
{
    GetEditor()->GetShortcuts().Add(MakeShared<FOdysseyAnimationGlobalShortcuts>(AsShared()));

    GetEditor()->OnSourceChanged().AddRaw(this, &FOdysseyAnimationEditorExtension::OnSourceChanged);
    mGUI = MakeShareable(new FOdysseyAnimationEditorGUI(this));
    mGUI->Initialize();

    mEditor->SetVectorHUDFlags( FOdysseyVectorHUD::HUD_MODE_OBJECT
                              | FOdysseyVectorHUD::HUD_MODE_OBJECT_ALLOWED
                              | FOdysseyVectorHUD::HUD_MODE_VERTEX_ALLOWED
                              | FOdysseyVectorHUD::HUD_MODE_INBETWEEN_ALLOWED );

    mOutOfPegsTool = GetEditor()->AddTool<UOdysseyAnimationEditorOutOfPegsTool>();

    FSlateApplication::Get().RegisterInputPreProcessor(mFlipSystem);
}

void
FOdysseyAnimationEditorExtension::Finalize()
{
    FSlateApplication::Get().UnregisterInputPreProcessor(mFlipSystem);
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

        mTimelinePosition = MakeShared<FOdysseyAnimationEditorTimelinePosition>();

        UOdysseyAnimation::OnCurrentFrameChanged().RemoveAll(this);
        FOdysseyImageRenderingAbility::OnImageRenderingChangedDelegate().RemoveAll(this);
        UOdysseyLayer::OnMediaChanged().RemoveAll(this);
        return;
    }

    mAnimationSource = StaticCastSharedPtr<FOdysseyAnimationEditorSource>(source);

    UOdysseyAnimation* animation = Animation();

    mTimelinePosition = MakeShared<FOdysseyAnimationEditorTimelinePosition>();

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

TSharedRef<FOdysseyAnimationEditorTimelinePosition>
FOdysseyAnimationEditorExtension::TimelinePosition()
{
    return mTimelinePosition;
}

float
FOdysseyAnimationEditorExtension::PlaybackFramesPerSecond() const
{
    return mPlaybackFramesPerSecond;
}

TSharedPtr<FOdysseyAnimationEditorFlipSystem>
FOdysseyAnimationEditorExtension::FlipSystem() const
{
    return mFlipSystem;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------- Events

void
FOdysseyAnimationEditorExtension::OnLayerMediaChanged()
{
    GetEditor()->SanitizeCurrentTool(); //Refresh the current tool
}

void
FOdysseyAnimationEditorExtension::OnImageRenderingChanged(const FOdysseyImageRenderingChangedEvent& iEvent)
{
    TRACE_CPUPROFILER_EVENT_SCOPE(FOdysseyAnimationEditorExtension::OnImageRenderingChanged);
    if (iEvent.IsInteractive() || iEvent.GetType() != FOdysseyImageRenderingChangedEvent::eEventType::kCompositionChange)
        return;

    UOdysseyAnimation* animation = Animation();
    if (!animation)
        return;

    TArray<FGuid> imageRenderingComposition = animation->GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType::Render, animation->CurrentFrame);
    if ( imageRenderingComposition == mImageRenderingComposition )
        return;

    mImageRenderingComposition = imageRenderingComposition;

    GetEditor()->SanitizeCurrentTool();
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
    GetEditor()->SanitizeCurrentTool();
}

void
FOdysseyAnimationEditorExtension::ConfigureTools()
{
    TSharedPtr<FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider> provider = MakeShared<FOdysseyAnimationEditorRasterPaintBucketToolSourceProvider>(this);
    GetEditor()->GetRasterPaintBucketTool()->SetSourceProvider(provider);
}//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

UOdysseyAnimationEditorOutOfPegsTool*
FOdysseyAnimationEditorExtension::GetOutOfPegsTool() const
{
    return mOutOfPegsTool;
}
