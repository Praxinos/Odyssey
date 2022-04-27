// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "SequencerSettings.h"
#include "Styling/StyleColors.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Tracks/MovieSceneVisibilityTrack.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "EposSequenceHelpers.h"
#include "EposTracksToolbarHelpers.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardWidgets/SMetaKeysArea.h"
#include "NamingConvention.h"
#include "PlaneActor.h"
#include "Tools/LighttableTools.h"
#include "Tools/ResourceAssetTools.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionPlanes"

//---

class SInlineEditableTextBlockOnDoubleClick
    : public SInlineEditableTextBlock
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
};

FReply
SInlineEditableTextBlockOnDoubleClick::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    SInlineEditableTextBlock::OnMouseButtonDoubleClick( InMyGeometry, InMouseEvent );

    EnterEditingMode();

    return FReply::Handled();
}

class SKeysOverviewBox
    : public SBox
{
public:
    // no SLATE_ARGS
    // Because in the construct, we don't have to manage the SBox ones (HAlign, Padding, Content, ...)
    // Otherwise we need to have HAlign::Halign( SBox::HAlign ), ...
    // To avoid this for this specific widget, iPlaneBinding is given as a parameter instead of an arg
    // So we can only call SBox::Construct( SBox::FArguments( InArgs ) );

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection, FMovieScenePossessable iPlaneBinding );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
};

void
SKeysOverviewBox::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection, FMovieScenePossessable iPlaneBinding )
{
    mBoardSection = iBoardSection;
    mBinding = iPlaneBinding;

    SBox::Construct( SBox::FArguments( InArgs ) );
}

int32
SKeysOverviewBox::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        &background_brush,
        ESlateDrawEffect::None,
        background_brush.GetTint( InWidgetStyle )
    );

    if( !mBoardSection.IsValid() )
        return SBox::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    TSharedPtr<const FMetaChannel> material_meta_channel = board_section->GetPlaneMaterialMetaChannel( mBinding );
    TSharedPtr<const FMetaChannel> transform_meta_channel = board_section->GetPlaneTransformMetaChannel( mBinding );
    TSharedPtr<const FMetaChannel> opacity_meta_channel = board_section->GetPlaneOpacityMetaChannel( mBinding );

    auto FillKeyPositions = [=]( TSharedPtr<const FMetaChannel> iMetaChannel, TArray<float>& iKeyPositions )
    {
        for( const auto& pair : iMetaChannel->GetMetaKeys() )
        {
            FFrameNumber time = pair.Key;
            //FMetaKey meta_key = pair.Value;

            FFrameTime outer_time = time * inner_to_outer_transform;
            double outer_second = FQualifiedFrameTime( outer_time, movie_scene->GetTickResolution() ).AsSeconds();

            iKeyPositions.AddUnique( converter.SecondsToPixel( outer_second ) );
        }
    };

    TArray<float> key_positions_in_pixel;
    FillKeyPositions( material_meta_channel, key_positions_in_pixel );
    FillKeyPositions( transform_meta_channel, key_positions_in_pixel );
    FillKeyPositions( opacity_meta_channel, key_positions_in_pixel );

    //---

    for( float key_position : key_positions_in_pixel )
    {
        const float offset_top_y = 1.f;
        const float offset_bottom_y = 2.f;
        const float full_height = AllottedGeometry.GetLocalSize().Y;
        const FVector2D KeyMarkSize = FVector2D( 3.f, full_height - ( offset_top_y + offset_bottom_y ) );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeyMarkSize, FSlateLayoutTransform( FVector2D( key_position - FMath::CeilToFloat( KeyMarkSize.X / 2.f ), offset_top_y ) ) ),
            FAppStyle::Get().GetBrush( "Sequencer.KeyMark" ),
            ESlateDrawEffect::None,
            FLinearColor( 1.f, 1.f, 1.f, 1.f )
        );
    }

    LayerId++;

    return SBox::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

static
void
ToggleKeysAreaVisibility( UMovieSceneCinematicBoardSection* iBoardSection, TArray<FGuid> iPlaneBindings, FGuid iPlaneReference )
{
    // This is the current plane which is the reference state
    bool is_reference_visible = iBoardSection->IsPlaneKeysAreaVisible( iPlaneReference );

    for( auto plane_binding : iPlaneBindings )
    {
        if( is_reference_visible )
        {
            if( iBoardSection->IsPlaneKeysAreaVisible( plane_binding ) )
                iBoardSection->TogglePlaneKeysAreaVisibility( plane_binding ); // If the master is expanded, collapse all expanded planes
        }
        else
        {
            if( !iBoardSection->IsPlaneKeysAreaVisible( plane_binding ) )
                iBoardSection->TogglePlaneKeysAreaVisibility( plane_binding ); // If the master is collapsed, expand all collpased planes
        }
    }
};

//---
//---
//---

class SCinematicBoardSectionPlaneTitle
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneTitle )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    ~SCinematicBoardSectionPlaneTitle();

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
    void MovieSceneDataChanged( EMovieSceneDataChangeType iType );

    void                TogglePlaneVisibility();
    bool                IsPlaneVisible() const;

    void                ToggleLighttable();
    bool                IsLighttableOn() const;

    FSlateColor         GetBackgroundTint() const;

    FText               HandleTitleText() const;
    void                HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType );

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    /** Delegate binding handle for ISequencer::OnMovieSceneDataChanged */
    FDelegateHandle mMovieSceneDataChangedHandle;

    TArray<FDrawing> mCachedDrawings;
};

//---

SCinematicBoardSectionPlaneTitle::~SCinematicBoardSectionPlaneTitle()
{
    if( mBoardSection.IsValid() && mBoardSection.Pin()->GetSequencer().IsValid() )
        mBoardSection.Pin()->GetSequencer()->OnMovieSceneDataChanged().Remove( mMovieSceneDataChangedHandle );
}

