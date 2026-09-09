// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "CinematicBoardWidgets/SCinematicBoardSectionAnimations.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "EditorModeManager.h"
#include "Engine/Texture2D.h"
#include "ImageUtils.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieSceneBoolSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "SequencerSettings.h"
#include "Styling/StyleColors.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "TimeToPixel.h"
#include "Tracks/MovieSceneVisibilityTrack.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "EposSequenceHelpers.h"
#include "EposTracksToolbarHelpers.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "CinematicBoardWidgets/SMetaKeysArea.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"
#include "NamingConvention.h"
#include "OdysseyAnimationActor.h"
#include "OdysseyAnimationCell.h"
#include "OdysseyAnimationCut.h"
#include "OdysseyAnimationCutChannel.h"
#include "OdysseyAnimationTimelineSection.h"
#include "OdysseyPainterEditorSettings.h"
#include "OdysseyViewportDrawingEditorEdMode.h"
#include "Tools/ResourceAssetTools.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"
#include "Tools/LighttableTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionAnimations"

//---

class SInlineEditableTextBlockOnDoubleClick4 //TODO: use the same as in other widget
    : public SInlineEditableTextBlock
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
};

FReply
SInlineEditableTextBlockOnDoubleClick4::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    SInlineEditableTextBlock::OnMouseButtonDoubleClick( InMyGeometry, InMouseEvent );

    EnterEditingMode();

    return FReply::Handled();
}

class SKeysOverviewBox2 //TODO: try to use the same as in plane widget ? but there are some references to GetAnimationTimelineMetaChannel()/...
    : public SBox
{
public:
    // no SLATE_ARGS
    // Because in the construct, we don't have to manage the SBox ones (HAlign, Padding, Content, ...)
    // Otherwise we need to have HAlign::Halign( SBox::HAlign ), ...
    // To avoid this for this specific widget, iAnimationBinding is given as a parameter instead of an arg
    // So we can only call SBox::Construct( SBox::FArguments( InArgs ) );

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection, FMovieScenePossessable iAnimationBinding );

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
};

void
SKeysOverviewBox2::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection, FMovieScenePossessable iAnimationBinding )
{
    mBoardSection = iBoardSection;
    mBinding = iAnimationBinding;

    SBox::Construct( SBox::FArguments( InArgs ) );
}

int32
SKeysOverviewBox2::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
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
    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();

    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    TSharedPtr<const FMetaChannel> timeline_meta_channel = board_section->GetAnimationTimelineMetaChannel( mBinding );
    TSharedPtr<const FMetaChannel> transform_meta_channel = board_section->GetAnimationTransformMetaChannel( mBinding );
    TSharedPtr<const FMetaChannel> opacity_meta_channel = board_section->GetAnimationOpacityMetaChannel( mBinding );

    auto FillKeyPositions = [=]( TSharedPtr<const FMetaChannel> iMetaChannel, TArray<float>& iKeyPositions )
    {
        for( const auto& pair : iMetaChannel->GetMetaKeys() )
        {
            FFrameNumber time = pair.Key;
            //FMetaKey meta_key = pair.Value;

            TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( time );
            if( !outer_time )
                continue;

            double outer_second = FQualifiedFrameTime( *outer_time, movie_scene->GetTickResolution() ).AsSeconds();

            iKeyPositions.AddUnique( converter.SecondsToPixel( outer_second ) );
        }
    };

    TArray<float> key_positions_in_pixel;
    FillKeyPositions( timeline_meta_channel, key_positions_in_pixel );
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
ToggleKeysAreaVisibility5( UMovieSceneCinematicBoardSection* iBoardSection, TArray<FGuid> iAnimationBindings, FGuid iAnimationReference )
{
    // This is the current animation which is the reference state
    bool is_reference_visible = iBoardSection->IsAnimationKeysAreaVisible( iAnimationReference );

    for( auto animation_binding : iAnimationBindings )
    {
        if( is_reference_visible )
        {
            if( iBoardSection->IsAnimationKeysAreaVisible( animation_binding ) )
                iBoardSection->ToggleAnimationKeysAreaVisibility( animation_binding ); // If the master is expanded, collapse all expanded animations
        }
        else
        {
            if( !iBoardSection->IsAnimationKeysAreaVisible( animation_binding ) )
                iBoardSection->ToggleAnimationKeysAreaVisibility( animation_binding ); // If the master is collapsed, expand all collpased animations
        }
    }
};

//---
//---
//---

class SCinematicBoardSectionAnimationTitle
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimationTitle )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    ~SCinematicBoardSectionAnimationTitle();

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
    void                MovieSceneDataChanged( EMovieSceneDataChangeType iType );

    FText               GetTooltipText() const;

    void                ToggleAnimationVisibility();
    bool                IsAnimationVisible() const;

    void                ToggleLighttable();
    bool                IsLighttableOn() const;

    FSlateColor         GetBackgroundTint() const;

    FText               HandleTitleText() const;
    void                HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType );

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;
};

//---

SCinematicBoardSectionAnimationTitle::~SCinematicBoardSectionAnimationTitle()
{
}

//---

void
SCinematicBoardSectionAnimationTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;
    check( mBinding.GetGuid().IsValid() );
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    SetToolTipText( MakeAttributeSP( this, &SCinematicBoardSectionAnimationTitle::GetTooltipText ) );

    enum class ePriority: int32
    {
        VeryLow = 0,
        Low = 100,
        Medium = 200,
        High = 300,
        VeryHigh = 400,
    };

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    FMenuEntryResizeParams resize_params;

    //-

    auto ToggleKeysAreaVisibility = [this]()
    {
        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *section_object, sequencer->GetFocusedTemplateID() );

        TSet<FGuid> animation_bindings_selected;
        TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
        for( FGuid animation_binding : animation_bindings )
        {
            TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

            TArray<AOdysseyAnimationActor*> animation_actors_selected;
            ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
            if( animation_actors_selected.Num() )
                animation_bindings_selected.Add( animation_binding );
        }
        animation_bindings = animation_bindings_selected.Array();

        if( animation_bindings.Contains( mBinding.GetGuid() ) )
            ::ToggleKeysAreaVisibility5( section_object, animation_bindings, mBinding.GetGuid() );
        else
            section_object->ToggleAnimationKeysAreaVisibility( mBinding.GetGuid() );
    };

    auto GetKeysAreaTooltip = [this]() -> FText
    {
        FText commun_tooltip = GetTooltipText();
        FText button_tooltip;

        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        if( section_object->IsAnimationKeysAreaVisible( mBinding.GetGuid() ) )
            button_tooltip = LOCTEXT( "hide-animation-keys-area-tooltip", "Hide keys area" );
        else
            button_tooltip = LOCTEXT( "show-animation-keys-area-tooltip", "Show keys area" );

        return FText::Join( FText::FromString( TEXT( "\n\n" ) ), commun_tooltip, button_tooltip );
    };

    auto GetKeysAreaIcon = [this]() -> FSlateIcon
    {
        UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );
        if( section_object->IsAnimationKeysAreaVisible( mBinding.GetGuid() ) )
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "TreeArrow_Expanded" );
        else
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "TreeArrow_Collapsed" );
    };

    resize_params.ClippingPriority = ePriority::Medium;

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( ToggleKeysAreaVisibility )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetKeysAreaTooltip ),
        MakeAttributeLambda( GetKeysAreaIcon ),
        EUserInterfaceActionType::Button,
        NAME_None,
        mOptionalWidgetsVisibility,
        TAttribute<FText>(),
        resize_params
    );

    //-

    auto GetAnimationActorVisibilityTooltip = [this]() -> FText
    {
        FText commun_tooltip = GetTooltipText();
        FText button_tooltip;

        if( IsAnimationVisible() )
            button_tooltip = LOCTEXT( "hide-animation-actor-tooltip", "Hide animation actor" );
        else
            button_tooltip = LOCTEXT( "show-animation-actor-tooltip", "Show animation actor" );

        return FText::Join( FText::FromString( TEXT( "\n\n" ) ), commun_tooltip, button_tooltip );
    };

    auto GetAnimationActorVisibilityIcon = [this]() -> FSlateIcon
    {
        if( IsAnimationVisible() )
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.VisibleIcon16x" );
        else
            return FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.NotVisibleIcon16x" );
    };

    resize_params.ClippingPriority = ePriority::High;

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateRaw( this, &SCinematicBoardSectionAnimationTitle::ToggleAnimationVisibility )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetAnimationActorVisibilityTooltip ),
        MakeAttributeLambda( GetAnimationActorVisibilityIcon ),
        EUserInterfaceActionType::Button,
        NAME_None,
        mOptionalWidgetsVisibility,
        TAttribute<FText>(),
        resize_params
    );

    //-

    auto DetachAnimation = [this]()
    {
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection_object, sequencer->GetFocusedTemplateID() );

        TSet<FGuid> animation_bindings_selected;
        TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
        for( FGuid animation_binding : animation_bindings )
        {
            TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

            TArray<AOdysseyAnimationActor*> animation_actors_selected;
            ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
            if( animation_actors_selected.Num() )
                animation_bindings_selected.Add( animation_binding );
        }
        animation_bindings = animation_bindings_selected.Array();

        if( animation_bindings.Contains( mBinding.GetGuid() ) )
            BoardSequenceTools::DetachAnimation( sequencer, subsection_object, animation_bindings );
        else
            BoardSequenceTools::DetachAnimation( sequencer, subsection_object, mBinding.GetGuid() );
    };

    auto CanDetachAnimation = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();

        if( !IsFocusedSequenceSameAs( sequencer, subsection_object ) )
            return false;

        return BoardSequenceTools::CanDetachAnimation( sequencer, subsection_object, mBinding.GetGuid() );
    };

    auto GetDetachAnimationTooltip = [this]() -> FText
    {
        FText commun_tooltip = GetTooltipText();
        FText button_tooltip;

        button_tooltip = LOCTEXT( "DetachAnimation", "Detach the animation" );

        return FText::Join( FText::FromString( TEXT( "\n\n" ) ), commun_tooltip, button_tooltip );
    };

    resize_params.ClippingPriority = ePriority::Low;

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( DetachAnimation ),
            FCanExecuteAction::CreateLambda( CanDetachAnimation )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetDetachAnimationTooltip ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "DetachAnimation" ),
        EUserInterfaceActionType::Button,
        NAME_None,
        mOptionalWidgetsVisibility,
        TAttribute<FText>(),
        resize_params
    );

    //-

    auto CreateAnimationCut = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection_object, sequencer->GetFocusedTemplateID() );

        TSet<FGuid> animation_bindings_selected;
        TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
        for( FGuid animation_binding : animation_bindings )
        {
            TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

            TArray<AOdysseyAnimationActor*> animation_actors_selected;
            ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
            if( animation_actors_selected.Num() )
                animation_bindings_selected.Add( animation_binding );
        }
        animation_bindings = animation_bindings_selected.Array();

        if( animation_bindings.Contains( mBinding.GetGuid() ) )
            BoardSequenceTools::CreateAnimationCut( sequencer, subsection_object, local_frame, animation_bindings );
        else
            BoardSequenceTools::CreateAnimationCut( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    auto CanCreateAnimationCut = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        if( !IsFocusedSequenceSameAs( sequencer, subsection_object ) )
            return false;

        return BoardSequenceTools::CanCreateAnimationCut( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    auto GetCreateAnimationCutTooltip = [this]() -> FText
        {
            FText commun_tooltip = GetTooltipText();
            FText button_tooltip;

            button_tooltip = LOCTEXT( "create-animationcut", "Create a animation cut (set the current frame where to create the animation cut)" );

            return FText::Join( FText::FromString( TEXT( "\n\n" ) ), commun_tooltip, button_tooltip );
        };

    resize_params.ClippingPriority = ePriority::VeryHigh;

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( CreateAnimationCut ),
            FCanExecuteAction::CreateLambda( CanCreateAnimationCut )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetCreateAnimationCutTooltip ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "CreateAnimationCut" ),
        EUserInterfaceActionType::Button,
        NAME_None,
        mOptionalWidgetsVisibility,
        TAttribute<FText>(),
        resize_params
    );

    //-

    auto GetLighttableTooltip = [this]() -> FText
        {
            FText commun_tooltip = GetTooltipText();
            FText button_tooltip;

            FText warning;
            if( !GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ) )
                warning = LOCTEXT( "warning-no-odyssey-edmode-tooltip", "\n\nWarning: Lighttable is only visible when Odyssey Mode is active" );

            if( IsLighttableOn() )
                button_tooltip = FText::Format( LOCTEXT( "disable-lighttable-tooltip", "Disable the lighttable{0}" ), warning );
            else
                button_tooltip = FText::Format( LOCTEXT( "enable-lighttable-tooltip", "Enable the lighttable{0}" ), warning );

            return FText::Join( FText::FromString( TEXT( "\n\n" ) ), commun_tooltip, button_tooltip );
        };

    auto GetLighttableIcon = [this]() -> FSlateIcon
        {
            if( IsLighttableOn() )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOn" );
            else
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "LighttableOff" );
        };

    resize_params.ClippingPriority = ePriority::Medium;

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateRaw( this, &SCinematicBoardSectionAnimationTitle::ToggleLighttable ),
            FCanExecuteAction::CreateLambda( [this]()
                                             {
                                                 if( !GLevelEditorModeTools().IsModeActive( FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId ) )
                                                     return false;

                                                 return true;
                                             } )
        ),
        NAME_None,
        FText::GetEmpty(),
        MakeAttributeLambda( GetLighttableTooltip ),
        MakeAttributeLambda( GetLighttableIcon ),
        EUserInterfaceActionType::Button,
        NAME_None,
        mOptionalWidgetsVisibility,
        TAttribute<FText>(),
        resize_params
    );

    //---

    FMenuEntryStyleParams style_params;
    style_params.HorizontalAlignment = HAlign_Fill;
    style_params.SizeRule = FSizeParam::ESizeRule::SizeRule_Stretch;
    style_params.FillSize = 1.f;

    resize_params.ClippingPriority = ePriority::VeryLow;

    LeftToolbarBuilder.AddWidget(
        SNew( SHorizontalBox )
        +SHorizontalBox::Slot()
        .FillWidth( 1.f )
        .HAlign( HAlign_Center )
        .Padding( 0, 3.f )
        [
            SNew( SInlineEditableTextBlockOnDoubleClick4 )
            .Text( this, &SCinematicBoardSectionAnimationTitle::HandleTitleText )
            .OnTextCommitted( this, &SCinematicBoardSectionAnimationTitle::HandleTitleTextOnCommited )
        ]
        ,
        style_params,
        NAME_None,
        true,
        FNewMenuDelegate(),
        TAttribute<EVisibility>(),
        //mOptionalWidgetsVisibility,
        resize_params
    );

    //---

    // This code is kept as an example to display a warning button in the animation titlebar
    // - to indicate a mismatch of sections length
    // - ...

    //auto IsWarning = [this]() -> bool
    //{
    //    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    //    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    //    ISequencer* sequencer = board_section->GetSequencer().Get();

    //    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

    //    //-

    //    ShotSequenceHelpers::FFindOrCreateAnimationVisibilityResult animation_visibility_result = ShotSequenceHelpers::FindAnimationVisibilityTrackAndSections( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

    //    if( !animation_visibility_result.mTrack.IsValid() )
    //        return false;

    //    if( animation_visibility_result.mSections.Num() == 0 )
    //        return false;

    //    TRange<FFrameNumber> full_range( TRange<FFrameNumber>::Empty() );
    //    for( auto section : animation_visibility_result.mSections )
    //        full_range = TRange<FFrameNumber>::Hull( full_range, section->GetTrueRange() );

    //    return full_range != result.mInnerMovieScene->GetPlaybackRange();
    //};

    //auto GetWarningTooltip = [this]() -> FText
    //{
    //    return LOCTEXT( "warning-tooltip", "Warning: the animation visibility track doesn't match the shot length, set it maually" );
    //};

    //resize_params.ClippingPriority = int32(ePriority::VeryLow) + 10;

    //LeftToolbarBuilder.AddToolBarButton(
    //    FUIAction(
    //        FExecuteAction(),
    //        FCanExecuteAction(),
    //        FGetActionCheckState(),
    //        FIsActionButtonVisible::CreateLambda( IsWarning )
    //    ),
    //    NAME_None,
    //    FText::GetEmpty(),
    //    MakeAttributeLambda( GetWarningTooltip ),
    //    FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Icons.Warning" ),
    //    EUserInterfaceActionType::Button,
    //    NAME_None,
    //    TAttribute<EVisibility>(),
    //    //mOptionalWidgetsVisibility,
    //    TAttribute<FText>(),
    //    resize_params
    //);

    //---

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();

    //---

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( FAppStyle::Get().GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" ) )
        .BorderBackgroundColor( this, &SCinematicBoardSectionAnimationTitle::GetBackgroundTint )
        [
            SNew( SVerticalBox )

            + SVerticalBox::Slot()
            .AutoHeight()
            [
                    SNew( SHorizontalBox )

                    + SHorizontalBox::Slot()
                    .FillWidth( 1.f )
                    [
                        left_toolbar
                    ]
            ]
        ]
    ];
}

