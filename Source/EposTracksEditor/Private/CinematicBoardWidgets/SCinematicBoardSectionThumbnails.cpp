// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardWidgets/SCinematicBoardSectionThumbnails.h"

#include "Brushes/SlateColorBrush.h"
#include "MovieSceneToolsUserSettings.h"
#include "SequencerSectionPainter.h"
#include "Widgets/Input/SSpinBox.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MovieSceneCinematicBoardSection.h"
#include "EposTracksToolbarHelpers.h"
#include "NamingConvention.h"
#include "Settings/EposTracksEditorSettings.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionThumbnails"

//---

void
SCinematicBoardSectionThumbnails::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;
    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    LeftToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent ),
        FText::GetEmpty(),
        LOCTEXT( "AddBoardBefore", "Add a new board or shot before" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Plus" ) );

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();
    left_toolbar->SetVisibility( mOptionalWidgetsVisibility );

    //---

    FSlimHorizontalToolBarBuilder TopToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    TopToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    auto GetSwitchTakeTooltip = [this]() -> FText
    {
        FText take_tooltip = LOCTEXT( "switch-take-tooltip", "Switch take" );
        if( BoardSequenceTools::IsDrawingInEditionMode( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSubSectionObject() ) )
            take_tooltip = LOCTEXT( "switch-take-with-warning-tooltip", "Switch take\n\nDrawing(s) must not be in edition mode" );

        return take_tooltip;
    };

    TopToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction::CreateLambda( [this]() { return !BoardSequenceTools::IsDrawingInEditionMode( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSubSectionObject() ); } )
        ),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionThumbnails::MakeTakeMenu ),
        FText::GetEmpty(),
        MakeAttributeLambda( GetSwitchTakeTooltip ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "Take" ) );

    auto IsTopToolBarVisible = [this]() -> EVisibility
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( section_object );
        return ( board_section->GetTakes().Num() > 1 && mOptionalWidgetsVisibility.Get().IsVisible() ) ? EVisibility::Visible : EVisibility::Collapsed;
    };

    TSharedRef< SWidget > top_toolbar = TopToolbarBuilder.MakeWidget();
    top_toolbar->SetVisibility( MakeAttributeLambda( IsTopToolBarVisible ) );

    //---

    FSlimHorizontalToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    MiddleToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionThumbnails::MakeCreateCameraMenu ),
        FText::GetEmpty(),
        LOCTEXT( "create-camera-and-settings-tooltip", "Create a new camera" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Plus" ) );

    auto IsToolBarVisible = [this]() -> EVisibility
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        return BoardSequenceTools::CanCreateCamera( sequencer, section_object->GetInclusiveStartFrame() ) ? EVisibility::Visible : EVisibility::Hidden;
    };

    TSharedRef< SWidget > middle_toolbar = MiddleToolbarBuilder.MakeWidget();
    middle_toolbar->SetVisibility( MakeAttributeLambda( IsToolBarVisible ) );

    //---

    FSlimHorizontalToolBarBuilder RightToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    RightToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionFloatingToolBar" );

    RightToolbarBuilder.AddComboButton(
        FUIAction(),
        FOnGetContent::CreateSP( this, &SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent ),
        FText::GetEmpty(),
        LOCTEXT( "AddBoardAfter", "Add a new board or shot after" ),
        FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Plus" ) );

    TSharedRef< SWidget > right_toolbar = RightToolbarBuilder.MakeWidget();
    right_toolbar->SetVisibility( mOptionalWidgetsVisibility );

    //---

    ChildSlot
    [
        SNew( SOverlay )
        + SOverlay::Slot()
        [
            SNew( SBox )
        ]
        + SOverlay::Slot()
        .HAlign( HAlign_Fill )
        .VAlign( VAlign_Fill )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                left_toolbar
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Fill )
            [
                SNew( SVerticalBox )

                + SVerticalBox::Slot()
                .AutoHeight()
                .HAlign( HAlign_Center )
                [
                    top_toolbar
                ]

                + SVerticalBox::Slot()
                .FillHeight( 1 )
                .HAlign( HAlign_Center )
                .VAlign( VAlign_Center )
                [
                    middle_toolbar
                ]
                // No more needed as the creation button of camera is inside the popup
                //+ SVerticalBox::Slot()
                //.AutoHeight()
                //[
                //    SNew( STextBlock )
                //    .Text( LOCTEXT( "CreateCameraInfo", "Don't forget to setup\nyour camera settings" ) )
                //    .ColorAndOpacity( FLinearColor( .5f, .5f, .0f ) )
                //    .ShadowColorAndOpacity( FLinearColor::Black )
                //    .ShadowOffset( FVector2D( 1.f, 1.f ) )
                //    .Visibility_Lambda( [this]()
                //                        {
                //                            return BoardSequenceTools::CanCreateCamera( mBoardSection.Pin()->GetSequencer().Get(), mBoardSection.Pin()->GetSectionObject()->GetInclusiveStartFrame() )
                //                                && mOptionalWidgetsVisibility.Get() == EVisibility::Visible
                //                                ? EVisibility::Visible : EVisibility::Collapsed;
                //                        } )
                //]
            ]
            + SHorizontalBox::Slot()
            [
                SNew( SSpacer )
            ]
            + SHorizontalBox::Slot()
            .AutoWidth()
            .VAlign( VAlign_Center )
            [
                right_toolbar
            ]
        ]
    ];
}