void
SCinematicBoardSectionPlaneTitle::MovieSceneDataChanged( EMovieSceneDataChangeType iType )
{
    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );

    //---

    TArray<FDrawing> cached_drawings = ShotSequenceHelpers::GetAllDrawings( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

    // To not call for every ticks when the section is resizing
    if( mCachedDrawings != cached_drawings )
    {
        mCachedDrawings = cached_drawings;

        LighttableTools::Update( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
    }

    //---

    ShotSequenceHelpers::FFindOrCreatePlaneVisibilityResult plane_visibility_result = ShotSequenceHelpers::FindPlaneVisibilityTrackAndSections( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

    if( plane_visibility_result.mTrack.IsValid() && plane_visibility_result.mSections.Num() == 1 )
    {
        if( plane_visibility_result.mSections[0]->GetTrueRange() != result.mInnerMovieScene->GetPlaybackRange() )
        {
            plane_visibility_result.mSections[0]->SetRange( result.mInnerMovieScene->GetPlaybackRange() );
        }
    }
}

//---

void
SCinematicBoardSectionPlaneTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;
    check( mBinding.GetGuid().IsValid() );
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    mMovieSceneDataChangedHandle = mBoardSection.Pin()->GetSequencer()->OnMovieSceneDataChanged().AddSP( this, &SCinematicBoardSectionPlaneTitle::MovieSceneDataChanged ); //TODO: or do it elsewhere ? in the USection/UTrack/... ?

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    //-

    auto ToggleKeysAreaVisibility = [this]()
    {
        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *section_object, sequencer->GetFocusedTemplateID() );
        TArray<FGuid> plane_bindings;
        ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

        if( plane_bindings.Contains( mBinding.GetGuid() ) )
            ::ToggleKeysAreaVisibility( section_object, plane_bindings, mBinding.GetGuid() );
        else
            section_object->TogglePlaneKeysAreaVisibility( mBinding.GetGuid() );
    };

    auto GetKeysAreaTooltip = [this]() -> FText
    {
        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        if( section_object->IsPlaneKeysAreaVisible( mBinding.GetGuid() ) )
            return LOCTEXT( "hide-plane-keys-area-tooltip", "Hide keys area" );
        else
            return LOCTEXT( "show-plane-keys-area-tooltip", "Show keys area" );
    };

    auto GetKeysAreaIcon = [this]() -> FSlateIcon
    {
        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        if( section_object->IsPlaneKeysAreaVisible( mBinding.GetGuid() ) )
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "TreeArrow_Expanded" );
        else
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "TreeArrow_Collapsed" );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( ToggleKeysAreaVisibility )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetKeysAreaTooltip ),
        MakeAttributeLambda( GetKeysAreaIcon ) );

    //-

    auto GetPlaneActorVisibilityTooltip = [this]() -> FText
    {
        if( IsPlaneVisible() )
            return LOCTEXT( "hide-plane-actor-tooltip", "Hide plane actor" );
        else
            return LOCTEXT( "show-plane-actor-tooltip", "Show plane actor" );
    };

    auto GetPlaneActorVisibilityIcon = [this]() -> FSlateIcon
    {
        if( IsPlaneVisible() )
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.VisibleIcon16x" );
        else
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.NotVisibleIcon16x" );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateRaw( this, &SCinematicBoardSectionPlaneTitle::TogglePlaneVisibility )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetPlaneActorVisibilityTooltip ),
        MakeAttributeLambda( GetPlaneActorVisibilityIcon ) );

    //-

    auto DetachPlane = [this]()
    {
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection_object, sequencer->GetFocusedTemplateID() );
        TArray<FGuid> plane_bindings;
        ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

        if( plane_bindings.Contains( mBinding.GetGuid() ) )
            BoardSequenceTools::DetachPlane( sequencer, subsection_object, plane_bindings );
        else
            BoardSequenceTools::DetachPlane( sequencer, subsection_object, mBinding.GetGuid() );
    };

    auto CanDetachPlane = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        return BoardSequenceTools::CanDetachPlane( sequencer, subsection_object, mBinding.GetGuid() );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( DetachPlane ),
            FCanExecuteAction::CreateLambda( CanDetachPlane ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( [=](){ return CanDetachPlane(); } )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "DetachPlane", "Detach the plane" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "DetachPlane" ) );

    //-

    auto CreateDrawing = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection_object, sequencer->GetFocusedTemplateID() );
        TArray<FGuid> plane_bindings;
        ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

        if( plane_bindings.Contains( mBinding.GetGuid() ) )
            BoardSequenceTools::CreateDrawing( sequencer, subsection_object, local_frame, plane_bindings );
        else
            BoardSequenceTools::CreateDrawing( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    auto CanCreateDrawing = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateDrawing( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( CreateDrawing ),
            FCanExecuteAction::CreateLambda( CanCreateDrawing )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "create-drawing", "Create a drawing (set the current frame where to create the drawing keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "CreateDrawing" ) );

    //-

    auto GetLighttableTooltip = [this]() -> FText
    {
        if( IsLighttableOn() )
            return LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable" );
        else
            return LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable" );
    };

    auto GetLighttableIcon = [this]() -> FSlateIcon
    {
        if( IsLighttableOn() )
            return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
        else
            return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateRaw( this, &SCinematicBoardSectionPlaneTitle::ToggleLighttable )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetLighttableTooltip ),
        MakeAttributeLambda( GetLighttableIcon ) );

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();
    left_toolbar->SetVisibility( mOptionalWidgetsVisibility );

    //---

    FSlimHorizontalToolBarBuilder RightToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    RightToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    //-

    auto IsWarning = [this]() -> bool
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

        //-

        ShotSequenceHelpers::FFindOrCreatePlaneVisibilityResult plane_visibility_result = ShotSequenceHelpers::FindPlaneVisibilityTrackAndSections( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

        if( !plane_visibility_result.mTrack.IsValid() )
            return false;

        if( plane_visibility_result.mSections.Num() == 0 )
            return false;

        TRange<FFrameNumber> full_range( TRange<FFrameNumber>::Empty() );
        for( auto section : plane_visibility_result.mSections )
            full_range = TRange<FFrameNumber>::Hull( full_range, section->GetTrueRange() );

        return full_range != result.mInnerMovieScene->GetPlaybackRange();
    };

    auto GetWarningTooltip = [this]() -> FText
    {
        return LOCTEXT( "warning-tooltip", "Warning: the plane visibility track doesn't match the shot length, set it maually" );
    };

    RightToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( IsWarning )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetWarningTooltip ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Icons.Warning" ) );

    TSharedRef< SWidget > right_toolbar = RightToolbarBuilder.MakeWidget();
    right_toolbar->SetVisibility( mOptionalWidgetsVisibility );

    //---

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( FAppStyle::Get().GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" ) )
        .BorderBackgroundColor( this, &SCinematicBoardSectionPlaneTitle::GetBackgroundTint )
        [
            SNew( SVerticalBox )

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                SNew( SHorizontalBox )
                + SHorizontalBox::Slot()
                .FillWidth( .5f )
                [
                    left_toolbar
                ]
                + SHorizontalBox::Slot()
                .AutoWidth()
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    SNew( SInlineEditableTextBlockOnDoubleClick )
                    .Text( this, &SCinematicBoardSectionPlaneTitle::HandleTitleText )
                    .OnTextCommitted( this, &SCinematicBoardSectionPlaneTitle::HandleTitleTextOnCommited )
                ]
                + SHorizontalBox::Slot()
                .FillWidth( .5f )
                .HAlign( HAlign_Right )
                [
                    right_toolbar
                ]
            ]
        ]
    ];
}