//---

FCursorReply
SCinematicBoardSectionAnimationTitle::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionAnimationTitle::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    // To not trigger the OnMouseButtonDown of SSequencerTrackArea
    // Otherwise OnMouseButtonMove and OnMouseButtonUp will also trigger
    // And OnMouseButtonDown will attempt to start a selection or a drag of the section (normal behavior)
    // But as OnMouseButtonUp is handle here, the one of SSequencerTrackArea won't be handle and the normal section drag won't finish clean
    // (For example, the cursor won't update to crosshair after the up on the empty zone of the SSequencerTrackArea)
    //return FReply::Handled();

    // To be able to move the section through a title animation, otherwise (Handled) it's no more possible
    // Let's see if it's a problem to not handled now (see the comment above)
    // (Or maybe process the Up here ? to avoid this problem ? but in this case, it should also be unhandled to allow SSequencerTrackArea to manage the drag section)
    return FReply::Unhandled();
}

FReply
SCinematicBoardSectionAnimationTitle::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

//---

FText
SCinematicBoardSectionAnimationTitle::GetTooltipText() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText animation_text = FText::Format( LOCTEXT( "tooltip-animation-timeline-area-animation-name", "Animation: {0}" ), animation_track_text );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), animation_text );
}

//---

FText
SCinematicBoardSectionAnimationTitle::HandleTitleText() const
{
    return FText::FromString( mBinding.GetName() );
}

void
SCinematicBoardSectionAnimationTitle::HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType )
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
SCinematicBoardSectionAnimationTitle::GetBackgroundTint() const
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
SCinematicBoardSectionAnimationTitle::ToggleAnimationVisibility()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

    TSet<FGuid> animation_bindings_selected;
    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
    for( FGuid animation_binding : animation_bindings )
    {
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

        TArray<AOdysseyAnimationActor*> animation_actors_selected;
        ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
        if( animation_actors_selected.Num() )
            animation_bindings_selected.Add( animation_binding );
    }
    animation_bindings = animation_bindings_selected.Array();

    if( animation_bindings.Contains( mBinding.GetGuid() ) )
        BoardSequenceTools::ToggleAnimationVisibility( sequencer, *subsection_object, animation_bindings, mBinding.GetGuid() );
    else
        BoardSequenceTools::ToggleAnimationVisibility( sequencer, *subsection_object, mBinding.GetGuid() );
}

bool
SCinematicBoardSectionAnimationTitle::IsAnimationVisible() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    if( !IsFocusedSequenceSameAs( sequencer, *subsection_object ) )
        return false;

    return BoardSequenceTools::IsAnimationVisible( sequencer, *subsection_object, mBinding.GetGuid() );
}

//---

void
SCinematicBoardSectionAnimationTitle::ToggleLighttable()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

    TSet<FGuid> animation_bindings_selected;
    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
    for( FGuid animation_binding : animation_bindings )
    {
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

        TArray<AOdysseyAnimationActor*> animation_actors_selected;
        ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
        if( animation_actors_selected.Num() )
            animation_bindings_selected.Add( animation_binding );
    }
    animation_bindings = animation_bindings_selected.Array();

    // This is the current animation which is the reference state
    bool is_reference_on = LighttableTools::IsOn( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

    if( animation_bindings.Contains( mBinding.GetGuid() ) )
    {
        for( auto animation_binding : animation_bindings )
        {
            if( is_reference_on )
                LighttableTools::Deactivate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );
            else
                LighttableTools::Activate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );
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
SCinematicBoardSectionAnimationTitle::IsLighttableOn() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

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

class SCinematicBoardSectionAnimationTransformKeys
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimationTransformKeys )
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
SCinematicBoardSectionAnimationTransformKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
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
SCinematicBoardSectionAnimationTransformKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetAnimationTransformMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionAnimationTransformKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetAnimationTransformMetaChannel( mBinding );
}

void
SCinematicBoardSectionAnimationTransformKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildAnimationsTransformMetaChannel();
}

//---

FText
SCinematicBoardSectionAnimationTransformKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    return GetAreaTooltipText();
}

FText
SCinematicBoardSectionAnimationTransformKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText animation_text = FText::Format( LOCTEXT( "tooltip-animation-transform-area-animation-name", "Animation: {0}" ), animation_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-animation-transform-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), animation_text, num_keys_text );
}

//---

FCursorReply
SCinematicBoardSectionAnimationTransformKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionAnimationTransformKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationTransformKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationTransformKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionAnimationTransformKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .05f, .13f, .12f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionAnimationTransformKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class SCinematicBoardSectionAnimationTimelineKeys
    : public SMetaKeysArea
    , public FGCObject
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimationTimelineKeys )
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

public:
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

public:
    // SWidget overrides
    virtual FVector2D ComputeDesiredSize( float ) const override;
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

    virtual void Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) override;