void
SCinematicBoardSectionThumbnails::CreateCamera( TSharedRef<FString> iCameraName, TSharedRef<FString> iPlaneName )
{
    if( !mBoardSection.IsValid() )
        return;

    ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
    UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
    FCameraArgs camera_args;
    camera_args.mName = *iCameraName;
    FPlaneArgs plane_args;
    plane_args.mName = *iPlaneName;
    BoardSequenceTools::CreateCamera( sequencer, section_object->GetInclusiveStartFrame(), camera_args, plane_args );
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::MakeCreateCameraMenu()
{
    FMenuBuilder MenuBuilder( true, mBoardSection.Pin()->GetSequencer()->GetCommandBindings() );

    //---

    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );

    FString camera_path;
    TSharedRef<FString> camera_name = MakeShared<FString>();
    NamingConvention::GenerateCameraActorPathName( *sequencer, sequencer->GetRootMovieSceneSequence(), result.mInnerSequence, camera_path, *camera_name );

    FString plane_path;
    TSharedRef<FString> plane_name = MakeShared<FString>();
    NamingConvention::GeneratePlaneActorPathName( *sequencer, sequencer->GetRootMovieSceneSequence(), result.mInnerSequence, plane_path, *plane_name );

    //---

    EposTracksToolbarHelpers::MakeCameraEntries( MenuBuilder, camera_name, FSimpleDelegate::CreateRaw( this, &SCinematicBoardSectionThumbnails::CreateCamera, camera_name, plane_name ) );
    EposTracksToolbarHelpers::MakeCameraSettingsEntries( MenuBuilder );

    EposTracksToolbarHelpers::MakePlaneEntries( MenuBuilder, plane_name, FSimpleDelegate::CreateRaw( this, &SCinematicBoardSectionThumbnails::CreateCamera, camera_name, plane_name ), false /* iFocus */ );
    EposTracksToolbarHelpers::MakePlaneSettingsEntries( MenuBuilder );
    EposTracksToolbarHelpers::MakeTextureSettingsEntries( MenuBuilder );

    //---

    auto CreateCameraOnClick = [this, camera_name, plane_name]() -> FReply
    {
        if( !mBoardSection.IsValid() )
            return FReply::Unhandled();

        CreateCamera( camera_name, plane_name );

        return FReply::Handled();
    };

    auto CanCreateCamera = [this]() -> bool
    {
        if( !mBoardSection.IsValid() )
            return false;

        //PATCH
        if( !GCurrentLevelEditingViewportClient )
            return false;
        //PATCH

        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        return BoardSequenceTools::CanCreateCamera( sequencer, section_object->GetInclusiveStartFrame() );
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
                                   .Text( LOCTEXT( "create-camera-and-plane-label", "Create a new camera and its plane" ) )
                                   .ToolTipText( LOCTEXT( "create-camera-and-plane-tooltip", "Create a new camera and its plane with those settings" ) )
                                   .OnClicked_Lambda( CreateCameraOnClick )
                                   .IsEnabled_Lambda( CanCreateCamera )
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

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::MakeTakeMenu()
{
    FMenuBuilder MenuBuilder( true, mBoardSection.Pin()->GetSequencer()->GetCommandBindings() );

    //---

    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();
    UMovieSceneCinematicBoardSection* board_section = Cast<UMovieSceneCinematicBoardSection>( &subsection );

    for( auto take : board_section->GetTakes() )
    {
        TWeakObjectPtr<UMovieSceneSequence> take_sequence = take.GetSequence();
        if( !take_sequence.IsValid() )
            continue;

        MenuBuilder.AddMenuEntry(
            take_sequence->GetDisplayName(),
            FText::Format( LOCTEXT( "TakeNumberTooltip", "Switch to {0}" ), FText::FromString( take_sequence->GetPathName() ) ),
            take_sequence->GetPathName() == board_section->GetSequence()->GetPathName() ? FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Sequencer.Star" ) : FSlateIcon( FAppStyle::Get().GetStyleSetName(), "Sequencer.Empty" ),
            FUIAction( FExecuteAction::CreateLambda( [this, sequencer, board_section, take]()
                                                     {
                                                         BoardSequenceTools::SwitchTake( sequencer.Get(), *board_section, board_section->FindTake( take ) );
                                                     } ) )
        );
    }

    MenuBuilder.AddSeparator();

    MenuBuilder.AddMenuEntry(
        LOCTEXT( "NewTake", "New Take" ),
        FText::Format( LOCTEXT( "NewTakeTooltip", "Create a new take for {0}" ), FText::FromString( board_section->GetBoardDisplayName() ) ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "Take" ),
        FUIAction( FExecuteAction::CreateLambda( [this, sequencer, board_section]()
                                                 {
                                                     BoardSequenceTools::CreateTake( sequencer.Get(), *board_section );
                                                 } ) )
    );

    return MenuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardBeforeComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    TSharedRef<int32> numberOfNewSequence( new int32( 1 ) );

    auto InsertShot = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        for( int i = 0; i < *iNumberOfNewSequence; i++ )
        {
            CinematicBoardTrackTools::InsertShot( sequencer, section_object->GetInclusiveStartFrame() );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertShot, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationText( LOCTEXT( "section.create-shot-before-label", "New {0}|plural(one=Shot,other=Shots)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-before-tooltip", "Create a new section containing a new shot before this section with the same duration as the current section" ) );

    //-

    auto InsertShotWithDuration = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultSectionDuration * tick_resolution;
        for( int i = 0; i < *iNumberOfNewSequence; i++ )
        {
            CinematicBoardTrackTools::InsertShot( sequencer, section_object->GetInclusiveStartFrame(), time.RoundToFrame().Value );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertShotWithDuration, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationWidget( LOCTEXT( "section.create-shot-before-with-duration-label", "New {0}|plural(one=Shot,other=Shots)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-before-with-duration-tooltip", "Create a new section containing a new shot before this section with the specified duration" ) );

    //-

    menuBuilder.AddSeparator();

    //-

    auto InsertBoard = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        for( int i = 0; i < *iNumberOfNewSequence; i++ )
        {
            CinematicBoardTrackTools::InsertBoard( sequencer, section_object->GetInclusiveStartFrame() );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertBoard, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationText( LOCTEXT( "section.create-board-before-label", "New {0}|plural(one=Board,other=Boards)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-board-before-tooltip", "Create a new section containing a new board before this section with the same duration as the current section" ) );

    //-

    auto InsertBoardWithDuration = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultSectionDuration * tick_resolution;
        for( int i = 0; i < *iNumberOfNewSequence; i++ )
        {
            CinematicBoardTrackTools::InsertBoard( sequencer, section_object->GetInclusiveStartFrame(), time.RoundToFrame().Value );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertBoardWithDuration, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationWidget( LOCTEXT( "section.create-board-before-with-duration-label", "New {0}|plural(one=Board,other=Boards)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-board-before-with-duration-tooltip", "Create a new section containing a new board before this section with the specified duration" ) );

    //-

    menuBuilder.AddSeparator();

    //-

    auto CloneSection = [this]( bool iEmptyDrawings )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        UMovieSceneCinematicBoardSection* board_section = CastChecked<UMovieSceneCinematicBoardSection>( subsection_object );

        CinematicBoardTrackTools::CloneSection( sequencer, board_section, subsection_object->GetInclusiveStartFrame(), iEmptyDrawings );
    };

    auto CanCloneSection = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();

        return !!Cast<UShotSequence>( subsection_object->GetSequence() );

    };

    menuBuilder.AddMenuEntry( LOCTEXT( "section.clone-section-before-label", "Clone Shot" ),
                              LOCTEXT( "section.clone-section-before-tooltip", "Clone this shot before (actors and drawing assets will be cloned as well)" ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( CloneSection, false ),
                                         FCanExecuteAction::CreateLambda( CanCloneSection ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.clone-section-before-and-empty-drawings-label", "Clone Shot (with empty drawings)" ),
                              LOCTEXT( "section.clone-section-before-and-empty-drawings-tooltip", "Clone this shot before (actors and empty drawing assets will be cloned as well)" ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( CloneSection, true ),
                                         FCanExecuteAction::CreateLambda( CanCloneSection ) ) );

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::HandleAddBoardAfterComboButtonGetMenuContent()
{
    FMenuBuilder menuBuilder( true, nullptr );

    TSharedRef<int32> numberOfNewSequence( new int32( 1 ) );

    auto InsertShot = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        for( int i = 0; i < *iNumberOfNewSequence && subsection_object; i++ )
        {
            subsection_object = CinematicBoardTrackTools::InsertShot( sequencer, subsection_object->GetExclusiveEndFrame() - 1 );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertShot, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationText( LOCTEXT( "section.create-shot-after-label", "New {0}|plural(one=Shot,other=Shots)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-after-tooltip", "Create a new section containing a new shot after this section with the same duration as the current section" ) );

    //-

    auto InsertShotWithDuration = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultSectionDuration * tick_resolution;
        for( int i = 0; i < *iNumberOfNewSequence && subsection_object; i++ )
        {
            subsection_object = CinematicBoardTrackTools::InsertShot( sequencer, subsection_object->GetExclusiveEndFrame() - 1, time.RoundToFrame().Value );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertShotWithDuration, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationWidget( LOCTEXT( "section.create-shot-after-with-duration-label", "New {0}|plural(one=Shot,other=Shots)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-shot-after-with-duration-tooltip", "Create a new section containing a new shot after this section with the specified duration" ) );

    //-

    menuBuilder.AddSeparator();

    //-

    auto InsertBoard = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        for( int i = 0; i < *iNumberOfNewSequence && subsection_object; i++ )
        {
            subsection_object = CinematicBoardTrackTools::InsertBoard( sequencer, subsection_object->GetExclusiveEndFrame() - 1 );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertBoard, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationText( LOCTEXT( "section.create-board-after-label", "New {0}|plural(one=Board,other=Boards)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-board-after-tooltip", "Create a new section containing a new board after this section with the same duration as the current section" ) );

    //-

    auto InsertBoardWithDuration = [this]( TSharedPtr<int32> iNumberOfNewSequence )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultSectionDuration * tick_resolution;
        for( int i = 0; i < *iNumberOfNewSequence && subsection_object; i++ )
        {
            subsection_object = CinematicBoardTrackTools::InsertBoard( sequencer, subsection_object->GetExclusiveEndFrame() - 1, time.RoundToFrame().Value );
        }
    };

    menuBuilder.AddMenuEntry( FUIAction( FExecuteAction::CreateLambda( InsertBoardWithDuration, numberOfNewSequence ) ),
                              CreatePopupEntryNewSectionWithDurationWidget( LOCTEXT( "section.create-board-after-with-duration-label", "New {0}|plural(one=Board,other=Boards)" ), numberOfNewSequence ),
                              NAME_None,
                              LOCTEXT( "section.create-board-after-with-duration-tooltip", "Create a new section containing a new board after this section with the specified duration" ) );

    //-

    menuBuilder.AddSeparator();

    //-

    auto CloneSection = [this]( bool iEmptyDrawings )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();
        UMovieSceneCinematicBoardSection* board_section = CastChecked<UMovieSceneCinematicBoardSection>( subsection_object );

        CinematicBoardTrackTools::CloneSection( sequencer, board_section, subsection_object->GetExclusiveEndFrame() - 1, iEmptyDrawings );
    };

    auto CanCloneSection = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSubSection* subsection_object = &mBoardSection.Pin()->GetSubSectionObject();

        return !!Cast<UShotSequence>( subsection_object->GetSequence() );

    };

    menuBuilder.AddMenuEntry( LOCTEXT( "section.clone-section-after-label", "Clone Shot" ),
                              LOCTEXT( "section.clone-section-after-tooltip", "Clone this shot after (actors and drawing assets will be cloned as well)" ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( CloneSection, false ),
                                         FCanExecuteAction::CreateLambda( CanCloneSection ) ) );

    menuBuilder.AddMenuEntry( LOCTEXT( "section.clone-section-after-and-empty-drawings-label", "Clone Shot (with empty drawings)" ),
                              LOCTEXT( "section.clone-section-after-and-empty-drawings-tooltip", "Clone this shot after (actors and empty drawing assets will be cloned as well)" ),
                              FSlateIcon(),
                              FUIAction( FExecuteAction::CreateLambda( CloneSection, true ),
                                         FCanExecuteAction::CreateLambda( CanCloneSection ) ) );

    return menuBuilder.MakeWidget();
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::CreatePopupEntryNewSectionWithDurationWidget( FText iLabel, TSharedRef<int32> iNumberOfNewSequence )
{
    *iNumberOfNewSequence = 1; // Always create 1 by default

    auto OnNumberOfNewSequenceChanged = [=]( int32 iNewValue )
    {
        *iNumberOfNewSequence = iNewValue;
    };

    auto GetNumberOfNewSequence = [=]() -> int32
    {
        return *iNumberOfNewSequence;
    };

    //-

    auto OnDurationChanged = [=]( double iNewValue )
    {
        FFrameTime time = FFrameTime::FromDecimal( iNewValue );

        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameRate display_rate = sequencer->GetFocusedDisplayRate();
        time = FFrameRate::TransformTime( FFrameRate::TransformTime( time, tick_resolution, display_rate ).FloorToFrame(), display_rate, tick_resolution );

        double new_value_as_seconds = tick_resolution.AsSeconds( time.RoundToFrame() );

        GetMutableDefault<UEposTracksEditorSettings>()->SetDefaultSectionDuration( new_value_as_seconds );
    };

    auto GetDuration = [=]() -> double
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameRate display_rate = sequencer->GetFocusedDisplayRate();
        // Convert default duration in seconds (fe: 3.2 seconds) to 76800 frames in tick resolution (3.2 * 24000)
        FFrameTime time = GetDefault<UEposTracksEditorSettings>()->DefaultSectionDuration * tick_resolution;
        // Convert the duration to display rate (-> 76) and back again to tick resolution (-> 76000) to have a multiple of 1000 (scale between display rate and tick resolution)
        // And in this case, as the gui is displayed in display rate, 76000 is displayed as "00076" (and not "00076*" for 76800)
        time = FFrameRate::TransformTime( FFrameRate::TransformTime( time, tick_resolution, display_rate ).FloorToFrame(), display_rate, tick_resolution );

        return time.GetFrame().Value;
    };

    //-

    auto GetLabel = [=]() -> FText
    {
        return FText::Format( iLabel, *iNumberOfNewSequence );
    };

    //-

    ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
    FFrameRate tick_resolution = sequencer->GetFocusedTickResolution(); // For the example below: 24000 fps
    FFrameRate display_rate = sequencer->GetFocusedDisplayRate();       // For the example below: 24fps

    return SNew( SHorizontalBox )
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FEposTracksEditorStyle::Get().GetMargin( "SectionFloatingToolBar.Block.IndentedPadding" ) )
        [
            SNew( SSpinBox<int32> )
            .Style( FEposTracksEditorStyle::Get(), "HyperlinkSpinBox" )
            .OnValueCommitted_Lambda( [=] (int32 Value, ETextCommit::Type) { OnNumberOfNewSequenceChanged(Value); } )
            .OnValueChanged_Lambda( [=] (int32 Value) { OnNumberOfNewSequenceChanged(Value); } )
            .MinValue( 1 )
            .MaxValue( 10 )
            .Value_Lambda( GetNumberOfNewSequence )
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FMargin( 5, 0, 0, 0 ) )
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text_Lambda( GetLabel )
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SSpacer)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FEposTracksEditorStyle::Get().GetMargin( "SectionFloatingToolBar.Block.IndentedPadding" ) )
        [
            SNew( SSpinBox<double> )
            .TypeInterface( sequencer->GetNumericTypeInterface() )
            .Style( FEposTracksEditorStyle::Get(), "HyperlinkSpinBox" )
            .OnValueCommitted_Lambda( [=] (double Value, ETextCommit::Type) { OnDurationChanged(Value); } )
            .OnValueChanged_Lambda( [=] (double Value) { OnDurationChanged(Value); } )
            .MinValue( FFrameRate::TransformTime( 1, display_rate, tick_resolution ).GetFrame().Value ) // Convert 1 frame in display rate (24fps) to 1000 frames in tick resolution (24000fps)
            .Value_Lambda( GetDuration )
        ];
}

TSharedRef<SWidget>
SCinematicBoardSectionThumbnails::CreatePopupEntryNewSectionWithDurationText( FText iLabel, TSharedRef<int32> iNumberOfNewSequence )
{
    *iNumberOfNewSequence = 1; // Always create 1 by default

    auto OnNumberOfNewSequenceChanged = [=]( int32 iNewValue )
    {
        *iNumberOfNewSequence = iNewValue;
    };

    auto GetNumberOfNewSequence = [=]() -> int32
    {
        return *iNumberOfNewSequence;
    };

    //-

    auto GetDuration = [this]() -> FString
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        FFrameRate tick_resolution = sequencer->GetFocusedTickResolution();
        FFrameRate display_rate = sequencer->GetFocusedDisplayRate();
        FFrameTime time = section_object->GetTrueRange().Size<FFrameNumber>();
        time = FFrameRate::TransformTime( FFrameRate::TransformTime( time, tick_resolution, display_rate ).FloorToFrame(), display_rate, tick_resolution );
        FString time_string = sequencer->GetNumericTypeInterface()->ToString( time.GetFrame().Value );

        return time_string;
    };

    //-

    auto GetLabel = [=]() -> FText
    {
        return FText::Format( iLabel, *iNumberOfNewSequence );
    };

    //-

    return SNew( SHorizontalBox )
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FEposTracksEditorStyle::Get().GetMargin( "SectionFloatingToolBar.Block.IndentedPadding" ) )
        [
            SNew( SSpinBox<int32> )
            .Style( FEposTracksEditorStyle::Get(), "HyperlinkSpinBox" )
            .OnValueCommitted_Lambda( [=] (int32 Value, ETextCommit::Type) { OnNumberOfNewSequenceChanged(Value); } )
            .OnValueChanged_Lambda( [=] (int32 Value) { OnNumberOfNewSequenceChanged(Value); } )
            .MinValue( 1 )
            .MaxValue( 10 )
            .Value_Lambda( GetNumberOfNewSequence )
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FMargin( 5, 0, 0, 0 ) )
        .VAlign( VAlign_Center )
        [
            SNew( STextBlock )
            .Text_Lambda( GetLabel )
        ]
        + SHorizontalBox::Slot()
        [
            SNew(SSpacer)
        ]
        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding( FEposTracksEditorStyle::Get().GetMargin( "SectionFloatingToolBar.Block.IndentedPadding" ) )
        [
            SNew( STextBlock )
            .TextStyle( FAppStyle::Get(), "NormalText.Subdued" )
            .Text( FText::FromString( GetDuration() ) )
        ];
}

FVector2D
SCinematicBoardSectionThumbnails::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();

    auto* Settings = GetDefault<UMovieSceneUserThumbnailSettings>();
    size.Y = Settings->bDrawThumbnails ? Settings->ThumbnailSize.Y : SequencerSectionConstants::DefaultSectionHeight;
    size.Y = size.Y + 2 * SequencerSectionConstants::DefaultSectionHeight /* top/bottom film-border */;

    return size;
}

//---

static
FTimeToPixel
ConstructTimeConverterForSection( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
}

struct FSequencerSectionPainterImpl
    : FSequencerSectionPainter
{
public:
    /** Constructor */
    FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry );

    /** Virtual destructor */
    virtual ~FSequencerSectionPainterImpl();

    /** Paint the section background with the specified tint override */
    virtual int32 PaintSectionBackground( const FLinearColor& Tint );

public:
    /** Get a time-to-pixel converter for the section */
    virtual const FTimeToPixel& GetTimeConverter() const;

public:
    FTimeToPixel TimeToPixelConverter;
};