//---

FCursorReply
SCinematicBoardSectionPlaneTitle::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionPlaneTitle::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    // To not trigger the OnMouseButtonDown of SSequencerTrackArea
    // Otherwise OnMouseButtonMove and OnMouseButtonUp will also trigger
    // And OnMouseButtonDown will attempt to start a selection or a drag of the section (normal behavior)
    // But as OnMouseButtonUp is handle here, the one of SSequencerTrackArea won't be handle and the normal section drag won't finish clean
    // (For example, the cursor won't update to crosshair after the up on the empty zone of the SSequencerTrackArea)
    //return FReply::Handled();

    // To be able to move the section through a title plane, otherwise (Handled) it's no more possible
    // Let's see if it's a problem to not handled now (see the comment above)
    // (Or maybe process the Up here ? to avoid this problem ? but in this case, it should also be unhandled to allow SSequencerTrackArea to manage the drag section)
    return FReply::Unhandled();
}

FReply
SCinematicBoardSectionPlaneTitle::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        UMovieSceneSection* section_object = board_section->GetSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        if( MouseEvent.IsControlDown() )
            BoardSequenceTools::SelectMultiPlane( sequencer, subsection_object, mBinding.GetGuid() );
        else
            BoardSequenceTools::SelectSinglePlane( sequencer, subsection_object, mBinding.GetGuid() );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

//---

FText
SCinematicBoardSectionPlaneTitle::HandleTitleText() const
{
    return FText::FromString( mBinding.GetName() );
}

void
SCinematicBoardSectionPlaneTitle::HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType )
{
    if( iType != ETextCommit::OnEnter )
        return;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceTools::RenameBinding( sequencer, *subsection_object, mBinding.GetGuid(), iText.ToString() );

    //---

    UMovieSceneSequence* sequence = subsection_object->GetSequence();
    UMovieScene* movie_scene = sequence ? sequence->GetMovieScene() : nullptr;
    FMovieScenePossessable* possessable = movie_scene ? movie_scene->FindPossessable( mBinding.GetGuid() ) : nullptr;
    if( possessable )
        mBinding = *possessable; // Update the cached one
}

FSlateColor
SCinematicBoardSectionPlaneTitle::GetBackgroundTint() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    auto objects = sequencer->FindBoundObjects( mBinding.GetGuid(), result.mInnerSequenceId );

    auto is_selected = []( TWeakObjectPtr<> iObject )
    {
        return iObject.IsValid() ? iObject->IsSelected() : false;
    };
    // Same as in ...\Engine\Source\Editor\Sequencer\Private\SAnimationOutlinerTreeNode.cpp::GetNodeBackgroundTint()
    if( Algo::AnyOf( objects, is_selected ) )
        return FStyleColors::Select;

    return FStyleColors::Header;
}

//---

void
SCinematicBoardSectionPlaneTitle::TogglePlaneVisibility()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    TArray<FGuid> plane_bindings;
    ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

    if( plane_bindings.Contains( mBinding.GetGuid() ) )
        BoardSequenceTools::TogglePlaneVisibility( sequencer, *subsection_object, plane_bindings, mBinding.GetGuid() );
    else
        BoardSequenceTools::TogglePlaneVisibility( sequencer, *subsection_object, mBinding.GetGuid() );
}

bool
SCinematicBoardSectionPlaneTitle::IsPlaneVisible() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    return BoardSequenceTools::IsPlaneVisible( sequencer, *subsection_object, mBinding.GetGuid() );
}

//---

void
SCinematicBoardSectionPlaneTitle::ToggleLighttable()
{
    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
    ISequencer*                     sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    TArray<FGuid> plane_bindings;
    ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

    // This is the current plane which is the reference state
    bool is_reference_on = LighttableTools::IsOn( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

    if( plane_bindings.Contains( mBinding.GetGuid() ) )
    {
        for( auto plane_binding : plane_bindings )
        {
            if( is_reference_on )
                LighttableTools::Deactivate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, plane_binding );
            else
                LighttableTools::Activate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, plane_binding );
        }
    }
    else
    {
        if( is_reference_on )
            LighttableTools::Deactivate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
        else
            LighttableTools::Activate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
    }
}

bool
SCinematicBoardSectionPlaneTitle::IsLighttableOn() const
{
    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
    ISequencer*                     sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

    //---

    // IsLighttableOn() is not called every ticks
    // The button (which calls IsLighttableOn()) is only displayed when the mouse hovers the corresponding section
    // So it 's not a real problem while IsLighttableOn() is only called on buttons which are not displayed all the time
    return LighttableTools::IsOn( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
}

//---
//---
//---

class SCinematicBoardSectionPlaneKeys
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneKeys )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SMetaKeysArea overrides
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() override;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const override;
    virtual void                            RebuildMetaChannel() override;

    //virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) override;

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const override;
    virtual FText GetAreaTooltipText() const override;

    virtual const FSlateBrush* GetBackgroundBrush() const override;

private:
    FMovieScenePossessable mBinding;
};

void
SCinematicBoardSectionPlaneKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
    ];
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionPlaneKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetPlaneTransformMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionPlaneKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetPlaneTransformMetaChannel( mBinding );
}

void
SCinematicBoardSectionPlaneKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildPlanesTransformMetaChannel();
}

//---

FText
SCinematicBoardSectionPlaneKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    return GetAreaTooltipText();
}

FText
SCinematicBoardSectionPlaneKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText plane_text = FText::Format( LOCTEXT( "tooltip-plane-transform-area-plane-name", "Plane: {0}" ), plane_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-plane-transform-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), plane_text, num_keys_text );
}

//---