protected:
    // SMetaKeysArea overrides
    virtual TSharedPtr<FMetaChannel>        GetMetaChannel() override;
    virtual TSharedPtr<const FMetaChannel>  GetMetaChannel() const override;
    virtual void                            RebuildMetaChannel() override;

    virtual bool BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) override;

    virtual FText GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const override;
    virtual FText GetAreaTooltipText() const override;

    virtual const FSlateBrush* GetBackgroundBrush() const override;

    virtual int32 DrawThumbnails( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const;

    virtual void ComputeClampRangePreMoveDuringDrag( TSharedPtr<FMetaChannel> iKeys, TRange<FFrameNumber>& oClampRangeInSubsequence ) const override;

    virtual void OnStartDragKeys( TSharedPtr<FMetaChannel> iKeys ) override;
    virtual void OnDragKeys( TSharedPtr<FMetaChannel> iKeys ) override;
    virtual void OnStopDragKeys( TSharedPtr<FMetaChannel> iKeys ) override;

    virtual void OnClickKeys( TSharedPtr<FMetaChannel> iKeys ) override;

    virtual EDragMode InitDragMode() const;

    virtual bool ExcludeKey( FFrameNumber iFrameNumber ) const;
    virtual bool ExcludeThumbnail( FFrameTime iTime ) const;

private:
    //TArray<FGuid> BuildThumbnailCache( UOdysseyAnimation* iAnimation, FFrameNumber iFrameTimeline, bool iForce ) const;

    //struct FThumbnailData
    //{
    //    FQualifiedFrameTime QTime;
    //    FIntVector2 Size;
    //    UTexture2D* Texture;
    //    FSlateBrush* Brush;
    //};
    //TArray<FThumbnailData> mThumbnails;
    //bool mNeedRebuildThumbnails = true;
    //void RebuildThumbnails();

    void SetDelegates();
    void OnAnimationCutChannelChanged();
    void OnAnimationChanged( const FOdysseyRenderingChangedEvent& iEvent );

private:
    FMovieScenePossessable              mBinding;

    //mutable TMap<TArray<FGuid>, TSharedPtr<::ULIS::FBlock>> mBlocks;
    //mutable TMap<TArray<FGuid>, UTexture2D*>                mTextures;
    //mutable TMap<TArray<FGuid>, FSlateBrush*>               mBrushes;
    //mutable TMap<TArray<FGuid>, TSet<int32>>                      mImages;

    TObjectPtr<UTexture2D> mCheckboardTexture = nullptr;
    FSlateBrush* mCheckboardBrush = nullptr;
};

void
SCinematicBoardSectionAnimationTimelineKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    mBinding = InArgs._Binding;

    const UOdysseyPainterEditorSettings& settings = *GetDefault<UOdysseyPainterEditorSettings>();
    mCheckboardTexture = FImageUtils::CreateCheckerboardTexture( settings.GetCheckerColorOne(), settings.GetCheckerColorTwo(), settings.GetCheckerSize() );

    mCheckboardBrush = new FSlateBrush();
    mCheckboardBrush->Tiling = ESlateBrushTileType::Both;
    mCheckboardBrush->SetResourceObject( mCheckboardTexture );

    SetDelegates();

    ChildSlot
    [
        SNew( SBox )
    ];
}

FVector2D
SCinematicBoardSectionAnimationTimelineKeys::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight * 3 + 5.f;

    return size;
}

//---

void
SCinematicBoardSectionAnimationTimelineKeys::AddReferencedObjects( FReferenceCollector& Collector ) //override
{
    Collector.AddReferencedObject( mCheckboardTexture );

    //TArray<UTexture2D*> textures;
    //mTextures.GenerateValueArray( textures );
    //for( UTexture2D* texture : textures )
    //    Collector.AddReferencedObject( texture );

    //for( FThumbnailData& thumbnail : mThumbnails )
    //    Collector.AddReferencedObject( thumbnail.Texture );
}

FString
SCinematicBoardSectionAnimationTimelineKeys::GetReferencerName() const //override
{
    return "SCinematicBoardSectionAnimationTimelineKeys";
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionAnimationTimelineKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetAnimationTimelineMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionAnimationTimelineKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetAnimationTimelineMetaChannel( mBinding );
}

void
SCinematicBoardSectionAnimationTimelineKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildAnimationsTimelineMetaChannel();
}

SMetaKeysArea::EDragMode
SCinematicBoardSectionAnimationTimelineKeys::InitDragMode() const
{
    if( FSlateApplication::Get().GetModifierKeys().IsControlDown() )
        return EDragMode::kMoveSingleKey;

    return EDragMode::kShiftFromKey;
}

void
SCinematicBoardSectionAnimationTimelineKeys::SetDelegates()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    BoardSequenceHelpers::FInnerSequenceResult result_inner = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *sequencer, result_inner.mInnerSequence, result_inner.mInnerSequenceId, mBinding.GetGuid() );

    TMultiMap<UOdysseyAnimation*, FFrameNumber> map;
    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> animation_timeline_section : result.mSections )
    {
        animation_timeline_section->OnAnimationCutChannelChanged().AddSP( this, &SCinematicBoardSectionAnimationTimelineKeys::OnAnimationCutChannelChanged );
        if( animation_timeline_section->GetAnimation() )
            animation_timeline_section->GetAnimation()->OnRenderingChangedDelegate().AddSP( this, &SCinematicBoardSectionAnimationTimelineKeys::OnAnimationChanged );
    }

    //TSet<UOdysseyAnimation*> animations;

        //animations.Add( animation_timeline_section->GetAnimation() );

    //for( auto pair : GetMetaChannel()->GetMetaKeys() )
    //{
    //    for( const auto& subkey : pair.Value.mSubKeys )
    //    {
    //        TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
    //        FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();

    //        FOdysseyAnimationCutValue value;
    //        UE::MovieScene::GetKeyValue( object_channel, subkey.mKeyHandle, value );
    //        FAnimationCut animationcut = value.Value;

    //        animations.Add( animationcut.GetAnimation() );
    //    }
    //}

    //for( UOdysseyAnimation* animation : animations )
    //    animation->OnImageRenderingChangedDelegate().AddSP( this, &SCinematicBoardSectionAnimationTimelineKeys::OnSectionChanged );
}

void
SCinematicBoardSectionAnimationTimelineKeys::OnAnimationChanged( const FOdysseyRenderingChangedEvent& iEvent )
{
    if( iEvent.IsInteractive() )
        return;

    if( iEvent.GetType() == FOdysseyRenderingChangedEvent::kValueChange )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        board_section->ReBuildAnimationsTimelineThumbnails( mBinding, iEvent.GetId() );
    }
}

void
SCinematicBoardSectionAnimationTimelineKeys::OnAnimationCutChannelChanged()
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    //ISequencer* sequencer = board_section->GetSequencer().Get();

    //TODO: this should be done somewhere inside FCinematicBoardSection (?) and this delegate should be linked to a FCinematicBoardSection one ?
    board_section->BuildAnimationsTimelineChannelProxy();
    //RebuildMetaChannel();

    //mBrushes.Empty();
    //mTextures.Empty();
    //mBlocks.Empty();
    //mImages.Empty();

    //mNeedRebuildThumbnails = true;

    //UE_LOG( LogTemp, Warning, TEXT( "mNeedRebuildThumbnails: %d" ), mNeedRebuildThumbnails );

    //---

    //BoardSequenceHelpers::FInnerSequenceResult result_inner = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
    //ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *sequencer, result_inner.mInnerSequence, result_inner.mInnerSequenceId, mBinding.GetGuid() );

    //TMultiMap<UOdysseyAnimation*, FFrameNumber> map;
    //for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> animation_timeline_section : result.mSections )
    //{
    //    const FOdysseyAnimationCutChannel& object_channel = animation_timeline_section->GetAnimationCutChannel();

    //    for( FFrameNumber frame_in_sequence : object_channel.GetTimes() )
    //    {
    //        FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );

    //        UOdysseyAnimation* animation = animation_timeline_section->GetAnimation();

    //        UE_LOG( LogTemp, Warning, TEXT( "animation: %p - frame in timeline: %d" ), animation, frame_in_timeline.Value );

    //        map.Add( animation, frame_in_timeline );
    //    }
    //}

    //---

    //TArray<UOdysseyAnimation*> animations;
    //map.GetKeys( animations );
    //for( UOdysseyAnimation* animation : animations )
    //{
    //    UE_LOG( LogTemp, Warning, TEXT( "animation2: %p" ), animation );

    //    TArray<FFrameNumber> frames;
    //    map.MultiFind( animation, frames );
    //    for( FFrameNumber frame : frames )
    //    {
    //        UE_LOG( LogTemp, Warning, TEXT( "animation2: %d" ), frame.Value );

    //        BuildThumbnailCache( animation, frame, true );
    //    }
    //}

    //---

    //typedef TArray<FGuid> FRenderingComposition;

    //if( !GetMetaChannel().IsValid() )
    //    return;

    //TArray<FRenderingComposition> new_rendering_composition;

    //for( const auto& pair : GetMetaChannel()->GetMetaKeys() )
    //{
    //    //FFrameNumber time = pair.Key;
    //    FMetaKey meta_key = pair.Value;
    //    FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

    //    for( const auto& subkey : pair.Value.mSubKeys )
    //    {
    //        UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
    //        if( !animation_timeline_section )
    //            continue;

    //        TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
    //        FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();

    //        FFrameNumber frame_in_sequence;
    //        object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );

    //        //-

    //        FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );

    //        FRenderingComposition rendering_compositions = BuildThumbnailCache( animation_timeline_section->GetAnimation(), frame_in_timeline, true );..................................................
    //        //FRenderingComposition rendering_compositions = BuildThumbnailCache( animation_timeline_section->GetAnimation(), frame_in_timeline, false );
    //        new_rendering_composition.Add( rendering_compositions );
    //    }
    //}

    //TArray<FRenderingComposition> rendering_compositions_to_remove;

    //for( const auto& pair : mBlocks )
    //{
    //    const FRenderingComposition& rendering_composition = pair.Key;
    //    if( !new_rendering_composition.Contains( rendering_composition ) )
    //        rendering_compositions_to_remove.Add( rendering_composition );
    //}

    //for( const FRenderingComposition& rendering_composition_to_remove : rendering_compositions_to_remove )
    //{
    //    mBrushes.Remove( rendering_composition_to_remove );
    //    mTextures.Remove( rendering_composition_to_remove );
    //    mBlocks.Remove( rendering_composition_to_remove );
    //    mImages.Remove( rendering_composition_to_remove );
    //}

    //UE_LOG( LogTemp, Warning, TEXT( "num of brushes: %d" ), mBrushes.Num() );
}