FSequencerSectionPainterImpl::FSequencerSectionPainterImpl( UMovieSceneSection& InSection, FSlateWindowElementList& _OutDrawElements, const FGeometry& InSectionGeometry )
    : FSequencerSectionPainter( _OutDrawElements, InSectionGeometry, InSection )
    , TimeToPixelConverter( ConstructTimeConverterForSection( SectionGeometry, InSection ) )
{
}

FSequencerSectionPainterImpl::~FSequencerSectionPainterImpl()
{
}

int32
FSequencerSectionPainterImpl::PaintSectionBackground( const FLinearColor& Tint )
{
    return 0;
}

const FTimeToPixel&
FSequencerSectionPainterImpl::GetTimeConverter() const
{
    return TimeToPixelConverter;
}

//---

int32
SCinematicBoardSectionThumbnails::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBoardSection.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection*  subsection_object = &board_section->GetSubSectionObject();

    FSequencerSectionPainterImpl painter( *subsection_object, OutDrawElements, AllottedGeometry );
    //painter.KeyAreaElements = ;
    painter.SectionClippingRect = board_section->GetRootPainter( Args )->SectionClippingRect.IntersectionWith( MyCullingRect );
    painter.LayerId = LayerId;
    painter.bParentEnabled = board_section->GetRootPainter( Args )->bParentEnabled;
    painter.bIsHighlighted = board_section->GetRootPainter( Args )->bIsHighlighted;
    painter.bIsSelected = board_section->GetRootPainter( Args )->bIsSelected;

    //---

    static const FSlateBrush* filmBorder = FAppStyle::Get().GetBrush( "Sequencer.Section.FilmBorder" );

    FVector2D localSectionSize = painter.SectionGeometry.GetLocalSize();

    // Paint fancy-looking film border.
    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, 4.f ) ) ),
        filmBorder
    );

    FSlateDrawElement::MakeBox(
        painter.DrawElements,
        painter.LayerId++,
        painter.SectionGeometry.ToPaintGeometry( FVector2D( localSectionSize.X - 2.f, 7.f ), FSlateLayoutTransform( FVector2D( 1.f, localSectionSize.Y - 11.f ) ) ),
        filmBorder
    );

    //---

    painter.LayerId = board_section->FKeyThumbnailSection::OnPaintSection( painter );

    //---

    // Paint the sub-sequence information/looping boundaries/etc.

    FSubSectionPainterParams subSectionPainterParams( board_section->GetContentPadding() );
    subSectionPainterParams.bShowTrackNum = false;

    FSubSectionPainterUtil::PaintSection( board_section->GetSequencer(), *subsection_object, painter, subSectionPainterParams );

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, painter.LayerId, InWidgetStyle, bParentEnabled );
}

#undef LOCTEXT_NAMESPACE