FCursorReply
SCinematicBoardSectionPlaneKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionPlaneKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionPlaneKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class SCinematicBoardSectionPlaneMaterialKeys
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneMaterialKeys )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SMetaKeysArea overrides
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() override;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const override;
    virtual void                            RebuildMetaChannel() override;

    virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) override;

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const override;
    virtual FText GetAreaTooltipText() const override;

    virtual const FSlateBrush* GetBackgroundBrush() const override;

private:
    FMovieScenePossessable              mBinding;
};

void
SCinematicBoardSectionPlaneMaterialKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
    ];
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionPlaneMaterialKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetPlaneMaterialMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionPlaneMaterialKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetPlaneMaterialMetaChannel( mBinding );
}

void
SCinematicBoardSectionPlaneMaterialKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildPlanesMaterialMetaChannel();
}

//---

bool
SCinematicBoardSectionPlaneMaterialKeys::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
{
    auto CloneKey = [=]( TSharedPtr<FMetaChannel> iKeys )
    {
        if( iKeys->NumMetaKeys() != 1 ) // For the moment, only 1 metakey can be cloned
            return;

        auto it = iKeys->GetMetaKeys().CreateConstIterator();
        if( it.Value().mSubKeys.Num() != 1 ) // For the moment, only 1 subkey can be cloned
            return;

        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                BoardSequenceTools::CloneDrawing( sequencer, *subsection_object, subkey.mSection.Get(), subkey.mChannelHandle, subkey.mKeyHandle, local_frame );

                break; // Only 1 key for the moment, and otherwise, CloneDrawing should take 3 arrays like DeleteDrawing
            }
        }
    };

    auto CanCloneKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        if( iKeys->NumMetaKeys() != 1 ) // For the moment, only 1 metakey can be cloned
            return false;

        auto it = iKeys->GetMetaKeys().CreateConstIterator();
        if( it.Value().mSubKeys.Num() != 1 ) // For the moment, only 1 subkey can be cloned
            return false;

        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        return BoardSequenceTools::CanCloneDrawing( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    //-

    auto DeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        TArray<TWeakObjectPtr<UMovieSceneSection>> sections;
        TArray<FMovieSceneChannelHandle> channelHandles;
        TArray<FKeyHandle> keyHandles;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                sections.Add( subkey.mSection );
                channelHandles.Add( subkey.mChannelHandle );
                keyHandles.Add( subkey.mKeyHandle );
            }
        }

        BoardSequenceTools::DeleteDrawing( sequencer, *subsection_object, sections, channelHandles, keyHandles );
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        return true;
    };

    //-

    auto EditKey = [=]( UTexture2D* iTexture )
    {
        if( !iTexture )
            return;

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        check( !AssetEditorSubsystem->FindEditorsForAsset( iTexture ).Num() );

        AssetEditorSubsystem->OpenEditorForAsset( iTexture );
    };

    auto CanEditKey = [=]( UTexture2D* iTexture ) -> bool
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

        TArray<IAssetEditorInstance*> opened_editors = AssetEditorSubsystem->FindEditorsForAsset( iTexture );
        //FName name = opened_editors.Num() ? opened_editors[0]->GetEditorName() : NAME_None;

        return !opened_editors.Num();
    };

    //-

    TMap<FString, FString> map; // Maybe use a TMultiMap if we want to display multiple textures inside 1 material
    TArray<UTexture2D*> textures;
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            FDrawing drawing = ShotSequenceHelpers::ConvertToDrawing( subkey.mSection, subkey.mChannelHandle, subkey.mKeyHandle );

            UMaterialInstance* material = drawing.GetMaterial();
            UTexture2D* texture = ProjectAssetTools::GetTexture2D( nullptr, material );

            if( texture )
                textures.Add( texture );

            if( material )
            {
                map.Add( material->GetName(), texture ? texture->GetName() : TEXT( "" ) );
            }
        }
    }

    TArray<FString> material_names;
    for( const auto& pair : map )
        material_names.Add( pair.Key );

    FString material_name = FString::Join( material_names, TEXT(", ") );

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    if( textures.Num() > 1 )
    {
        ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "texture-multi-key-section-label", "Textures" ) );

        for( auto texture : textures )
        {
            ioMenuBuilder.AddMenuEntry( FText::Format( LOCTEXT( "edit-texture-multi-key-label", "Edit {0}..." ), FText::FromString( texture->GetName() ) ),
                                        LOCTEXT( "edit-texture-multi-key-tooltip", "Edit the texture of the current key with its default editor\n(If it's not possible, the texture is already opened)" ),
                                        FSlateIcon(),
                                        FUIAction( FExecuteAction::CreateLambda( EditKey, texture ),
                                                   FCanExecuteAction::CreateLambda( CanEditKey, texture ) ) );
        }

        ioMenuBuilder.EndSection();
    }
    else if( textures.Num() == 1 )
    {
        ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "texture-key-section-label", "Texture" ) );
        //ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "texture-key-section-label", "Texture: {0}" ), FText::FromString( textures[0]->GetName() ) ) );

        ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-texture-key-label", "Edit..." ),
                                    LOCTEXT( "edit-texture-key-tooltip", "Edit the texture of the current key with its default editor\n(If it's not possible, the texture is already opened)" ),
                                    FSlateIcon(),
                                    FUIAction( FExecuteAction::CreateLambda( EditKey, textures[0] ),
                                               FCanExecuteAction::CreateLambda( CanEditKey, textures[0] ) ) );

        ioMenuBuilder.EndSection();
    }

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "material-key-section-label", "Material" ) );
    //ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "material-key-section-label", "Material: {0}" ), FText::FromString( material_name ) ) );

    ioMenuBuilder.AddMenuEntry( FText::Format( LOCTEXT( "clone-material-key-label", "Clone at {0}" ), FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) ) ),
                                LOCTEXT( "clone-material-key-tooltip", "Clone the current key (material and texture) at the current frame" ),
                                FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Duplicate" ),
                                FUIAction( FExecuteAction::CreateLambda( CloneKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanCloneKey, iKeys ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-material-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for all the channels -> see camera key delete
                                LOCTEXT( "delete-material-key-tooltip", "Delete the current key" ),
                                FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, iKeys ) ) );

    ioMenuBuilder.EndSection();

    return true;
}