//void
//SCinematicBoardSectionAnimationTimelineKeys::OnAnimationChanged( const FOdysseyImageRenderingChangedEvent& iEvent )
//{
//    if( iEvent.IsInteractive() || mState == EState::kDragging )
//        return;
//
//    UE_LOG( LogTemp, Warning, TEXT( "OnAnimationChanged" ) );
//
//    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
//    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
//    ISequencer* sequencer = board_section->GetSequencer().Get();
//
//    BoardSequenceHelpers::FInnerSequenceResult result_inner = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
//    ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *sequencer, result_inner.mInnerSequence, result_inner.mInnerSequenceId, mBinding.GetGuid() );
//
//    TMultiMap<UOdysseyAnimation*, FFrameNumber> map;
//    for( TWeakObjectPtr<UOdysseyAnimationTimelineSection> animation_timeline_section : result.mSections )
//    {
//        //animation_timeline_section->RebuildAnimationCuts();
//        board_section->BuildAnimationsTimelineChannelProxy();
//        RebuildMetaChannel();
//
//        const FOdysseyAnimationCutChannel& object_channel = animation_timeline_section->GetAnimationCutChannel();
//
//        for( FFrameNumber frame_in_sequence : object_channel.GetTimes() )
//        {
//            FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
//
//            UOdysseyAnimation* animation = animation_timeline_section->GetAnimation();
//
//            UE_LOG( LogTemp, Warning, TEXT( "animation: %p - frame in timeline: %d" ), animation, frame_in_timeline.Value );
//
//            map.Add( animation, frame_in_timeline );
//        }
//    }
//
//    //TMultiMap<UOdysseyAnimation*, FFrameNumber> map;
//    //for( auto pair : GetMetaChannel()->GetMetaKeys() )
//    //{
//    //    for( const auto& subkey : pair.Value.mSubKeys )
//    //    {
//    //        UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
//    //        if( !animation_timeline_section )
//    //            continue;
//
//    //        TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
//    //        FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();
//
//    //        FFrameNumber frame_in_sequence;
//    //        object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );
//
//    //        FOdysseyAnimationCutValue value;
//    //        UE::MovieScene::GetKeyValue( object_channel, subkey.mKeyHandle, value );
//    //        FAnimationCut animationcut = value.Value;
//
//    //        FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
//
//    //        UOdysseyAnimation* animation = animationcut.GetAnimation();
//
//    //        UE_LOG( LogTemp, Warning, TEXT( "animation: %p" ), animation );
//
//    //        map.Add( animation, frame_in_timeline );
//    //    }
//    //}
//
//    //---
//
//    {
//        typedef TArray<FGuid> FRenderingComposition;
//
//        TArray<TPair<FFrameNumber, FRenderingComposition>> new_renderingCompositions;
//        TArray<UOdysseyAnimation*> animations;
//        map.GetKeys( animations );
//        for( UOdysseyAnimation* animation : animations )
//        {
//            TArray<FFrameNumber> frames;
//            map.MultiFind( animation, frames );
//            for( FFrameNumber frame : frames )
//            {
//                TPair<FFrameNumber, FRenderingComposition> pair( frame, animation->GetImageRenderingComposition( IOdysseyImageRenderer::eRenderType::Render, frame.Value ) );
//                new_renderingCompositions.Add( pair );
//            }
//        }
//
//        // Remove all old rendering compositions
//        auto RemoveOldRenderingCompositions = []<typename T>( T& iMap, const TArray<TPair<FFrameNumber, FRenderingComposition>>& iNewRenderingCompositions )
//        {
//            TArray<FRenderingComposition> old_renderingCompositions;
//            iMap.GetKeys( old_renderingCompositions );
//            for( const FRenderingComposition& old_renderingComposition : old_renderingCompositions )
//            {
//                if( !iNewRenderingCompositions.ContainsByPredicate( [&old_renderingComposition]( const TPair<FFrameNumber, FRenderingComposition>& iElement )
//                                                                    {
//                                                                        return iElement.Value == old_renderingComposition;
//                                                                    } ) )
//                    iMap.Remove( old_renderingComposition );
//            }
//        };
//        RemoveOldRenderingCompositions( mBrushes, new_renderingCompositions );
//        RemoveOldRenderingCompositions( mTextures, new_renderingCompositions );
//        RemoveOldRenderingCompositions( mBlocks, new_renderingCompositions );
//        RemoveOldRenderingCompositions( mImages, new_renderingCompositions );
//
//        // Remove all rendering compositions which contains the updated rendering composition
//        auto RemoveContainingRenderingCompositions = []<typename T>( T& iMap, const FGuid& iNewId )
//        {
//            TArray<FRenderingComposition> old_renderingCompositions;
//            iMap.GetKeys( old_renderingCompositions );
//            for( const FRenderingComposition& old_renderingComposition : old_renderingCompositions )
//            {
//                if( old_renderingComposition.Contains( iNewId ) )
//                    iMap.Remove( old_renderingComposition );
//            }
//        };
//        RemoveContainingRenderingCompositions( mBrushes, iEvent.GetId() );
//        RemoveContainingRenderingCompositions( mTextures, iEvent.GetId() );
//        RemoveContainingRenderingCompositions( mBlocks, iEvent.GetId() );
//        RemoveContainingRenderingCompositions( mImages, iEvent.GetId() );
//    }
//
//    mBrushes.Empty();
//    mTextures.Empty();
//    mBlocks.Empty();
//    mImages.Empty();
//
//    //---
//
//    //TArray<UOdysseyAnimation*> animations;
//    //map.GetKeys( animations );
//    //for( UOdysseyAnimation* animation : animations )
//    //{
//    //    UE_LOG( LogTemp, Warning, TEXT( "animation2: %p" ), animation );
//
//    //    TArray<FFrameNumber> frames;
//    //    map.MultiFind( animation, frames );
//    //    for( FFrameNumber frame : frames )
//    //    {
//    //        UE_LOG( LogTemp, Warning, TEXT( "animation2: %d" ), frame.Value );
//
//    //        BuildThumbnailCache( animation, frame, true );
//    //    }
//    //}
//
//    UE_LOG( LogTemp, Warning, TEXT( "%s --- num: %d" ), *iEvent.GetId().ToString(), mBrushes.Num() );
//}

//void
//SCinematicBoardSectionAnimationTimelineKeys::RebuildThumbnails()
//{
//    if( 1 )
//        return;
//
//
//
//
//
//
//    mThumbnails.Empty();
//
//    TSharedPtr<const FMetaChannel> meta_channel = GetMetaChannel();
//    if( !meta_channel.IsValid() )
//        return;
//
//    //---
//
//    if( !mBoardSection.IsValid() )
//        return;
//
//    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
//    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
//
//    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();
//    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
//    check( movie_scene );
//
//    for( const auto& pair : meta_channel->GetMetaKeys() )
//    {
//        FFrameNumber meta_frame = pair.Key;
//        //FMetaKey meta_key = pair.Value;
//        //FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;
//
//        for( const auto& subkey : pair.Value.mSubKeys )
//        {
//            UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
//            if( !animation_timeline_section )
//                continue;
//
//            //TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
//            //FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();
//
//            //FFrameNumber frame_in_sequence;
//            //object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );
//
//            //-
//
//            FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( meta_frame );
//            //FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
//            if( frame_in_timeline < 0 )
//            {
//                FFrameNumber debug = 2; debug = frame_in_timeline;
//            }
//
//            TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( meta_frame );
//            //TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( frame_in_sequence );
//            if( !outer_time )
//                continue;
//
//            UE_LOG( LogTemp, Warning, TEXT( "frame_in_timeline.Value: %d" ), frame_in_timeline.Value );
//
//            //---
//
//            UOdysseyAnimation* animation = animation_timeline_section->GetAnimation();
//
//            TSharedPtr<::ULIS::FBlock> block_full = MakeShared<::ULIS::FBlock>( animation->GetWidth(), animation->GetHeight(), animation->GetFormat() );
//
//            ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( animation->GetFormat() );
//
//            TSharedPtr<IOdysseyImageRenderer> renderer = animation->BuildImageRenderer( IOdysseyImageRenderer::eRenderType::Render, frame_in_timeline.Value );
//            renderer->Init();
//            FOdysseyImageRendererCopyParams params( block_full, { block_full->Rect() } );
//            renderer->Copy( params, {} );
//
//            float ratio = animation->GetWidth() / float( animation->GetHeight() );
//            const FIntVector2 thumbnail_size( 200 * ratio, 200 );
//            TSharedPtr<::ULIS::FBlock> block = MakeShared<::ULIS::FBlock>( thumbnail_size.X, thumbnail_size.Y, animation->GetFormat() );
//            ctx.Resize( *block_full, *block );
//
//            ctx.Finish();
//
//            //-
//
//            TConstArrayView64<uint8> data( block->Bits(), block->BytesTotal() );
//            UTexture2D* texture = UTexture2D::CreateTransient(
//                block->Width(),
//                block->Height(),
//                EPixelFormat::PF_B8G8R8A8,
//                NAME_None,
//                data
//            );
//
//            FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( 0, 0, 0, 0, block->Width(), block->Height() );
//            int blockBytes = GPixelFormats[EPixelFormat::PF_B8G8R8A8].BlockBytes;
//            texture->UpdateTextureRegions(
//                0
//                , 1
//                , region
//                , block->Width() * blockBytes
//                , blockBytes
//                , block->Bits()
//                , []( uint8*, const FUpdateTextureRegion2D* iRegions )
//                {
//                    delete iRegions;
//                }
//            );
//
//            ////////////////////////////
//            // Fence now to ensure the update is processed on the GPU by the end of this function
//            FRenderCommandFence fence;
//            fence.BeginFence();
//            fence.Wait();
//
//            //-
//
//            FSlateBrush* brush = new FSlateBrush();
//
//            brush->SetResourceObject( texture );
//
//            //---
//
//            FThumbnailData thumbnail;
//            thumbnail.QTime = FQualifiedFrameTime( *outer_time, movie_scene->GetTickResolution() );
//            thumbnail.Texture = texture;
//            thumbnail.Brush = brush;
//            thumbnail.Size = thumbnail_size;
//
//            mThumbnails.Add( thumbnail );
//
//            break; // only the first subkey is used
//        }
//    }
//}

//TArray<FGuid>
//SCinematicBoardSectionAnimationTimelineKeys::BuildThumbnailCache( UOdysseyAnimation* iAnimation, FFrameNumber iFrameTimeline, bool iForce ) const
//{
//    TArray<FGuid> renderingComposition = iAnimation->GetImageRenderingComposition( IOdysseyImageRenderer::eRenderType::Render, iFrameTimeline.Value );
//
//    if( !mImages.Contains( renderingComposition ) || iForce )
//    {
//        TSet<int32> image;
//        image.Add( iFrameTimeline.Value );
//        mImages.Add( renderingComposition, image );
//    }
//    else
//    {
//        TSet<int32>& image = mImages.FindChecked( renderingComposition );
//        image.Add( iFrameTimeline.Value );
//    }
//
//    //---
//
//    TSharedPtr<::ULIS::FBlock> block;
//    if( !mBlocks.Contains( renderingComposition ) || iForce )
//    {
//        TSharedPtr<::ULIS::FBlock> block_full = MakeShared<::ULIS::FBlock>( iAnimation->GetWidth(), iAnimation->GetHeight(), iAnimation->GetFormat() );
//
//        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext( iAnimation->GetFormat() );
//
//        TSharedPtr<IOdysseyImageRenderer> renderer = iAnimation->BuildImageRenderer( IOdysseyImageRenderer::eRenderType::Render, iFrameTimeline.Value );
//        renderer->Init();
//        FOdysseyImageRendererCopyParams params( block_full, { block_full->Rect() } );
//        renderer->Copy( params, {} );
//
//        float ratio = iAnimation->GetWidth() / float( iAnimation->GetHeight() );
//        const FIntVector2 thumbnail_size( 200 * ratio, 200 );
//        block = MakeShared<::ULIS::FBlock>( thumbnail_size.X, thumbnail_size.Y, iAnimation->GetFormat() );
//        ctx.Resize( *block_full, *block );
//
//        ctx.Finish();
//
//        mBlocks.Add( renderingComposition, block );
//    }
//    else
//    {
//        block = mBlocks.FindChecked( renderingComposition );
//    }
//
//    //---
//
//    UTexture2D* texture = nullptr;
//    if( !mTextures.Contains( renderingComposition ) || iForce )
//    {
//        TConstArrayView64<uint8> data( block->Bits(), block->BytesTotal() );
//        texture = UTexture2D::CreateTransient(
//            block->Width(),
//            block->Height(),
//            EPixelFormat::PF_B8G8R8A8,
//            NAME_None,
//            data
//        );
//
//        mTextures.Add( renderingComposition, texture );
//
//        FUpdateTextureRegion2D* region = new FUpdateTextureRegion2D( 0, 0, 0, 0, block->Width(), block->Height() );
//        int blockBytes = GPixelFormats[EPixelFormat::PF_B8G8R8A8].BlockBytes;
//        texture->UpdateTextureRegions(
//            0
//            , 1
//            , region
//            , block->Width() * blockBytes
//            , blockBytes
//            , block->Bits()
//            , []( uint8*, const FUpdateTextureRegion2D* iRegions )
//            {
//                delete iRegions;
//            }
//        );
//
//        ////////////////////////////
//        // Fence now to ensure the update is processed on the GPU by the end of this function
//        FRenderCommandFence fence;
//        fence.BeginFence();
//        fence.Wait();
//    }
//    else
//    {
//        texture = mTextures.FindChecked( renderingComposition );
//    }
//
//    //---
//
//    FSlateBrush* brush = nullptr;
//    if( !mBrushes.Contains( renderingComposition ) || iForce )
//    {
//        brush = new FSlateBrush();
//
//        mBrushes.Add( renderingComposition, brush );
//    }
//    else
//    {
//        brush = mBrushes.FindChecked( renderingComposition );
//    }
//
//    if( brush->GetResourceObject() != texture )
//        brush->SetResourceObject( texture );
//
//    //---
//
//    return renderingComposition;
//}

