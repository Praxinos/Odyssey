// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "CinematicBoardWidgets/SCinematicBoardSectionCamera.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "CineCameraActor.h"
#include "KeyDrawParams.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "SequencerSettings.h"
#include "Styling/StyleColors.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "EposSequenceHelpers.h"
#include "Shot/ShotSequence.h"
#include "Styles/EposTracksEditorStyle.h"
#include "Tools/EposSequenceTools.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionCamera"

//---

class SInlineEditableTextBlockOnDoubleClick3
    : public SInlineEditableTextBlock
{
    virtual FReply OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) override;
};

FReply
SInlineEditableTextBlockOnDoubleClick3::OnMouseButtonDoubleClick( const FGeometry& InMyGeometry, const FPointerEvent& InMouseEvent ) //override
{
    SInlineEditableTextBlock::OnMouseButtonDoubleClick( InMyGeometry, InMouseEvent );

    EnterEditingMode();

    return FReply::Handled();
}

//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionCameraTitle
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionCameraTitle )
        {}
        SLATE_ATTRIBUTE( EVisibility, OptionalWidgetsVisibility )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

    virtual FReply OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual FReply OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

private:
    FSlateColor         GetBackgroundTint() const;

    FText               HandleTitleText() const;
    void                HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType );

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;
};

void
SCinematicBoardSectionCameraTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    //---

    FSlimHorizontalToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetStyle( &FEposTracksEditorStyle::Get(), "SectionTitleToolBar" );

    auto PilotEject = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
            BoardSequenceTools::EjectCamera( sequencer, subsection_object, local_frame );
        else
            BoardSequenceTools::PilotCamera( sequencer, subsection_object, local_frame );
    };

    auto CanPilotEject = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        return BoardSequenceTools::CanPilotCamera( sequencer, subsection_object, local_frame )
                || BoardSequenceTools::CanEjectCamera( sequencer, subsection_object, local_frame );
    };

    auto IsPilotChecked = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

        return BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object );
    };

    TAttribute<FText> GetTooltip = MakeAttributeLambda(
        [this]() -> FText
        {
            ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
            const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
            FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

            if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
                return LOCTEXT( "eject-camera-tooltip", "Eject the existing camera" );
            else
                return LOCTEXT( "pilot-camera-tooltip", "Pilot the existing camera (create a camera and set the current frame where to create the camera keyframe)" );
        } );

    TAttribute<FSlateIcon> GetIcon = MakeAttributeLambda(
        [this]() -> FSlateIcon
        {
            ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
            const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
            FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;

            if( BoardSequenceTools::IsPilotingCamera( sequencer, subsection_object ) )
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "EjectCamera" );
            else
                return FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "PilotCamera" );
        } );

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( PilotEject ),
            FCanExecuteAction::CreateLambda( CanPilotEject ),
            FIsActionChecked::CreateLambda( IsPilotChecked )
        ),
        NAME_None,
        FText::GetEmpty(),
        GetTooltip,
        GetIcon,
        EUserInterfaceActionType::ToggleButton );

    //-

    auto Snap = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::SnapCameraToViewport( sequencer, subsection_object, local_frame );
    };

    auto CanSnap = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanSnapCameraToViewport( sequencer, subsection_object, local_frame );
    };

    LeftToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( Snap ),
            FCanExecuteAction::CreateLambda( CanSnap )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "snap-camera-to-viewport-tooltip", "Snap the existing camera to the viewport (create a camera and set the current frame where to create the camera keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get().GetStyleSetName(), "SnapCameraToViewport" ) );

    auto IsToolBarVisible = [this]() -> EVisibility
    {
        bool is_visible = mOptionalWidgetsVisibility.Get().IsVisible();
        is_visible &= !!Cast<UShotSequence>( mBoardSection.Pin()->GetSubSectionObject().GetSequence() );

        return is_visible ? EVisibility::Visible : EVisibility::Hidden;
    };

    TSharedRef< SWidget > left_toolbar = LeftToolbarBuilder.MakeWidget();
    left_toolbar->SetVisibility( MakeAttributeLambda( IsToolBarVisible ) );

    //---

    ChildSlot
    .HAlign( HAlign_Fill )
    [
        SNew( SBorder )
        .BorderImage( FAppStyle::Get().GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" ) )
        .BorderBackgroundColor( this, &SCinematicBoardSectionCameraTitle::GetBackgroundTint )
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
                SNew( SInlineEditableTextBlockOnDoubleClick3 )
                .Text( this, &SCinematicBoardSectionCameraTitle::HandleTitleText )
                .OnTextCommitted( this, &SCinematicBoardSectionCameraTitle::HandleTitleTextOnCommited )
            ]
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                SNew( SSpacer )
            ]
        ]
    ];
}

//---

FCursorReply
SCinematicBoardSectionCameraTitle::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionCameraTitle::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
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
SCinematicBoardSectionCameraTitle::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        UMovieSceneSection* section_object = board_section->GetSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        ACineCameraActor* camera = BoardSequenceHelpers::GetCamera( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

        // To unselect section(s)
        sequencer->EmptySelection();
        // And then select the current one
        sequencer->SelectSection( section_object );

        // To unselect all actors
        GEditor->SelectNone( true, true );
        // And then select the current one
        GEditor->SelectActor( camera, true, true );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FText
SCinematicBoardSectionCameraTitle::HandleTitleText() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FGuid camera_binding;
    BoardSequenceHelpers::GetCamera( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID(), &camera_binding );

    UMovieSceneSequence* sequence = subsection_object->GetSequence();
    UMovieScene* movie_scene = sequence ? sequence->GetMovieScene() : nullptr;

    return movie_scene ? movie_scene->GetObjectDisplayName( camera_binding ) : FText::GetEmpty();
}