FText
SCinematicBoardSectionPlaneMaterialKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    TMap<FString, FString> map; // Maybe use a TMultiMap if we want to display multiple textures inside 1 material
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            FDrawing drawing = ShotSequenceHelpers::ConvertToDrawing( subkey.mSection, subkey.mChannelHandle, subkey.mKeyHandle );

            UMaterialInstance* material = drawing.GetMaterial();
            UTexture2D* texture = ProjectAssetTools::GetTexture2D( nullptr, material );

            if( material )
            {
                map.Add( material->GetName(), texture ? texture->GetName() : TEXT( "" ) );
            }
        }
    }

    TArray<FText> lines;
    lines.Add( FText::Format( LOCTEXT( "tooltip-plane-material-key-plane-name", "Plane: {0}" ), plane_track_text ) );
    for( const auto& pair : map )
    {
        lines.Add( FText::Format( LOCTEXT( "tooltip-plane-material-key-value-material", "Material: {0}" ), FText::FromString( pair.Key ) ) );
        lines.Add( FText::Format( LOCTEXT( "tooltip-plane-material-key-value-texture", "Texture: {0}" ), FText::FromString( pair.Value ) ) );
    }

    return FText::Join( FText::FromString( TEXT( "\n" ) ), lines );
}

FText
SCinematicBoardSectionPlaneMaterialKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText plane_text = FText::Format( LOCTEXT( "tooltip-plane-material-area-plane-name", "Plane: {0}" ), plane_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-plane-material-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), plane_text, num_keys_text );
}

FCursorReply
SCinematicBoardSectionPlaneMaterialKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionPlaneMaterialKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionPlaneMaterialKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class SCinematicBoardSectionPlaneOpacityKeys
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneOpacityKeys )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SMetaKeysArea overrides
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() override;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const override;
    virtual void                            RebuildMetaChannel() override;

    virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) override;

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const override;
    virtual FText GetAreaTooltipText() const override;

    virtual const FSlateBrush* GetBackgroundBrush() const override;

private:
    FMovieScenePossessable              mBinding;
};

void
SCinematicBoardSectionPlaneOpacityKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
    ];
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionPlaneOpacityKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetPlaneOpacityMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionPlaneOpacityKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetPlaneOpacityMetaChannel( mBinding );
}

void
SCinematicBoardSectionPlaneOpacityKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildPlanesOpacityMetaChannel();
}

//---

bool
SCinematicBoardSectionPlaneOpacityKeys::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
{
    auto DeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        TArray<TWeakObjectPtr<UMovieSceneSection>> sections;
        TArray<FMovieSceneChannelHandle> channelHandles;
        TArray<FKeyHandle> keyHandles;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                sections.Add( subkey.mSection );
                channelHandles.Add( subkey.mChannelHandle );
                keyHandles.Add( subkey.mKeyHandle );
            }
        }

        BoardSequenceTools::DeleteOpacity( sequencer, *subsection_object, sections, channelHandles, keyHandles );
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        return true;
    };

    //-

    auto SetKey = [=]( TSharedPtr<FMetaChannel> iKeys, float iOpacity )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        TArray<TWeakObjectPtr<UMovieSceneSection>> sections;
        TArray<FMovieSceneChannelHandle> channelHandles;
        TArray<FKeyHandle> keyHandles;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                sections.Add( subkey.mSection );
                channelHandles.Add( subkey.mChannelHandle );
                keyHandles.Add( subkey.mKeyHandle );
            }
        }

        BoardSequenceTools::SetOpacity( sequencer, *subsection_object, sections, channelHandles, keyHandles, iOpacity );
    };

    auto CanSetKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        return true;
    };

    auto SetOpacitySubMenu = [=]( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        float current_opacity = -1.0;

        for( auto pair : iKeys->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                FKeyOpacity opacity_key = ShotSequenceHelpers::ConvertToOpacityKey( subkey.mSection, subkey.mChannelHandle, subkey.mKeyHandle );
                if( !opacity_key.Exists() )
                    continue;

                opacity_key.GetOpacity( current_opacity );
            }
        }

        //-

        float opacities[] = { 0.f, .1f, .2f,
                                            -1.f, //sep
                             .25f, .3f, .33f,
                                            -1.f, //sep
                             .4f, .5f, .6f,
                                            -1.f, //sep
                             .66f, .7f, .75f,
                                            -1.f, //sep
                             .8f, .9f, 1.f };
        for( float opacity : opacities )
        {
            if( opacity < -0.1f )
            {
                ioMenuBuilder.AddSeparator();
                continue;
            }

            ioMenuBuilder.AddMenuEntry(
                FText::Format( LOCTEXT( "set-drawing-opacity-0-label", "{0}" ), FText::AsPercent( opacity ) ),
                FText::Format( LOCTEXT( "set-drawing-opacity-0-tooltip", "Set the drawing opacity at {0}" ), FText::AsPercent( opacity ) ),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda( SetKey, iKeys, opacity ),
                    FCanExecuteAction::CreateLambda( CanSetKey, iKeys ),
                    FIsActionChecked::CreateLambda( [=]() { return FMath::IsNearlyEqual( opacity, current_opacity, KINDA_SMALL_NUMBER ); } )
                ),
                NAME_None,
                EUserInterfaceActionType::Check );
        }
    };

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "plane-section-label", "Plane: {0}" ), plane_track_text ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-plane-opacity-key-label", "Delete" ),
                                LOCTEXT( "delete-plane-opacity-key-tooltip", "Delete the current key" ),
                                FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, iKeys ) ) );

    ioMenuBuilder.AddSubMenu(
        LOCTEXT( "set-drawing-opacity-label", "Set Opacity" ),
        LOCTEXT( "set-drawing-opacity-tooltip", "Set the current drawing opacity" ),
        FNewMenuDelegate::CreateLambda( SetOpacitySubMenu, iKeys )
    );

    ioMenuBuilder.EndSection();

    return true;
}

FText
SCinematicBoardSectionPlaneOpacityKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    TArray<float> opacities;
    for( const auto& pair : iKeys->GetMetaKeys() )
    {
        FFrameNumber frame_number = pair.Key;
        const FMetaKey& meta_key = pair.Value;

        for( const auto& subkey : meta_key.mSubKeys )
        {
            FKeyOpacity opacity_key = ShotSequenceHelpers::ConvertToOpacityKey( subkey.mSection, subkey.mChannelHandle, subkey.mKeyHandle );
            if( !opacity_key.Exists() )
                continue;

            float opacity;
            opacity_key.GetOpacity( opacity );

            opacities.Add( opacity );
        }
    }

    TArray<FText> lines;
    lines.Add( FText::Format( LOCTEXT( "tooltip-plane-opacity-key-plane-name", "Plane: {0}" ), plane_track_text ) );
    for( auto opacity : opacities )
        lines.Add( FText::Format( LOCTEXT( "tooltip-plane-opacity-key-value", "Opacity: {0}" ), FText::AsPercent( opacity ) ) );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), lines );
}