//---

bool
SCinematicBoardSectionAnimationTimelineKeys::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
{
    auto EditAnimation = [=]( UOdysseyAnimation* iAnimation )
    {
        if( !iAnimation )
            return;

        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();
        check( !AssetEditorSubsystem->FindEditorsForAsset( iAnimation ).Num() );

        AssetEditorSubsystem->OpenEditorForAsset( iAnimation );
    };

    auto CanEditAnimation = [=]( UOdysseyAnimation* iAnimation ) -> bool
    {
        UAssetEditorSubsystem* AssetEditorSubsystem = GEditor->GetEditorSubsystem<UAssetEditorSubsystem>();

        TArray<IAssetEditorInstance*> opened_editors = AssetEditorSubsystem->FindEditorsForAsset( iAnimation );
        //FName name = opened_editors.Num() ? opened_editors[0]->GetEditorName() : NAME_None;

        return !opened_editors.Num();
    };

    //-

    UOdysseyAnimation* animation = nullptr;
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
            FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();

            FFrameNumber frame_in_sequence;
            object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );

            FOdysseyAnimationCutValue value;
            UE::MovieScene::GetKeyValue( object_channel, subkey.mKeyHandle, value );
            FAnimationCut animationcut = value.Value;

            animation = animationcut.GetAnimation();
        }
    }

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "animationcut-key-section-label", "Animation" ) );

    if( animation )
    {
        ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-animationcut-key-label", "Edit..." ),
                                    LOCTEXT( "edit-animationcut-key-tooltip", "Edit the animation of the current key with its default editor\n(If it's not possible, the animation is already opened)" ),
                                    FSlateIcon(),
                                    FUIAction( FExecuteAction::CreateLambda( EditAnimation, animation ),
                                               FCanExecuteAction::CreateLambda( CanEditAnimation, animation ) ) );
    }

    ioMenuBuilder.EndSection();

    return true;
}

FText
SCinematicBoardSectionAnimationTimelineKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    UOdysseyAnimation* animation = nullptr;
    TMultiMap<FFrameNumber, FText> map;
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
            FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();

            FFrameNumber frame_in_sequence;
            object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );

            FOdysseyAnimationCutValue value;
            UE::MovieScene::GetKeyValue( object_channel, subkey.mKeyHandle, value );
            FAnimationCut animationcut = value.Value;
            animation = animationcut.GetAnimation();

            TArray<UOdysseyLayerCell*> cells = animationcut.GetCellsReference();
            for( UOdysseyLayerCell* cell : cells )
            {
                map.Add( frame_in_sequence, cell->GetLayer()->GetLayerName() );
            }
        }
    }

    TArray<FText> lines;
    lines.Add( FText::Format( LOCTEXT( "tooltip-animation-timeline-key-animation-name", "Animation: {0}" ), animation_track_text ) );

    TArray<FFrameNumber> keys;
    map.GetKeys( keys );
    for( FFrameNumber key : keys )
    {
        FFrameTime frametime = FFrameRate::TransformTime( key, inner_moviescene->GetTickResolution(), inner_moviescene->GetDisplayRate() );
        lines.Add( FText::Format( LOCTEXT( "tooltip-animation-timeline-key-value-frame", "Frame: {0}" ), FText::AsNumber( frametime.GetFrame().Value ) ) );

        TArray<FText> values;
        map.MultiFind( key, values );
        for( FText value : values )
            lines.Add( FText::Format( LOCTEXT( "tooltip-animation-timeline-key-value-layer-name", "Layer: {0}" ), value ) );
    }

    return FText::Join( FText::FromString( TEXT( "\n" ) ), lines );
}

FText
SCinematicBoardSectionAnimationTimelineKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText animation_text = FText::Format( LOCTEXT( "tooltip-animation-timeline-area-animation-name", "Animation: {0}" ), animation_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-animation-timeline-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), animation_text, num_keys_text );
}

//---

void
SCinematicBoardSectionAnimationTimelineKeys::OnStartDragKeys( TSharedPtr<FMetaChannel> iKeys ) //override
{
}

static
void
GetKeyHandlesAndNewFrames( TSharedPtr<FMetaChannel> iKeys, TMultiMap<UOdysseyAnimationTimelineSection*, FKeyHandle>& oKeyHandlesBySectionMap, TMultiMap<UOdysseyAnimationTimelineSection*, FFrameNumber>& oNewFramesBySectionMap )
{
    oKeyHandlesBySectionMap.Empty();
    oNewFramesBySectionMap.Empty();

    struct FKeyHandleAndFrames
    {
        FKeyHandle KeyHandle;
        FFrameNumber NewFrameInTimeline;
        FAnimationCut AnimationCut;
    };

    TMap<UOdysseyAnimationTimelineSection*, TArray<FKeyHandleAndFrames>> dragged_animationcuts_map;
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
            if( !animation_timeline_section )
                continue;

            TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
            FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();

            FFrameNumber frame_in_sequence;
            object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );

            FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );

            FOdysseyAnimationCutValue value;
            UE::MovieScene::GetKeyValue( object_channel, subkey.mKeyHandle, value );
            FAnimationCut animationcut = value.Value;

            FKeyHandleAndFrames new_key_handle = { subkey.mKeyHandle, frame_in_timeline, animationcut };
            TArray<FKeyHandleAndFrames>* key_handles = dragged_animationcuts_map.Find( animation_timeline_section );
            if( !key_handles )
            {
                TArray<FKeyHandleAndFrames> new_key_handles;
                new_key_handles.Add( new_key_handle );

                dragged_animationcuts_map.Add( animation_timeline_section, new_key_handles );
            }
            else
            {
                key_handles->Add( new_key_handle );
            }
        }
    }

    if( dragged_animationcuts_map.IsEmpty() )
        return;

    for( auto pair : dragged_animationcuts_map )
    {
        UOdysseyAnimationTimelineSection* animation_timeline_section = pair.Key;
        TArray<FKeyHandleAndFrames> dragged_key_handles = pair.Value;

        for( const FKeyHandleAndFrames& handles_and_frames : dragged_key_handles )
        {
            oKeyHandlesBySectionMap.Add( animation_timeline_section, handles_and_frames.KeyHandle );
            oNewFramesBySectionMap.Add( animation_timeline_section, handles_and_frames.NewFrameInTimeline );
        }
    }
}

void
SCinematicBoardSectionAnimationTimelineKeys::OnDragKeys( TSharedPtr<FMetaChannel> iKeys ) //override
{
    TMultiMap<UOdysseyAnimationTimelineSection*, FKeyHandle> key_handles_map;
    TMultiMap<UOdysseyAnimationTimelineSection*, FFrameNumber> new_frames_in_timeline_map;
    GetKeyHandlesAndNewFrames( iKeys, key_handles_map, new_frames_in_timeline_map );

    TArray<UOdysseyAnimationTimelineSection*> sections;
    key_handles_map.GetKeys( sections );
    for( int i = 0; i < sections.Num(); i++ )
    {
        UOdysseyAnimationTimelineSection* section = sections[i];

        TArray<FKeyHandle> key_handles;
        key_handles_map.MultiFind( section, key_handles );
        //TArray<FFrameNumber> new_frames_in_timeline;
        //new_frames_in_timeline_map.MultiFind( section, new_frames_in_timeline );

        section->UpdateAnimationCutChannel( key_handles, EPropertyChangeType::Interactive );
    }

    //mNeedRebuildThumbnails = true;
    //RebuildMetaChannel();
}

void
SCinematicBoardSectionAnimationTimelineKeys::OnStopDragKeys( TSharedPtr<FMetaChannel> iKeys ) //override
{
    TMultiMap<UOdysseyAnimationTimelineSection*, FKeyHandle> key_handles_map;
    TMultiMap<UOdysseyAnimationTimelineSection*, FFrameNumber> new_frames_in_timeline_map;
    GetKeyHandlesAndNewFrames( iKeys, key_handles_map, new_frames_in_timeline_map );

    TArray<UOdysseyAnimationTimelineSection*> sections;
    key_handles_map.GetKeys( sections );
    for( int i = 0; i < sections.Num(); i++ )
    {
        UOdysseyAnimationTimelineSection* section = sections[i];

        TArray<FKeyHandle> key_handles;
        key_handles_map.MultiFind( section, key_handles );
        //TArray<FFrameNumber> new_frames_in_timeline;
        //new_frames_in_timeline_map.MultiFind( section, new_frames_in_timeline );

        section->UpdateAnimationCutChannel( key_handles, EPropertyChangeType::ValueSet );
    }

    //mNeedRebuildThumbnails = true;
    //RebuildMetaChannel();
}

void
SCinematicBoardSectionAnimationTimelineKeys::OnClickKeys( TSharedPtr<FMetaChannel> iKeys ) //override
{
}