void
SCinematicBoardSectionCameraTitle::HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType )
{
    if( iType != ETextCommit::OnEnter )
        return;

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FGuid camera_binding;
    BoardSequenceHelpers::GetCamera( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID(), &camera_binding );

    BoardSequenceTools::RenameBinding( sequencer, *subsection_object, camera_binding, iText.ToString() );
}

FSlateColor
SCinematicBoardSectionCameraTitle::GetBackgroundTint() const
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    ACineCameraActor* camera = BoardSequenceHelpers::GetCamera( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

    // Same as in ...\Engine\Source\Editor\Sequencer\Private\SAnimationOutlinerTreeNode.cpp::GetNodeBackgroundTint()
    if( camera && camera->IsSelected() )
        return FStyleColors::Select;

    return FStyleColors::Header;
}

//---
//---
//---

class EPOSTRACKSEDITOR_API SCinematicBoardSectionCameraTransform
    : public SMetaKeysArea
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionCameraTransform )
        {}
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
};

//---

void
SCinematicBoardSectionCameraTransform::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    SMetaKeysArea::Construct( SMetaKeysArea::FArguments(), iBoardSection );

    ChildSlot
    [
        SNew( SBox )
        //[
        //    SNew( STextBlock )
        //    .Text( GetBindingName() ) )
        //]
    ];
}

//---

TSharedPtr<FMetaChannel>
SCinematicBoardSectionCameraTransform::GetMetaChannel() //override
{
    return mBoardSection.Pin()->GetCameraTransformMetaChannel();
}

TSharedPtr<const FMetaChannel>
SCinematicBoardSectionCameraTransform::GetMetaChannel() const //override
{
    return mBoardSection.Pin()->GetCameraTransformMetaChannel();
}

void
SCinematicBoardSectionCameraTransform::RebuildMetaChannel() //override
{
    mBoardSection.Pin()->ReBuildCameraTransformMetaChannel();
}

//---

bool
SCinematicBoardSectionCameraTransform::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder, TSharedPtr<FMetaChannel> iKeys ) //override
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

        BoardSequenceTools::DeleteCameraKey( sequencer, *subsection_object, sections, channelHandles, keyHandles );
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaChannel> iKeys ) -> bool
    {
        return true;
    };

    //-

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection& subsection = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FGuid camera_binding;
    BoardSequenceTools::GetCamera( sequencer, subsection, &camera_binding );

    FText camera_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( camera_binding ) : FText::GetEmpty();

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "camera-section-label", "Camera: {0}" ), camera_track_text ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-camera-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for the 9 (maybe more or less) channels
                                LOCTEXT( "delete-camera-key-tooltip", "Delete the current key" ),
                                FSlateIcon( FAppStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, iKeys ) ) );

    ioMenuBuilder.EndSection();

    return true;
}

FText
SCinematicBoardSectionCameraTransform::GetKeyTooltipText( TSharedPtr<FMetaChannel> iKeys ) const //override
{
    return GetAreaTooltipText();
}

FText
SCinematicBoardSectionCameraTransform::GetAreaTooltipText() const //override
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    UMovieSceneSubSection& subsection = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FGuid camera_binding;
    ACineCameraActor* camera = BoardSequenceTools::GetCamera( sequencer, subsection, &camera_binding );

    FText camera_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( camera_binding ) : FText::GetEmpty();

    //-

    if( camera_track_text.IsEmpty() )
        return LOCTEXT( "tooltip-camera-no", "No camera" );

    FText camera_text = FText::Format( LOCTEXT( "tooltip-camera-area-track", "Camera: {0}" ), camera_track_text );
    FText num_keys_text = FText::Format( LOCTEXT( "tooltip-camera-area-num-keys", "Keys: {0}" ), GetMetaChannel()->NumMetaKeys() );

    return FText::Join( FText::FromString( TEXT( "\n" ) ), camera_text, num_keys_text );
}

//---

FCursorReply
SCinematicBoardSectionCameraTransform::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    return SMetaKeysArea::OnCursorQuery( MyGeometry, CursorEvent );
}

FReply
SCinematicBoardSectionCameraTransform::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionCameraTransform::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseButtonUp( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionCameraTransform::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    return SMetaKeysArea::OnMouseMove( MyGeometry, MouseEvent );
}

const FSlateBrush*
SCinematicBoardSectionCameraTransform::GetBackgroundBrush() const //override
{
    static FSlateColorBrush background_brush = FSlateColorBrush( FLinearColor( .15f, .06f, .14f ) );

    return &background_brush;
}

int32
SCinematicBoardSectionCameraTransform::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    return SMetaKeysArea::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---
//---
//---

void
SCinematicBoardSectionCamera::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mOptionalWidgetsVisibility = InArgs._OptionalWidgetsVisibility;

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionCameraTitle, iBoardSection )
            .OptionalWidgetsVisibility( mOptionalWidgetsVisibility )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionCameraTransform, iBoardSection )
        ]
    ];
}

#undef LOCTEXT_NAMESPACE