FText
SCinematicBoardSectionPlaneOpacityKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText plane_text = FText::Format( LOCTEXT( "tooltip-plane-opacity-area-plane-name", "Plane: {0}" ), plane_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-plane-opacity-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), plane_text, num_keys_text );
}

FCursorReply
SCinematicBoardSectionPlaneOpacityKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionPlaneOpacityKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneOpacityKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneOpacityKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionPlaneOpacityKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionPlaneOpacityKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionPlane
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlane )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

private:
    void BuildContextMenu( FMenuBuilder& ioMenuBuilder );

    EVisibility GetKeysAreaVisibility() const;
    EVisibility GetKeysOverviewVisibility() const;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;
};

void
SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneTitle, iBoardSection )
            .Binding( mBinding )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SKeysOverviewBox, iBoardSection, mBinding )
            .HeightOverride( 8 )
            .Visibility( this, &SCinematicBoardSectionPlane::GetKeysOverviewVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneMaterialKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionPlane::GetKeysAreaVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionPlane::GetKeysAreaVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneOpacityKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionPlane::GetKeysAreaVisibility )
        ]
    ];
}

EVisibility
SCinematicBoardSectionPlane::GetKeysOverviewVisibility() const
{
    return GetKeysAreaVisibility().IsVisible() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SCinematicBoardSectionPlane::GetKeysAreaVisibility() const
{
    UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );

    return section_object->IsPlaneKeysAreaVisible( mBinding.GetGuid() ) ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SCinematicBoardSectionPlane::BuildContextMenu( FMenuBuilder& ioMenuBuilder )
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneCinematicBoardSection* board_section_object = Cast<UMovieSceneCinematicBoardSection>( board_section->GetSectionObject() );
    UMovieSceneSequence* inner_sequence = board_section_object->GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();
    FText current_frame_text = FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) );

    //---

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *board_section_object, sequencer->GetFocusedTemplateID() );
    TArray<FGuid> plane_bindings;
    ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kSelectedOnly, nullptr, &plane_bindings );

    if( !plane_bindings.Contains( mBinding.GetGuid() ) )
    {
        BoardSequenceTools::SelectSinglePlane( sequencer, board_section_object, mBinding.GetGuid() );

        plane_bindings = { mBinding.GetGuid() };
    }

    if( plane_bindings.Num() > 1 )
        plane_track_text = LOCTEXT( "planes-selected", "(selected)" );

    //---

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "plane-section-label", "Plane: {0}" ), plane_track_text ) );

    auto TogglePlaneVisibility = [this, plane_bindings]()
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();
        BoardSequenceTools::TogglePlaneVisibility( sequencer, subsection_object, plane_bindings, mBinding.GetGuid() );
    };

    bool is_plane_visible = BoardSequenceTools::IsPlaneVisible( sequencer, *board_section_object, mBinding.GetGuid() );
    FText label_text   = is_plane_visible ? LOCTEXT( "hide-plane-actor-label", "Hide" )                                 : LOCTEXT( "show-plane-actor-label", "Show" );
    FText tooltip_text = is_plane_visible ? LOCTEXT( "hide-plane-actor-tooltip", "Hide plane actor" )                   : LOCTEXT( "show-plane-actor-tooltip", "Show plane actor" );
    FSlateIcon icon    = is_plane_visible ? FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.VisibleIcon16x" ) : FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.NotVisibleIcon16x" );

    ioMenuBuilder.AddMenuEntry(
        label_text,
        tooltip_text,
        icon,
        FUIAction(
            FExecuteAction::CreateLambda( TogglePlaneVisibility )
        ) );

    //-

    auto DetachPlane = [this, plane_bindings]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        BoardSequenceTools::DetachPlane( sequencer, subsection_object, plane_bindings );
    };

    auto CanDetachPlane = [this, plane_bindings]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        return BoardSequenceTools::CanDetachPlane( sequencer, subsection_object, plane_bindings );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "detach-plane-label", "Detach {0}" ), plane_track_text ),
        LOCTEXT( "detach-plane-tooltip", "Detach the plane" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "DetachPlane" ),
        FUIAction(
            FExecuteAction::CreateLambda( DetachPlane ),
            FCanExecuteAction::CreateLambda( CanDetachPlane )
        ) );

    //-

    bool is_keys_area_visible = board_section_object->IsPlaneKeysAreaVisible( mBinding.GetGuid() );
    label_text   = is_keys_area_visible ? LOCTEXT( "hide-plane-keys-area-tooltip", "Collapse" )         : LOCTEXT( "show-plane-keys-area-tooltip", "Expand" );
    tooltip_text = is_keys_area_visible ? LOCTEXT( "hide-plane-keys-area-tooltip", "Hide keys area" )   : LOCTEXT( "show-plane-keys-area-tooltip", "Show keys area" );

    ioMenuBuilder.AddMenuEntry(
        label_text,
        tooltip_text,
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateStatic( ToggleKeysAreaVisibility, board_section_object, plane_bindings, mBinding.GetGuid() )
        ) );

    //-

    auto DeletePlane = [this, plane_bindings]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::DeletePlane( sequencer, subsection_object, plane_bindings );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "delete-plane-label", "Delete {0}" ), plane_track_text ),
        LOCTEXT( "delete-plane-tooltip", "Delete the plane and its corresponding actor" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
        FUIAction( FExecuteAction::CreateLambda( DeletePlane ) ) );

    ioMenuBuilder.EndSection();

    //-

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "drawing-section-label", "Drawing" ) );

    auto CreateDrawing = [this, plane_bindings]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::CreateDrawing( sequencer, subsection_object, local_frame, plane_bindings );
    };

    auto CanCreateDrawing = [this, plane_bindings]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateDrawing( sequencer, subsection_object, local_frame, plane_bindings );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "create-drawing-label", "Create {0}|plural(one=drawing,other=drawings) at {1}" ), plane_bindings.Num(), current_frame_text ),
        LOCTEXT( "create-drawing-tooltip", "Create a drawing\n(set the current frame where to create the drawing keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "CreateDrawing" ),
        FUIAction(
            FExecuteAction::CreateLambda( CreateDrawing ),
            FCanExecuteAction::CreateLambda( CanCreateDrawing )
        ) );

    //-

    auto CreateOpacity = [this, plane_bindings]( float iOpacity )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::CreateOpacity( sequencer, subsection_object, local_frame, plane_bindings, iOpacity );
    };

    auto CanCreateOpacity = [this, plane_bindings]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateOpacity( sequencer, subsection_object, local_frame, plane_bindings );
    };

    auto CreateOpacitySubMenu = [=]( FMenuBuilder& ioMenuBuilder )
    {
        float opacities[] = { 0.f, .1f, .2f,
                                            -1.f, //sep
                             .25f, .3f, .33f,
                                            -1.f, //sep
                             .4f, .5f, .6f,
                                            -1.f, //sep
                             .66f, .7f, .75f,
                                            -1.f, //sep
                             .8f, .9f, 1.f };
        for( float opacity : opacities )
        {
            if( opacity < -0.1f )
            {
                ioMenuBuilder.AddSeparator();
                continue;
            }

            ioMenuBuilder.AddMenuEntry(
                FText::Format( LOCTEXT( "create-drawing-opacity-0-label", "{0}" ), FText::AsPercent( opacity ) ),
                FText::Format( LOCTEXT( "create-drawing-opacity-0-tooltip", "Create the drawing opacity at {0}" ), FText::AsPercent( opacity ) ),
                FSlateIcon(),
                FUIAction(
                    FExecuteAction::CreateLambda( CreateOpacity, opacity ),
                    FCanExecuteAction::CreateLambda( CanCreateOpacity )
                ) );
        }
    };

    ioMenuBuilder.AddSubMenu(
        FText::Format( LOCTEXT( "create-drawing-opacity-label", "Create {0}|plural(one=opacity,other=opacities) at {1}" ), plane_bindings.Num(), current_frame_text ),
        LOCTEXT( "create-drawing-opacity-tooltip", "Create the drawing opacity\n(set the current frame where to set the opacity)" ),
        FNewMenuDelegate::CreateLambda( CreateOpacitySubMenu ),
        FUIAction( FExecuteAction(),
                   FCanExecuteAction::CreateLambda( CanCreateOpacity ) ),
        NAME_None,
        EUserInterfaceActionType::None
    );

    ioMenuBuilder.EndSection();
}