void
SCinematicBoardSectionAnimationTimelineKeys::ComputeClampRangePreMoveDuringDrag( TSharedPtr<FMetaChannel> iKeys, TRange<FFrameNumber>& oClampRangeInSubsequence ) const //override
{
    TRange<FFrameNumber> board_section_clamp_range_in_subsequence;
    SMetaKeysArea::ComputeClampRangePreMoveDuringDrag( iKeys, board_section_clamp_range_in_subsequence ); // Always call the default clamp range to get min/max boundary of the whole board section in the board track

    //-

    if( mDragMode == EDragMode::kMoveSingleKey )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        FFrameNumber clamp_max2 = UE::MovieScene::DiscreteExclusiveUpper( board_section_clamp_range_in_subsequence.GetUpperBound() ) - 1;

        BoardSequenceHelpers::FInnerSequenceResult result_inner = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
        ShotSequenceHelpers::FFindOrCreateTimelineResult result = ShotSequenceHelpers::FindTimelineTrackAndSections( *sequencer, result_inner.mInnerSequence, result_inner.mInnerSequenceId, mBinding.GetGuid(), clamp_max2 );
        if( result.mSections.Num() )
        {
            //TODO: certainly make a loop over all sections (?)
            FFrameNumber frame_in_timeline = result.mSections[0]->ConvertFrameFromSequenceToTimeline( clamp_max2 );
            FFrameNumber frame_in_sequence = result.mSections[0]->ConvertFrameFromTimelineToSequence( frame_in_timeline );

            clamp_max2 = frame_in_sequence;
        }

        board_section_clamp_range_in_subsequence.SetUpperBound( TRangeBound<FFrameNumber>::Inclusive( clamp_max2 ) );
    }

    //---

    // Get all key handles for each section
    TMultiMap<UOdysseyAnimationTimelineSection*, FKeyHandle> dragged_keys_map;
    for( auto pair : iKeys->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
            if( !animation_timeline_section )
                continue;

            TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
            FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();
            check( object_channel == &animation_timeline_section->GetAnimationCutChannel() );

            dragged_keys_map.Add( animation_timeline_section, subkey.mKeyHandle );
        }
    }

    // Get the key handle limits combining all sections
    TPair<UOdysseyAnimationTimelineSection*, FKeyHandle> first_limit_handle;
    TPair<UOdysseyAnimationTimelineSection*, FKeyHandle> last_limit_handle;

    TSet<UOdysseyAnimationTimelineSection*> sections;
    dragged_keys_map.GetKeys( sections );
    for( UOdysseyAnimationTimelineSection* section : sections )
    {
        TArray<FKeyHandle> handles;
        dragged_keys_map.MultiFind( section, handles );

        FKeyHandle previous_handle_in_section = section->GetAnimationCutChannel().FindPreviousKey( handles );
        FKeyHandle next_handle_in_section = section->GetAnimationCutChannel().FindNextKey( handles );

        if( previous_handle_in_section.IsValid() )
        {
            FFrameNumber previous_frame_in_section;
            section->GetAnimationCutChannel().GetKeyTime( previous_handle_in_section, previous_frame_in_section );

            if( !first_limit_handle.Key )
            {
                first_limit_handle.Key = section;
                first_limit_handle.Value = previous_handle_in_section;
            }
            else
            {
                FFrameNumber previous_frame;
                first_limit_handle.Key->GetAnimationCutChannel().GetKeyTime( first_limit_handle.Value, previous_frame );

                if( previous_frame_in_section < previous_frame )
                {
                    first_limit_handle.Key = section;
                    first_limit_handle.Value = previous_handle_in_section;
                }
            }
        }
        if( mDragMode == EDragMode::kMoveSingleKey )
        {
            if( next_handle_in_section.IsValid() )
            {
                FFrameNumber next_frame_in_section;
                section->GetAnimationCutChannel().GetKeyTime( next_handle_in_section, next_frame_in_section );

                if( !last_limit_handle.Key )
                {
                    last_limit_handle.Key = section;
                    last_limit_handle.Value = next_handle_in_section;
                }
                else
                {
                    FFrameNumber next_frame;
                    last_limit_handle.Key->GetAnimationCutChannel().GetKeyTime( last_limit_handle.Value, next_frame );

                    if( next_frame_in_section > next_frame )
                    {
                        last_limit_handle.Key = section;
                        last_limit_handle.Value = next_handle_in_section;
                    }
                }
            }
        }
    }

    // Compute the limit range of both key handle limits
    TRange<FFrameNumber> prev_last_cell_clamp_range_in_subsequence = TRange<FFrameNumber>::All();
    if( first_limit_handle.Key )
    {
        FFrameNumber frame_in_sequence;
        first_limit_handle.Key->GetAnimationCutChannel().GetKeyTime( first_limit_handle.Value, frame_in_sequence );
        FFrameNumber frame_in_timeline = first_limit_handle.Key->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
        frame_in_sequence = first_limit_handle.Key->ConvertFrameFromTimelineToSequence( frame_in_timeline + 1 );

        prev_last_cell_clamp_range_in_subsequence.SetLowerBound( TRangeBound<FFrameNumber>::Inclusive( frame_in_sequence ) );
    }
    else
    {
        FFrameNumber frame_in_sequence_min_0 = TNumericLimits<FFrameNumber>::Max();
        for( UOdysseyAnimationTimelineSection* section : sections )
        {
            FFrameNumber frame_in_sequence_0 = section->ConvertFrameFromTimelineToSequence( 0 ); // If no previous animationcut, the timeline can't never be before frame 0
            if( frame_in_sequence_min_0 > frame_in_sequence_0 )
                frame_in_sequence_min_0 = frame_in_sequence_0;
        }

        if( sections.Num() )
            prev_last_cell_clamp_range_in_subsequence.SetLowerBound( TRangeBound<FFrameNumber>::Inclusive( frame_in_sequence_min_0 ) );
    }
    if( mDragMode == EDragMode::kMoveSingleKey )
    {
        if( last_limit_handle.Key )
        {
            FFrameNumber frame_in_sequence;
            last_limit_handle.Key->GetAnimationCutChannel().GetKeyTime( last_limit_handle.Value, frame_in_sequence );
            FFrameNumber frame_in_timeline = last_limit_handle.Key->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
            frame_in_sequence = last_limit_handle.Key->ConvertFrameFromTimelineToSequence( frame_in_timeline - 1 );

            prev_last_cell_clamp_range_in_subsequence.SetUpperBound( TRangeBound<FFrameNumber>::Inclusive( frame_in_sequence ) );
        }
    }

    //---

    oClampRangeInSubsequence = TRange<FFrameNumber>::Intersection( board_section_clamp_range_in_subsequence, prev_last_cell_clamp_range_in_subsequence );
}

//---

FCursorReply
SCinematicBoardSectionAnimationTimelineKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionAnimationTimelineKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationTimelineKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationTimelineKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

void
SCinematicBoardSectionAnimationTimelineKeys::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) //override
{
    //if( mNeedRebuildThumbnails )
    //{
    //    mNeedRebuildThumbnails = false;
    //    RebuildThumbnails();
    //}

    return SMetaKeysArea::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );
}

const FSlateBrush*
SCinematicBoardSectionAnimationTimelineKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    return &background_brush;
}

bool
SCinematicBoardSectionAnimationTimelineKeys::ExcludeThumbnail( FFrameTime iTime ) const
{
    if( !mBoardSection.IsValid() )
        return false;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();

    TArray<const FCinematicBoardSection::FThumbnailData*> thumbnail_data;
    for( const FCinematicBoardSection::FThumbnailData& thumbnail : board_section->GetAnimationTimelineThumbnails( mBinding ) )
        thumbnail_data.Add( &thumbnail );
    thumbnail_data.Sort( []( const FCinematicBoardSection::FThumbnailData& iData1, const FCinematicBoardSection::FThumbnailData& iData2 )
                         {
                             return iData1.QTime.Time < iData2.QTime.Time;
                         } );

    bool is_last_key = ( thumbnail_data.Last()->QTime.Time == iTime );
    if( !is_last_key )
        return false;

    bool is_multi = thumbnail_data.Last()->MultipleSubkey;
    if( is_multi )
        return false;

    return true;
}

int32
SCinematicBoardSectionAnimationTimelineKeys::DrawThumbnails( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
{
    if( !mBoardSection.IsValid() )
        return LayerId;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();

    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );

    //UE_LOG( LogTemp, Warning, TEXT( "mThumbnails count: %d" ), mThumbnails.Num() );

    for( const FCinematicBoardSection::FThumbnailData& thumbnail : board_section->GetAnimationTimelineThumbnails( mBinding ) )
    //for( const FThumbnailData& thumbnail : mThumbnails )
    {

        FIntVector2 thumbnail_size;
        thumbnail_size.Y = AllottedGeometry.GetLocalSize().Y * .9f;
        thumbnail_size.X = thumbnail_size.Y * ( thumbnail.Size.X / float( thumbnail.Size.Y ) );

        const float KeyPositionPx = converter.SecondsToPixel( thumbnail.QTime.AsSeconds() );
        const FVector2D KeyTranslation( KeyPositionPx, ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( thumbnail_size.Y / 2.0f ) ) );

        //-

        static const FSlateBrush* SepBrush = new FSlateColorBrush( FColor::Black );
        const FVector2D SepSize( 3, AllottedGeometry.GetLocalSize().Y - 2 );
        const FVector2D SepTranslation( KeyPositionPx - FMath::CeilToFloat( SepSize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( SepSize.Y / 2.0f ) ) );

        //-

        FLinearColor borderTint = FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f );
        FLinearColor endTint = FLinearColor( 0.75f, 0.25f, 0.25f, 1.0f );

        //-

        if( !ExcludeThumbnail( thumbnail.QTime.Time ) )
        {
            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D( thumbnail_size.X, thumbnail_size.Y ), FSlateLayoutTransform( KeyTranslation ) ),
                mCheckboardBrush,
                ESlateDrawEffect::None,
                borderTint
            );

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( FVector2D( thumbnail_size.X, thumbnail_size.Y ), FSlateLayoutTransform( KeyTranslation ) ),
                thumbnail.Brush,
                ESlateDrawEffect::None,
                borderTint
            );

            FSlateDrawElement::MakeBox(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( SepSize, FSlateLayoutTransform( SepTranslation ) ),
                SepBrush,
                ESlateDrawEffect::None,
                borderTint
            );
        }
        else
        {
            TArray<FVector2f> points;
            points.Add( FVector2f( SepSize.X / 2, 0.f ) );
            points.Add( FVector2f( SepSize.X / 2, SepSize.Y ) );

            FSlateDrawElement::MakeDashedLines(
                OutDrawElements,
                LayerId,
                AllottedGeometry.ToPaintGeometry( SepSize, FSlateLayoutTransform( SepTranslation ) ),
                MoveTemp( points ),
                ESlateDrawEffect::None,
                endTint,
                2,      // Thickness
                4       // DashLengthPix
                //0     // DashScreenOffset
            );
        }

        LayerId++;
    }

    return LayerId;
}

//int32
//SCinematicBoardSectionAnimationTimelineKeys::DrawThumbnails( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const
//{
//    TSharedPtr<const FMetaChannel> meta_channel = GetMetaChannel();
//
//    if( !meta_channel.IsValid() )
//        return LayerId;
//
//    //---
//
//    if( !mBoardSection.IsValid() )
//        return LayerId;
//
//    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
//    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
//
//    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
//    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
//    FMovieSceneInverseSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().Inverse();
//    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
//    check( movie_scene );
//
//    for( const auto& pair : meta_channel->GetMetaKeys() )
//    {
//        //FFrameNumber time = pair.Key;
//        FMetaKey meta_key = pair.Value;
//        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;
//
//        for( const auto& subkey : pair.Value.mSubKeys )
//        {
//            UOdysseyAnimationTimelineSection* animation_timeline_section = Cast<UOdysseyAnimationTimelineSection>( subkey.mSection );
//            if( !animation_timeline_section )
//                continue;
//
//            TMovieSceneChannelHandle<FOdysseyAnimationCutChannel> channel_handle = subkey.mChannelHandle.Cast<FOdysseyAnimationCutChannel>();
//            FOdysseyAnimationCutChannel* object_channel = channel_handle.Get();
//
//            FFrameNumber frame_in_sequence;
//            object_channel->GetKeyTime( subkey.mKeyHandle, frame_in_sequence );
//
//            //-
//
//            FFrameNumber frame_in_timeline = animation_timeline_section->ConvertFrameFromSequenceToTimeline( frame_in_sequence );
//            if( frame_in_timeline < 0 )
//            {
//                FFrameNumber debug = 2; debug = frame_in_timeline;
//            }
//
//            TOptional<FFrameTime> outer_time = inner_to_outer_transform.TryTransformTime( frame_in_sequence );
//            if( !outer_time )
//                continue;
//
//            double outer_second = FQualifiedFrameTime( *outer_time, movie_scene->GetTickResolution() ).AsSeconds();
//
//            //-
//
//            TArray<FGuid> rendering_compositions = BuildThumbnailCache( animation_timeline_section->GetAnimation(), frame_in_timeline, false );
//
//            TSharedPtr<::ULIS::FBlock> block = mBlocks.FindChecked( rendering_compositions );
//            FSlateBrush* thumbnail_brush = mBrushes.FindChecked( rendering_compositions );
//
//            //-
//
//            FIntVector2 thumbnail_size;
//            thumbnail_size.Y = AllottedGeometry.GetLocalSize().Y * .9f;
//            thumbnail_size.X = thumbnail_size.Y * ( block->Width() / float( block->Height() ) );
//
//            const float KeyPositionPx = converter.SecondsToPixel( outer_second );
//            const FVector2D KeyTranslation( KeyPositionPx, ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( thumbnail_size.Y / 2.0f ) ) );
//
//            key_draw_param.BorderTint = FLinearColor( 0.5f, 0.5f, 0.5f, 1.0f );
//
//            FSlateDrawElement::MakeBox(
//                OutDrawElements,
//                LayerId,
//                AllottedGeometry.ToPaintGeometry( FVector2D( thumbnail_size.X, thumbnail_size.Y ), FSlateLayoutTransform( KeyTranslation ) ),
//                mCheckboardBrush,
//                ESlateDrawEffect::None,
//                key_draw_param.BorderTint
//            );
//
//            FSlateDrawElement::MakeBox(
//                OutDrawElements,
//                LayerId,
//                AllottedGeometry.ToPaintGeometry( FVector2D( thumbnail_size.X, thumbnail_size.Y ), FSlateLayoutTransform( KeyTranslation ) ),
//                thumbnail_brush,
//                ESlateDrawEffect::None,
//                key_draw_param.BorderTint
//            );
//
//            static const FSlateBrush* SepBrush = new FSlateColorBrush( FColor::Black );
//            const FVector2D SepSize( 3, AllottedGeometry.GetLocalSize().Y - 2 );
//            const FVector2D SepTranslation( KeyPositionPx - FMath::CeilToFloat( SepSize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( SepSize.Y / 2.0f ) ) );
//
//            FSlateDrawElement::MakeBox(
//                OutDrawElements,
//                LayerId,
//                AllottedGeometry.ToPaintGeometry( SepSize, FSlateLayoutTransform( SepTranslation ) ),
//                SepBrush,
//                ESlateDrawEffect::None,
//                key_draw_param.BorderTint
//            );
//
//            break; // Draw only the first subkey
//        }
//
//        LayerId++;
//    }
//
//    return LayerId;
//}

bool
SCinematicBoardSectionAnimationTimelineKeys::ExcludeKey( FFrameNumber iFrameNumber ) const
{
    TSharedPtr<const FMetaChannel> meta_channel = GetMetaChannel();
    if( !meta_channel.IsValid() )
        return false;

    TArray<FFrameNumber> keys;
    meta_channel->GetMetaKeys().GetKeys( keys );
    keys.Sort();

    bool is_last_key = ( keys.Last() == iFrameNumber );
    if( !is_last_key )
        return false;

    bool is_multi = meta_channel->GetMetaKeys().FindChecked( keys.Last() ).mSubKeys.Num() > 1;
    if( is_multi )
        return false;

    return true;
}

int32
SCinematicBoardSectionAnimationTimelineKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    LayerId = DrawBackground( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    LayerId = DrawThumbnails( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    LayerId = DrawKeys( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class SCinematicBoardSectionAnimationOpacityKeys
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimationOpacityKeys )
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
SCinematicBoardSectionAnimationOpacityKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
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
SCinematicBoardSectionAnimationOpacityKeys::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetAnimationOpacityMetaChannel( mBinding );
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionAnimationOpacityKeys::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetAnimationOpacityMetaChannel( mBinding );
}

void
SCinematicBoardSectionAnimationOpacityKeys::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildAnimationsOpacityMetaChannel();
}

//---

bool
SCinematicBoardSectionAnimationOpacityKeys::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
{
    auto DeleteKey = [this]( TSharedPtr<FMetaChannel> iKeys )
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

    auto SetKey = [this](TSharedPtr<FMetaChannel> iKeys, float iOpacity)
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

    auto SetOpacitySubMenu = [this, SetKey, CanSetKey]( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys )
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

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "animation-section-label", "Animation: {0}" ), animation_track_text ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-animation-opacity-key-label", "Delete" ),
                                LOCTEXT( "delete-animation-opacity-key-tooltip", "Delete the current key" ),
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
SCinematicBoardSectionAnimationOpacityKeys::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

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
    lines.Add( FText::Format( LOCTEXT( "tooltip-animation-opacity-key-animation-name", "Animation: {0}" ), animation_track_text ) );
    for( auto opacity : opacities )
        lines.Add( FText::Format( LOCTEXT( "tooltip-animation-opacity-key-value", "Opacity: {0}" ), FText::AsPercent( opacity ) ) );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), lines );
}

FText
SCinematicBoardSectionAnimationOpacityKeys::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();

    FText animation_text = FText::Format( LOCTEXT( "tooltip-animation-opacity-area-animation-name", "Animation: {0}" ), animation_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-animation-opacity-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), animation_text, num_keys_text );
}

FCursorReply
SCinematicBoardSectionAnimationOpacityKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionAnimationOpacityKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationOpacityKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimationOpacityKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionAnimationOpacityKeys::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .06f, .15f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionAnimationOpacityKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionAnimation
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionAnimation )
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
SCinematicBoardSectionAnimation::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
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
            SNew( SCinematicBoardSectionAnimationTitle, iBoardSection )
            .Binding( mBinding )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SKeysOverviewBox2, iBoardSection, mBinding )
            .HeightOverride( 8 )
            .Visibility( this, &SCinematicBoardSectionAnimation::GetKeysOverviewVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionAnimationTimelineKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionAnimation::GetKeysAreaVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionAnimationTransformKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionAnimation::GetKeysAreaVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionAnimationOpacityKeys, iBoardSection )
            .Binding( mBinding )
            .Visibility( this, &SCinematicBoardSectionAnimation::GetKeysAreaVisibility )
        ]
    ];
}

EVisibility
SCinematicBoardSectionAnimation::GetKeysOverviewVisibility() const
{
    return GetKeysAreaVisibility().IsVisible() ? EVisibility::Collapsed : EVisibility::Visible;
}

EVisibility
SCinematicBoardSectionAnimation::GetKeysAreaVisibility() const
{
    UMovieSceneCinematicBoardSection* section_object = Cast<UMovieSceneCinematicBoardSection>( mBoardSection.Pin()->GetSectionObject() );

    return section_object->IsAnimationKeysAreaVisible( mBinding.GetGuid() ) ? EVisibility::Visible : EVisibility::Collapsed;
}

void
SCinematicBoardSectionAnimation::BuildContextMenu( FMenuBuilder& ioMenuBuilder )
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneCinematicBoardSection* board_section_object = Cast<UMovieSceneCinematicBoardSection>( board_section->GetSectionObject() );
    UMovieSceneSequence* inner_sequence = board_section_object->GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FText animation_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();
    FText current_frame_text = FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) );

    //---

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *board_section_object, sequencer->GetFocusedTemplateID() );

    TSet<FGuid> animation_bindings_selected;
    TArray<FGuid> animation_bindings = ShotSequenceHelpers::GetAnimationBindings( *sequencer, result.mInnerSequence, result.mInnerSequenceId );
    for( FGuid animation_binding : animation_bindings )
    {
        TArray<AOdysseyAnimationActor*> animation_actors = ShotSequenceHelpers::GetAnimationSpawned( *sequencer, result.mInnerSequence, result.mInnerSequenceId, animation_binding );

        TArray<AOdysseyAnimationActor*> animation_actors_selected;
        ShotSequenceTools::FilterSelectedAnimations( animation_actors, &animation_actors_selected );
        if( animation_actors_selected.Num() )
            animation_bindings_selected.Add( animation_binding );
    }
    animation_bindings = animation_bindings_selected.Array();

    if( !animation_bindings.Contains( mBinding.GetGuid() ) )
    {
        BoardSequenceTools::SelectSingleAnimation( sequencer, board_section_object, mBinding.GetGuid() );

        animation_bindings = { mBinding.GetGuid() };
    }

    if( animation_bindings.Num() > 1 )
        animation_track_text = LOCTEXT( "animations-selected", "(selected)" );

    //---

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "animation-section-label", "Animation: {0}" ), animation_track_text ) );

    auto ToggleAnimationVisibility = [this, animation_bindings]()
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();
        BoardSequenceTools::ToggleAnimationVisibility( sequencer, subsection_object, animation_bindings, mBinding.GetGuid() );
    };

    bool is_animation_visible = BoardSequenceTools::IsAnimationVisible( sequencer, *board_section_object, mBinding.GetGuid() );
    FText label_text   = is_animation_visible ? LOCTEXT( "hide-animation-actor-label", "Hide" )                                 : LOCTEXT( "show-animation-actor-label", "Show" );
    FText tooltip_text = is_animation_visible ? LOCTEXT( "hide-animation-actor-tooltip", "Hide animation actor" )                   : LOCTEXT( "show-animation-actor-tooltip", "Show animation actor" );
    FSlateIcon icon    = is_animation_visible ? FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.VisibleIcon16x" ) : FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Level.NotVisibleIcon16x" );

    ioMenuBuilder.AddMenuEntry(
        label_text,
        tooltip_text,
        icon,
        FUIAction(
            FExecuteAction::CreateLambda( ToggleAnimationVisibility )
        ) );

    //-

    auto DetachAnimation = [this, animation_bindings]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        BoardSequenceTools::DetachAnimation( sequencer, subsection_object, animation_bindings );
    };

    auto CanDetachAnimation = [this, animation_bindings]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        return BoardSequenceTools::CanDetachAnimation( sequencer, subsection_object, animation_bindings );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "detach-animation-label", "Detach {0}" ), animation_track_text ),
        LOCTEXT( "detach-animation-tooltip", "Detach the animation" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "DetachAnimation" ),
        FUIAction(
            FExecuteAction::CreateLambda( DetachAnimation ),
            FCanExecuteAction::CreateLambda( CanDetachAnimation )
        ) );

    //-

    bool is_keys_area_visible = board_section_object->IsAnimationKeysAreaVisible( mBinding.GetGuid() );
    label_text   = is_keys_area_visible ? LOCTEXT( "hide-animation-keys-area-label", "Collapse" )           : LOCTEXT( "show-animation-keys-area-label", "Expand" );
    tooltip_text = is_keys_area_visible ? LOCTEXT( "hide-animation-keys-area-tooltip", "Hide keys area" )   : LOCTEXT( "show-animation-keys-area-tooltip", "Show keys area" );

    ioMenuBuilder.AddMenuEntry(
        label_text,
        tooltip_text,
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateStatic( ToggleKeysAreaVisibility5, board_section_object, animation_bindings, mBinding.GetGuid() )
        ) );

    //-

    auto DeleteAnimation = [this, animation_bindings]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::DeleteAnimation( sequencer, subsection_object, animation_bindings );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "delete-animation-label", "Delete {0}" ), animation_track_text ),
        LOCTEXT( "delete-animation-tooltip", "Delete the animation and its corresponding actor" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
        FUIAction( FExecuteAction::CreateLambda( DeleteAnimation ) ) );

    ioMenuBuilder.EndSection();

    //-

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "drawing-section-label", "Drawing" ) );

    auto CreateOpacity = [this, animation_bindings]( float iOpacity )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::CreateOpacity( sequencer, subsection_object, local_frame, animation_bindings, iOpacity );
    };

    auto CanCreateOpacity = [this, animation_bindings]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateOpacity( sequencer, subsection_object, local_frame, animation_bindings );
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
        FText::Format( LOCTEXT( "create-drawing-opacity-label", "Create {0}|plural(one=opacity,other=opacities) at {1}" ), animation_bindings.Num(), current_frame_text ),
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
SCinematicBoardSectionAnimation::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionAnimation::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        UMovieSceneSection* section_object = board_section->GetSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        if( MouseEvent.IsControlDown() )
            BoardSequenceTools::SelectMultiAnimation( sequencer, subsection_object, mBinding.GetGuid() );
        else
            BoardSequenceTools::SelectSingleAnimation( sequencer, subsection_object, mBinding.GetGuid() );

        return FReply::Handled();
    }
    else if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
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

SCinematicBoardSectionAnimations::~SCinematicBoardSectionAnimations()
{
    if( mSequencer.IsValid() )
        mSequencer.Pin()->OnMovieSceneDataChanged().Remove( mRebuildAnimationListHandle );
}

void
SCinematicBoardSectionAnimations::RebuildAnimationList( EMovieSceneDataChangeType iType )
{
    mNeedRebuildAnimationList = true;
}