FReply
SCinematicBoardSectionPlane::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlane::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
    {
        FMenuBuilder menu_builder( true, nullptr );
        BuildContextMenu( menu_builder );

        TSharedPtr<SWidget> menu = menu_builder.MakeWidget();
        FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu( AsShared(), WidgetPath, menu.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu ) );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

//---
//---
//---

SCinematicBoardSectionPlanes::~SCinematicBoardSectionPlanes()
{
    if( mSequencer.IsValid() )
        mSequencer.Pin()->OnMovieSceneDataChanged().Remove( mRebuildPlaneListHandle );
}

void
SCinematicBoardSectionPlanes::RebuildPlaneList( EMovieSceneDataChangeType iType )
{
    mNeedRebuildPlaneList = true;
}

void
SCinematicBoardSectionPlanes::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;
    mSequencer = mBoardSection.Pin()->GetSequencer();

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    mRebuildPlaneListHandle = mSequencer.Pin()->OnMovieSceneDataChanged().AddSP( this, &SCinematicBoardSectionPlanes::RebuildPlaneList );

    check( !mPossessables.Num() );

    //---

    FSlimHorizontalToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    MiddleToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionPlanes::MakeCreatePlaneMenu ),
        FText::GetEmpty(),
        LOCTEXT( "create-plane-and-settings-tooltip", "Create a new plane" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Plus" ) );

    auto IsToolBarVisible = [this]() -> EVisibility
    {
        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();

        bool is_visible = mOptionalWidgetsVisibility.Get().IsVisible();
        is_visible &= BoardSequenceTools::CanCreatePlane( sequencer, section_object->GetInclusiveStartFrame() );

        return is_visible ? EVisibility::Visible : EVisibility::Hidden;
    };

    TSharedRef< SWidget > middle_toolbar = MiddleToolbarBuilder.MakeWidget();
    middle_toolbar->SetVisibility( MakeAttributeLambda( IsToolBarVisible ) );

    //---

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SAssignNew( mWidgetPlaneList, SListView<TSharedRef<FMovieScenePossessable>> )
            .ListItemsSource( &mPossessables )
            .OnGenerateRow( this, &SCinematicBoardSectionPlanes::MakePlaneRow )
            .SelectionMode( ESelectionMode::None )
        ]
        + SVerticalBox::Slot()
        .HAlign( HAlign_Center )
        .AutoHeight()
        [
            middle_toolbar
        ]
    ];

    RebuildPlaneList();
}

void
SCinematicBoardSectionPlanes::CreatePlane( TSharedRef<FString> iPlaneName )
{
    if( !mBoardSection.IsValid() )
        return;

    ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
    UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
    FPlaneArgs plane_args;
    plane_args.mName = *iPlaneName;
    BoardSequenceTools::CreatePlane( sequencer, section_object->GetInclusiveStartFrame(), plane_args );
}

TSharedRef<SWidget>
SCinematicBoardSectionPlanes::MakeCreatePlaneMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer.Pin()->GetCommandBindings() );

    //---

    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );

    FString plane_path;
    TSharedRef<FString> plane_name = MakeShared<FString>();
    NamingConvention::GeneratePlaneActorPathName( *sequencer, sequencer->GetRootMovieSceneSequence(), result.mInnerSequence, plane_path, *plane_name );

    //---

    EposTracksToolbarHelpers::MakePlaneEntries( MenuBuilder, plane_name, FSimpleDelegate::CreateRaw( this, &SCinematicBoardSectionPlanes::CreatePlane, plane_name ) );
    EposTracksToolbarHelpers::MakePlaneSettingsEntries( MenuBuilder );
    EposTracksToolbarHelpers::MakeTextureSettingsEntries( MenuBuilder );

    //---

    auto CreatePlaneOnClick = [this, plane_name]() -> FReply
    {
        if( !mBoardSection.IsValid() )
            return FReply::Unhandled();

        CreatePlane( plane_name );

        return FReply::Handled();
    };

    auto CanCreatePlane = [this]() -> bool
    {
        if( !mBoardSection.IsValid() )
            return false;

        //PATCH
        if( !GCurrentLevelEditingViewportClient )
            return false;
        //PATCH

        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        return BoardSequenceTools::CanCreatePlane( sequencer, section_object->GetInclusiveStartFrame() );
    };

    MenuBuilder.AddWidget( SNew( SVerticalBox )
                           + SVerticalBox::Slot()
                           .AutoHeight()
                           [
                               SNew( SHorizontalBox )
                               + SHorizontalBox::Slot()
                               .HAlign( HAlign_Center )
                               [
                                   SNew( SButton )
                                   .Text( LOCTEXT( "create-plane-label", "Create a new plane" ) )
                                   .ToolTipText( LOCTEXT( "create-plane-tooltip", "Create a new plane with those settings" ) )
                                   .OnClicked_Lambda( CreatePlaneOnClick )
                                   .IsEnabled_Lambda( CanCreatePlane )
                               ]
                           ]
                           //PATCH
                           + SVerticalBox::Slot()
                           .AutoHeight()
                           .HAlign( HAlign_Center )
                           [
                                SNew( STextBlock )
                                .Text( FText::FromString( TEXT( "/!\\ Select an actor in the viewport first /!\\" ) ) )
                                .ColorAndOpacity( FLinearColor::Yellow )
                                .Visibility_Lambda( []() -> EVisibility { return !GCurrentLevelEditingViewportClient ? EVisibility::Visible : EVisibility::Collapsed; } )
                           ],
                           //PATCH
                           FText::GetEmpty(),
                           true /* NoIndent */ );

    return MenuBuilder.MakeWidget();
}


class STableRowPlane
    : public STableRow<TSharedPtr<FString>>
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override
    {
        return FReply::Unhandled();
    }
};

TSharedRef<ITableRow>
SCinematicBoardSectionPlanes::MakePlaneRow( TSharedRef<FMovieScenePossessable> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if( !mBoardSection.IsValid() )
        return SNew( STableRowPlane, iOwnerTable );

    return
        SNew( STableRowPlane, iOwnerTable )
        [
            SNew( SCinematicBoardSectionPlane, mBoardSection.Pin().ToSharedRef() )
            .Binding( *iItem )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ];
}

//static
//int
//GetMaxPlaneCount( IMovieScenePlayer& iPlayer, const UMovieSceneTrack* iTrack, FMovieSceneSequenceIDRef iSequenceID )
//{
//    int count = 0;
//    for( auto section : iTrack->GetAllSections() )
//    {
//        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
//
//        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, *subsection, iSequenceID );
//        TArray<FGuid> guids;
//        int plane_count = ShotSequenceHelpers::GetAllPlanes( iPlayer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAlwaysAll, nullptr, &guids );
//
//        count = FMath::Max( count, plane_count );
//    }
//
//    return count;
//}

void
SCinematicBoardSectionPlanes::RebuildPlaneList()
{
    if( !mBoardSection.IsValid() )
        return;

    //---

    if( !mNeedRebuildPlaneList )
        return;

    mNeedRebuildPlaneList = false;

    //---

    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    if( !inner_moviescene )
    {
        mPossessables.Empty();

        if( mWidgetPlaneList )
            mWidgetPlaneList->RequestListRefresh();
            //mWidgetPlaneList->RebuildList();

        return;
    }

    //---

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *mSequencer.Pin().Get(), subsection, mSequencer.Pin()->GetFocusedTemplateID() );
    check( inner_sequence == result.mInnerSequence ); // Just to test

    // Get all unordered planes
    TArray<FGuid> unordered_plane_bindings;
    int plane_count = ShotSequenceHelpers::GetAllPlanes( *mSequencer.Pin().Get(), result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAll, nullptr, &unordered_plane_bindings );

    // Find their corresponding scene binding
    TArray<FMovieSceneBinding*> ordered_scene_bindings;
    for( auto unordered_plane_binding : unordered_plane_bindings )
    {
        ordered_scene_bindings.Add( inner_moviescene->FindBinding( unordered_plane_binding ) );
    }

    // Sort scene bindings by their sorting order
    Algo::Sort( ordered_scene_bindings, []( FMovieSceneBinding* iA, FMovieSceneBinding* iB ) { return iA->GetSortingOrder() < iB->GetSortingOrder(); } );

    // Get all planes in the gui order
    TArray<FGuid> ordered_plane_bindings;
    for( auto ordered_scene_binding : ordered_scene_bindings )
    {
        ordered_plane_bindings.Add( ordered_scene_binding->GetObjectGuid() );
    }

    //---

    auto need_rebuild = [this]( const TArray<FGuid>& iBindings )
    {
        if( iBindings.Num() != mPossessables.Num() )
            return true;

        if( !iBindings.Num() ) // Rebuild when no possessables, otherwise list view will be empty and not containing max_planes rows (with invalid guid)
            return true;

        for( int i = 0; i < iBindings.Num(); i++ )
            if( iBindings[i] != mPossessables[i]->GetGuid() )
                return true;

        return false;
    };
    if( !need_rebuild( ordered_plane_bindings ) ) //TOCHECK: check if it's really ok
        return;

    mPossessables.Empty();

    // This doesn't work because this vertical box won't have the same size for all sections
    // and as the height of a track node is getting from the first section in the array (and not necessary the one at the first position in the gui)
    // if the first section has no (or less) planes than others, all planes in the vertical box won't be displayed
    //int max_planes = possessables.Num();

    // Now it seems possible with the new way to compute each section height from the track

    //int max_planes = GetMaxPlaneCount( *sequencer, subsection.GetTypedOuter<UMovieSceneTrack>(), sequencer->GetFocusedTemplateID() );
    //for( int i = 0; i < max_planes; i++ )
    //{
    //    FMovieScenePossessable possessable;
    //    if( bindings.IsValidIndex( i ) )
    //        possessable = *inner_moviescene->FindPossessable( bindings[i] );

    //    mPossessables.Add( MakeShared<FMovieScenePossessable>( possessable ) );
    //}

    for( int i = 0; i < plane_count; i++ )
    {
        FMovieScenePossessable possessable = *inner_moviescene->FindPossessable( ordered_plane_bindings[i] );

        mPossessables.Add( MakeShared<FMovieScenePossessable>( possessable ) );
    }

    if( mWidgetPlaneList )
        mWidgetPlaneList->RequestListRefresh();
        //mWidgetPlaneList->RebuildList();
}

void
SCinematicBoardSectionPlanes::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) //override
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

    RebuildPlaneList();
}

#undef LOCTEXT_NAMESPACE