void
SCinematicBoardSectionAnimations::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;
    mSequencer = mBoardSection.Pin()->GetSequencer();

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    mRebuildAnimationListHandle = mSequencer.Pin()->OnMovieSceneDataChanged().AddSP( this, &SCinematicBoardSectionAnimations::RebuildAnimationList );

    check( !mPossessables.Num() );

    RebuildAnimationList();

    //---

    FSlimHorizontalToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBarLarge" );

    MiddleToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionAnimations::MakeCreateAnimationMenu ),
        FText::GetEmpty(),
        LOCTEXT( "create-animation-and-settings-tooltip", "Create a new animation" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "CreateAnimation" ) );

    auto IsToolBarVisible = [this]() -> EVisibility
    {
        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();

        if( !IsFocusedSequenceSameAs( sequencer, subsection_object ) )
            return EVisibility::Hidden;

        bool is_visible = mPopupWidget.IsValid() || mOptionalWidgetsVisibility.Get().IsVisible();
        is_visible &= BoardSequenceTools::CanCreateAnimation( sequencer, section_object->GetInclusiveStartFrame() );

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
            SAssignNew( mWidgetAnimationList, SListView<TSharedRef<FMovieScenePossessable>> )
            .ListItemsSource( &mPossessables )
            .OnGenerateRow( this, &SCinematicBoardSectionAnimations::MakeAnimationRow )
            .SelectionMode( ESelectionMode::None )
        ]
        + SVerticalBox::Slot()
        .HAlign( HAlign_Center )
        .AutoHeight()
        [
            middle_toolbar
        ]
    ];
}

void
SCinematicBoardSectionAnimations::CreateAnimation( TSharedRef<FString> iAnimationName )
{
    if( !mBoardSection.IsValid() )
        return;

    ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
    UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();

    sequencer->SetLocalTimeDirectly( section_object->GetInclusiveStartFrame() );

    FAnimationArgs animation_args;
    animation_args.mName = *iAnimationName;
    animation_args.mSpawnable = GetDefault<UEposTracksEditorSettings>()->bSpawnable;
    BoardSequenceTools::CreateAnimation( sequencer, section_object->GetInclusiveStartFrame(), animation_args );
}

TSharedRef<SWidget>
SCinematicBoardSectionAnimations::MakeCreateAnimationMenu()
{
    FMenuBuilder menuBuilder( true, mSequencer.Pin()->GetCommandBindings() );

    //---

    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );
    UEposMovieSceneSequence* inner_epos_sequence = Cast<UEposMovieSceneSequence>( result.mInnerSequence );
    check( inner_epos_sequence );

    FString animation_path_not_used;
    TSharedRef<FString> animation_name = MakeShared<FString>();
    NamingConvention::GenerateAnimationActorPathName( *sequencer, *inner_epos_sequence, result.mInnerSequenceId, true /* Whatever as path is not used */, animation_path_not_used, *animation_name );

    //---

    EposTracksToolbarHelpers::MakeAnimationEntries( menuBuilder, animation_name, FSimpleDelegate::CreateRaw( this, &SCinematicBoardSectionAnimations::CreateAnimation, animation_name ) );
    EposTracksToolbarHelpers::MakeAnimationActorSettingsEntries( menuBuilder );
    EposTracksToolbarHelpers::MakeAnimationSettingsEntries( menuBuilder );

    //---

    auto CreateAnimationOnClick = [this, animation_name]() -> FReply
    {
        if( !mBoardSection.IsValid() )
            return FReply::Unhandled();

        CreateAnimation( animation_name );

        const UEposTracksEditorSettings& settings = *GetDefault<UEposTracksEditorSettings>();
        if( settings.BoardTrackSettings.GoToSectionStartFrameAfterCreationState == true )
        {
            FFrameNumber start_frame = UE::MovieScene::DiscreteInclusiveLower( mBoardSection.Pin()->GetSectionObject()->GetTrueRange() );
            mSequencer.Pin()->SetLocalTime( start_frame, ESnapTimeMode::STM_All, true );
        }

        return FReply::Handled();
    };

    auto CanCreateAnimation = [this]() -> bool
    {
        if( !mBoardSection.IsValid() )
            return false;

        //PATCH
        if( !GCurrentLevelEditingViewportClient )
            return false;
        //PATCH

        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        return BoardSequenceTools::CanCreateAnimation( sequencer, section_object->GetInclusiveStartFrame() );
    };

    menuBuilder.AddWidget(
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            [
                SNew( SCheckBox )
                    .IsChecked_Lambda( [this]()
                                       {
                                           const UEposTracksEditorSettings& settings = *GetDefault<UEposTracksEditorSettings>();
                                           return settings.BoardTrackSettings.GoToSectionStartFrameAfterCreationState ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
                                       } )
                    .OnCheckStateChanged_Lambda( [this]( ECheckBoxState iState )
                                                 {
                                                     UEposTracksEditorSettings* settings = GetMutableDefault<UEposTracksEditorSettings>();
                                                     settings->BoardTrackSettings.GoToSectionStartFrameAfterCreationState = ( iState == ECheckBoxState::Checked );
                                                     settings->SaveConfig();
                                                 } )
                .ToolTipText( LOCTEXT( "reset-to-section-start-frame-tooltip", "Go to start frame of the section after creating a new animation.\nOtherwise keep the current frame." ) )
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "reset-to-section-start-frame-label", "Go to section start frame" ) )
                ]
            ]
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 0, 10, 0, 0 )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            [
                SNew( SButton )
                .Text( LOCTEXT( "create-animation-label", "Create a new animation" ) )
                .ToolTipText( LOCTEXT( "create-animation-tooltip", "Create a new animation with those settings" ) )
                .OnClicked_Lambda( CreateAnimationOnClick )
                .IsEnabled_Lambda( CanCreateAnimation )
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
            .Visibility_Lambda( []() -> EVisibility
                                {
                                    return !GCurrentLevelEditingViewportClient ? EVisibility::Visible : EVisibility::Collapsed;
                                } )
        ],
        //PATCH
        FText::GetEmpty(),
        true /* NoIndent */ );

    TSharedRef<SWidget> widget = menuBuilder.MakeWidget();
    mPopupWidget = widget;
    return widget;
}


class STableRowAnimation
    : public STableRow<TSharedPtr<FString>>
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override
    {
        return FReply::Unhandled();
    }
};

TSharedRef<ITableRow>
SCinematicBoardSectionAnimations::MakeAnimationRow( TSharedRef<FMovieScenePossessable> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    if( !mBoardSection.IsValid() )
        return SNew( STableRowAnimation, iOwnerTable );

    return
        SNew( STableRowAnimation, iOwnerTable )
        [
            SNew( SCinematicBoardSectionAnimation, mBoardSection.Pin().ToSharedRef() )
            .Binding( *iItem )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ];
}

//static
//int
//GetMaxAnimationCount( IMovieScenePlayer& iPlayer, const UMovieSceneTrack* iTrack, FMovieSceneSequenceIDRef iSequenceID )
//{
//    int count = 0;
//    for( auto section : iTrack->GetAllSections() )
//    {
//        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );
//
//        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, *subsection, iSequenceID );
//        TArray<FGuid> guids;
//        int animation_count = ShotSequenceHelpers::GetAllAnimations( iPlayer, result.mInnerSequence, result.mInnerSequenceId, EGetAnimation::kAlwaysAll, nullptr, &guids );
//
//        count = FMath::Max( count, animation_count );
//    }
//
//    return count;
//}

void
SCinematicBoardSectionAnimations::RebuildAnimationList()
{
    if( !mBoardSection.IsValid() )
        return;

    //---

    if( !mNeedRebuildAnimationList )
        return;

    mNeedRebuildAnimationList = false;

    //---

    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    if( !inner_moviescene )
    {
        mPossessables.Empty();

        if( mWidgetAnimationList )
            mWidgetAnimationList->RequestListRefresh();
            //mWidgetAnimationList->RebuildList();

        return;
    }

    //---

    // Get all unordered animations
    TArray<FGuid> unordered_animation_bindings = BoardSequenceHelpers::GetAnimationBindings( *mSequencer.Pin().Get(), subsection, mSequencer.Pin()->GetFocusedTemplateID() );

    // Find their corresponding scene binding
    TArray<FMovieSceneBinding*> ordered_scene_bindings;
    for( auto unordered_animation_binding : unordered_animation_bindings )
    {
        ordered_scene_bindings.Add( inner_moviescene->FindBinding( unordered_animation_binding ) );
    }

    // Sort scene bindings by their sorting order/name
    // (This should match the native sorting of tracks inside shot)
    Algo::StableSort( ordered_scene_bindings, [inner_moviescene]( FMovieSceneBinding* iA, FMovieSceneBinding* iB )
                      {
                          // If at least one of the binding was not already sorted (by drag'n drop in shot), use the name to sort both
                          if( iA->GetSortingOrder() == -1 || iB->GetSortingOrder() == -1 )
                          {
                              FString nameA = inner_moviescene->GetObjectDisplayName( iA->GetObjectGuid() ).ToString();
                              FString nameB = inner_moviescene->GetObjectDisplayName( iB->GetObjectGuid() ).ToString();

                              return nameA < nameB;
                          }
                          // Otherwise just use the set sorting order
                          else
                              return iA->GetSortingOrder() < iB->GetSortingOrder();
                      } );

    // Get all animations in the gui order
    TArray<FGuid> ordered_animation_bindings;
    for( auto ordered_scene_binding : ordered_scene_bindings )
    {
        ordered_animation_bindings.Add( ordered_scene_binding->GetObjectGuid() );
    }

    //---

    auto need_rebuild = [this]( const TArray<FGuid>& iBindings )
    {
        if( iBindings.Num() != mPossessables.Num() )
            return true;

        if( !iBindings.Num() ) // Rebuild when no possessables, otherwise list view will be empty and not containing max_animations rows (with invalid guid)
            return true;

        for( int i = 0; i < iBindings.Num(); i++ )
            if( iBindings[i] != mPossessables[i]->GetGuid() )
                return true;

        return false;
    };
    if( !need_rebuild( ordered_animation_bindings ) ) //TOCHECK: check if it's really ok
        return;

    mPossessables.Empty();

    // This doesn't work because this vertical box won't have the same size for all sections
    // and as the height of a track node is getting from the first section in the array (and not necessary the one at the first position in the gui)
    // if the first section has no (or less) animations than others, all animations in the vertical box won't be displayed
    //int max_animations = possessables.Num();

    // Now it seems possible with the new way to compute each section height from the track

    //int max_animations = GetMaxAnimationCount( *sequencer, subsection.GetTypedOuter<UMovieSceneTrack>(), sequencer->GetFocusedTemplateID() );
    //for( int i = 0; i < max_animations; i++ )
    //{
    //    FMovieScenePossessable possessable;
    //    if( bindings.IsValidIndex( i ) )
    //        possessable = *inner_moviescene->FindPossessable( bindings[i] );

    //    mPossessables.Add( MakeShared<FMovieScenePossessable>( possessable ) );
    //}

    for( int i = 0; i < unordered_animation_bindings.Num(); i++ )
    {
        FMovieScenePossessable possessable = *inner_moviescene->FindPossessable( ordered_animation_bindings[i] );

        mPossessables.Add( MakeShared<FMovieScenePossessable>( possessable ) );
    }

    if( mWidgetAnimationList )
        mWidgetAnimationList->RequestListRefresh();
        //mWidgetAnimationList->RebuildList();
}

void
SCinematicBoardSectionAnimations::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) //override
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

    RebuildAnimationList();
}

#undef LOCTEXT_NAMESPACE
