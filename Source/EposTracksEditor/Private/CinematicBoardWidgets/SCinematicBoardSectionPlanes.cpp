// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sections/MovieScene3DTransformSection.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"
#include "SequencerSettings.h"
#include "Subsystems/AssetEditorSubsystem.h"
#include "Widgets/Text/SInlineEditableTextBlock.h"

#include "EposSequenceHelpers.h"
#include "EposTracksToolbarHelpers.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
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

    //void                ToggleLighttable();
    //ECheckBoxState      IsLighttableOn() const;

    FSlateColor         GetBackgroundTint() const;

    FText               HandleTitleText() const;
    void                HandleTitleTextOnCommited( const FText& iText, ETextCommit::Type iType );

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;
    TAttribute<EVisibility>             mOptionalWidgetsVisibility;

    /** Delegate binding handle for ISequencer::OnMovieSceneDataChanged */
    FDelegateHandle mMovieSceneDataChangedHandle;
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
    //TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    //UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    //BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );

    ////---

    //LighttableTools::Update( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
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

    FToolBarBuilder LeftToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    LeftToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    LeftToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSectionTitle.ToolBar" );

    auto DetachPlane = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
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
            FIsActionButtonVisible::CreateLambda( [=](){ return mOptionalWidgetsVisibility.Get() == EVisibility::Visible && CanDetachPlane(); } )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "DetachPlane", "Detach the plane" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.DetachPlane" ) );

    //-

    auto CreateDrawing = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
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
            FCanExecuteAction::CreateLambda( CanCreateDrawing ),
            FIsActionChecked(),
            FIsActionButtonVisible::CreateLambda( [this](){ return mOptionalWidgetsVisibility.Get() == EVisibility::Visible; } )
        ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "create-drawing", "Create a drawing (set the current frame where to create the drawing keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreateDrawing" ) );

    //-

    //LeftToolbarBuilder.AddToolBarButton(
    //    FUIAction(
    //        FExecuteAction::CreateRaw( this, &SCinematicBoardSectionPlaneTitle::ToggleLighttable ),
    //        FCanExecuteAction(),
    //        FGetActionCheckState::CreateRaw( this, &SCinematicBoardSectionPlaneTitle::IsLighttableOn ),
    //        FIsActionButtonVisible::CreateLambda( [this](){ return mOptionalWidgetsVisibility.Get() == EVisibility::Visible; } )
    //    ),
    //    NAME_None,
    //    FText::GetEmpty(),
    //    LOCTEXT( "lighttable-tooltip", "Enables or disables the lighttable" ),
    //    FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.Lighttable" ),
    //    EUserInterfaceActionType::ToggleButton );

    //---

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" ) )
        .BorderBackgroundColor( this, &SCinematicBoardSectionPlaneTitle::GetBackgroundTint )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .FillWidth( .5f )
            [
                LeftToolbarBuilder.MakeWidget()
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
            [
                SNew( SSpacer )
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
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        UMovieSceneSection* section_object = board_section->GetSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
        auto objects = sequencer->FindBoundObjects( mBinding.GetGuid(), result.mInnerSequenceId );

        // To unselect section(s)
        sequencer->EmptySelection();
        // And then select the current one
        sequencer->SelectSection( section_object );

        // To unselect all actors
        GEditor->SelectNone( true, true );
        // And then select the current one
        for( auto object : objects )
            GEditor->SelectActor( Cast<AActor>( object ), true, true );

        return FReply::Handled();
    }

    return SCompoundWidget::OnMouseButtonUp( MyGeometry, MouseEvent );
}

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

    UMovieSceneSequence* sequence = subsection_object->GetSequence();
    UMovieScene* movie_scene = sequence ? sequence->GetMovieScene() : nullptr;
    FMovieScenePossessable* possessable = movie_scene ? movie_scene->FindPossessable( mBinding.GetGuid() ) : nullptr;
    if( !possessable )
        return;

    //---

    const FScopedTransaction transaction( LOCTEXT( "SetTrackPlaneName", "Set Track Plane Name" ) );

    FMovieScenePossessable new_possessable( *possessable );
    new_possessable.SetName( iText.ToString() );
    movie_scene->ReplacePossessable( mBinding.GetGuid(), new_possessable );

    mBinding = *possessable; // Update the cached one

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
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
        return FEditorStyle::GetSlateColor( "SelectionColor_Pressed" );

    return FSlateColor( FLinearColor( FColor( 48, 48, 48, 255 ) ) );
}

//---

//void
//SCinematicBoardSectionPlaneTitle::ToggleLighttable()
//{
//    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
//    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
//    ISequencer*                     sequencer = board_section->GetSequencer().Get();
//
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
//
//    if( IsLighttableOn() == ECheckBoxState::Checked )
//        LighttableTools::Deactivate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
//    else
//        LighttableTools::Activate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
//}
//
//ECheckBoxState
//SCinematicBoardSectionPlaneTitle::IsLighttableOn() const
//{
//    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
//    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
//    ISequencer*                     sequencer = board_section->GetSequencer().Get();
//
//    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );
//
//    //---
//
//    bool lighttable_on = LighttableTools::IsOn( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() ); //TODO: improve to don't call it every ticks ?
//    return lighttable_on ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
//}

//---

class SCinematicBoardSectionPlaneKeys
    : public SCompoundWidget
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
    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TSharedPtr<FMetaFloatChannel> GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const;

    /** Start a transaction at mouse down */
    void BeginTransaction( const FText& iTransactionDesc );
    /** End the transaction at mouse up */
    void EndTransaction();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;

    /** Scoped transaction for this drag operation */
    TUniquePtr<FScopedTransaction>      mTransaction;

    TSharedPtr<FMetaFloatChannel>       mKeysUnderMouse;
};

void
SCinematicBoardSectionPlaneKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
    ];
}

FVector2D
SCinematicBoardSectionPlaneKeys::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

TSharedPtr<FMetaFloatChannel>
SCinematicBoardSectionPlaneKeys::GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const
{
    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    TSharedPtr<FMetaFloatChannel> meta_channel = board_section->GetPlaneTransformMetaChannel( mBinding );
    if( !meta_channel )
        return nullptr;

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
}

void
SCinematicBoardSectionPlaneKeys::BeginTransaction( const FText& iTransactionDesc ) // From FEditToolDragOperation::BeginTransaction#95
{
    // Begin an editor transaction and mark the section as transactional so it's state will be saved
    mTransaction.Reset( new FScopedTransaction( iTransactionDesc ) );

    //---

    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UMovieSceneSection* section = subkey.mSection.Get();
            if( !section )
                continue;

            section->SetFlags( RF_Transactional );
            // Save the current state of the section
            section->TryModify();
        }
    }
}

void
SCinematicBoardSectionPlaneKeys::EndTransaction()
{
    mTransaction.Reset();

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

FCursorReply
SCinematicBoardSectionPlaneKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    TSharedPtr<FMetaFloatChannel> meta_channel = GetKeysUnderMouse( CursorEvent );

    if( meta_channel.IsValid() && meta_channel->NumMetaKeys() )
        return FCursorReply::Cursor( EMouseCursor::CardinalCross );

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    check( !mKeysUnderMouse.IsValid() );

    mKeysUnderMouse = GetKeysUnderMouse( MouseEvent );

    if( !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        mKeysUnderMouse = nullptr;

        return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
    }

    //---

    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        BeginTransaction( LOCTEXT( "MovePlaneKeyTransaction", "Move Plane Keys" ) );

        return FReply::Handled().CaptureMouse( SharedThis( this ) );
    }

    mKeysUnderMouse = nullptr;

    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( HasMouseCapture() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        EndTransaction();

        mKeysUnderMouse = nullptr;

        return FReply::Handled().ReleaseMouseCapture();
    }

    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() || !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
    ISequencer*                     sequencer = board_section->GetSequencer().Get();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment, this should always be the case (until meta keys selection)
    check( mKeysUnderMouse->NumMetaKeys() == 1 );

    const bool snap = sequencer->GetSequencerSettings()->GetIsSnapEnabled() && sequencer->GetSequencerSettings()->GetSnapKeyTimesToInterval();
    const FFrameRate inner_tick_resolution = subsection_object->GetSequence()->GetMovieScene()->GetTickResolution();
    const FFrameRate inner_display_rate = subsection_object->GetSequence()->GetMovieScene()->GetDisplayRate();

    FFrameTime local_inner_time = mKeysUnderMouse->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate );
    FFrameTime local_time = local_inner_time * OuterToInnerTransform.InverseLinearOnly();

    //---

    // Rebuild the full real meta channel
    // This WON'T rebuild the mKeysUnderMouse as it is a copy of the a part of the real meta channel only available during the drag
    board_section->ReBuildPlanesTransformMetaChannel();

    //---

    // Modify all sections where keys have been moved (to force update the viewport)
    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UMovieSceneSection* section = subkey.mSection.Get();
            if( !section )
                continue;

            section->TryModify();
        }
    }

    // Update the current frame in the sequencer
    if( sequencer->GetSequencerSettings()->GetIsSnapEnabled() )
    {
        FFrameRate LocalResolution = sequencer->GetFocusedTickResolution();
        FFrameRate LocalDisplayRate = sequencer->GetFocusedDisplayRate();
        local_time = FFrameRate::TransformTime( FFrameRate::TransformTime( local_time, LocalResolution, LocalDisplayRate ).FloorToFrame(), LocalDisplayRate, LocalResolution );
    }
    sequencer->SetLocalTime( local_time );

    return FReply::Handled();
}

void
SCinematicBoardSectionPlaneKeys::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseEnter" ) );
}

void
SCinematicBoardSectionPlaneKeys::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseLeave" ) );
}

int32
SCinematicBoardSectionPlaneKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

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
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetPlaneTransformChannelProxy();
    TSharedPtr<FMetaFloatChannel> meta_channel = board_section->GetPlaneTransformMetaChannel( mBinding );

    if( !meta_channel.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    for( const auto& pair : meta_channel->GetMetaKeys() )
    {
        FFrameNumber time = pair.Key;
        FMetaKey meta_key = pair.Value;
        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        FFrameTime outer_time = time * inner_to_outer_transform;
        double outer_second = FQualifiedFrameTime( outer_time, movie_scene->GetTickResolution() ).AsSeconds();

        const FVector2D KeySize = SequencerSectionConstants::KeySize;

        static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = converter.SecondsToPixel( outer_second );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );
        const FVector2D KeyTranslationBorder( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f - BrushBorderWidth ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f - BrushBorderWidth ) ) );

        key_draw_param.BorderTint = FLinearColor( 0.05f, 0.05f, 0.05f, 1.0f );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            key_draw_param.BorderBrush,
            ESlateDrawEffect::None,
            key_draw_param.BorderTint
        );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize - 2.0f * BrushBorderWidth, FSlateLayoutTransform( key_draw_param.FillOffset + KeyTranslationBorder ) ),
            key_draw_param.FillBrush,
            ESlateDrawEffect::None,
            key_draw_param.FillTint
        );
    }

    LayerId++;

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

//---

class SCinematicBoardSectionPlaneMaterialKeys
    : public SCompoundWidget
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
    virtual void OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) override;
    virtual void OnMouseLeave( const FPointerEvent& MouseEvent ) override;

    virtual FCursorReply OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const override;

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TSharedPtr<FMetaMaterialChannel> GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const;

    void BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder );

    /** Start a transaction at mouse down */
    void BeginTransaction( const FText& iTransactionDesc );
    /** End the transaction at mouse up */
    void EndTransaction();

private:
    TWeakPtr<FCinematicBoardSection>    mBoardSection;
    FMovieScenePossessable              mBinding;

    /** Scoped transaction for this drag operation */
    TUniquePtr<FScopedTransaction>      mTransaction;

    TSharedPtr<FMetaMaterialChannel>    mKeysUnderMouse;
};

void
SCinematicBoardSectionPlaneMaterialKeys::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
    ];
}

FVector2D
SCinematicBoardSectionPlaneMaterialKeys::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

TSharedPtr<FMetaMaterialChannel>
SCinematicBoardSectionPlaneMaterialKeys::GetKeysUnderMouse( const FPointerEvent& MouseEvent ) const
{
    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    TSharedPtr<FMetaMaterialChannel> meta_channel = board_section->GetPlaneMaterialMetaChannel( mBinding );
    if( !meta_channel )
        return nullptr;

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
}

void
SCinematicBoardSectionPlaneMaterialKeys::BeginTransaction( const FText& iTransactionDesc ) // From FEditToolDragOperation::BeginTransaction#95
{
    // Begin an editor transaction and mark the section as transactional so it's state will be saved
    mTransaction.Reset( new FScopedTransaction( iTransactionDesc ) );

    //---

    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UMovieSceneSection* section = subkey.mSection.Get();
            if( !section )
                continue;

            section->SetFlags( RF_Transactional );
            // Save the current state of the section
            section->TryModify();
        }
    }
}

void
SCinematicBoardSectionPlaneMaterialKeys::EndTransaction()
{
    mTransaction.Reset();

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    sequencer->NotifyMovieSceneDataChanged( EMovieSceneDataChangeType::TrackValueChanged );
}

void
SCinematicBoardSectionPlaneMaterialKeys::BuildKeyContextMenu( FMenuBuilder& ioMenuBuilder )
{
    auto CloneKey = [=]( TSharedPtr<FMetaMaterialChannel> iKeysUnderMouse )
    {
        if( iKeysUnderMouse->NumMetaKeys() != 1 ) // For the moment, only 1 metakey can be cloned
            return;

        auto it = iKeysUnderMouse->GetMetaKeys().CreateConstIterator();
        if( it.Value().mSubKeys.Num() != 1 ) // For the moment, only 1 subkey can be cloned
            return;

        FFrameNumber key_framenumber = it.Key();

        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

        FDrawing drawing = ShotSequenceHelpers::GetDrawing( *sequencer, result.mInnerSequence, result.mInnerSequenceId, key_framenumber, mBinding.GetGuid() );

        UMaterialInstance* material_to_clone = drawing.GetMaterial();

        BoardSequenceTools::CloneDrawing( sequencer, material_to_clone, sequencer->GetLocalTime().Time.FrameNumber, mBinding.GetGuid() );
    };

    auto CanCloneKey = [=]( TSharedPtr<FMetaMaterialChannel> iKeysUnderMouse ) -> bool
    {
        if( iKeysUnderMouse->NumMetaKeys() != 1 ) // For the moment, only 1 metakey can be cloned
            return false;

        auto it = iKeysUnderMouse->GetMetaKeys().CreateConstIterator();
        if( it.Value().mSubKeys.Num() != 1 ) // For the moment, only 1 subkey can be cloned
            return false;

        FFrameNumber key_framenumber = it.Key();

        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, sequencer->GetFocusedMovieSceneSequence(), sequencer->GetFocusedTemplateID(), sequencer->GetLocalTime().Time.FrameNumber );

        FDrawing drawing = ShotSequenceHelpers::GetDrawing( *sequencer, result.mInnerSequence, result.mInnerSequenceId, result.mInnerTime.GetFrame(), mBinding.GetGuid() );

        return !drawing.Exists();
    };

    //-

    auto DeleteKey = [=]( TSharedPtr<FMetaMaterialChannel> iKeysUnderMouse )
    {
        FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
        const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
        ISequencer* sequencer = board_section->GetSequencer().Get();

        const FScopedTransaction transaction( LOCTEXT( "DeleteCameraKeys", "Delete camera keys" ) );

        for( auto pair : iKeysUnderMouse->GetMetaKeys() )
        {
            for( const auto& subkey : pair.Value.mSubKeys )
            {
                BoardSequenceTools::DeleteDrawing( sequencer, *subsection_object, subkey.mSection.Get(), subkey.mChannelHandle, subkey.mKeyHandle );
            }
        }
    };

    auto CanDeleteKey = [=]( TSharedPtr<FMetaMaterialChannel> iKeysUnderMouse ) -> bool
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

    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    UMaterialInstance* material = nullptr;
    UTexture2D* texture = nullptr;
    FString material_name( TEXT( "Multiple" ) );
    FString texture_name( TEXT( "Multiple" ) );

    if( mKeysUnderMouse->NumMetaKeys() == 1 ) // For the moment, only 1 metakey can be cloned
    {
        auto it = mKeysUnderMouse->GetMetaKeys().CreateConstIterator();
        if( it.Value().mSubKeys.Num() == 1 ) // For the moment, only 1 subkey can be cloned
        {
            FFrameNumber key_framenumber = it.Key();

            BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, *subsection_object, sequencer->GetFocusedTemplateID() );

            FDrawing drawing = ShotSequenceHelpers::GetDrawing( *sequencer, result.mInnerSequence, result.mInnerSequenceId, key_framenumber, mBinding.GetGuid() );
            material = drawing.GetMaterial();
            texture = ProjectAssetTools::GetTexture2D( result.mInnerSequence, material );

            material_name = material ? material->GetName() : TEXT( "None" );
            texture_name = texture ? texture->GetName() : TEXT( "None" );
        }
    }

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "texture-key-section-label", "Texture: {0}" ), FText::FromString( texture_name ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-texture-key-label", "Edit..." ),
                                LOCTEXT( "edit-texture-key-tooltip", "Edit the texture of the current key with its default editor\n(If it's not possible, the texture is already opened)" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( EditKey, texture ),
                                           FCanExecuteAction::CreateLambda( CanEditKey, texture ) ) );

    ioMenuBuilder.EndSection();

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "material-key-section-label", "Material: {0}" ), FText::FromString( material_name ) ) );

    ioMenuBuilder.AddMenuEntry( FText::Format( LOCTEXT( "clone-material-key-label", "Clone at {0}" ), FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) ) ),
                                LOCTEXT( "clone-material-key-tooltip", "Clone the current key (material and texture) at the current frame" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( CloneKey, mKeysUnderMouse ),
                                           FCanExecuteAction::CreateLambda( CanCloneKey, mKeysUnderMouse ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-material-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for all the channels -> see camera key delete
                                LOCTEXT( "delete-material-key-tooltip", "Delete the current key" ),
                                FSlateIcon(),
                                FUIAction( FExecuteAction::CreateLambda( DeleteKey, mKeysUnderMouse ),
                                           FCanExecuteAction::CreateLambda( CanDeleteKey, mKeysUnderMouse ) ) );

    ioMenuBuilder.EndSection();
}

FCursorReply
SCinematicBoardSectionPlaneMaterialKeys::OnCursorQuery( const FGeometry& MyGeometry, const FPointerEvent& CursorEvent ) const //override
{
    TSharedPtr<FMetaMaterialChannel> meta_channel = GetKeysUnderMouse( CursorEvent );

    if( meta_channel.IsValid() && meta_channel->NumMetaKeys() )
        return FCursorReply::Cursor( EMouseCursor::CardinalCross );

    return FCursorReply::Cursor( EMouseCursor::Default );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseButtonDown( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    check( !mKeysUnderMouse.IsValid() );

    mKeysUnderMouse = GetKeysUnderMouse( MouseEvent );

    if( !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        mKeysUnderMouse = nullptr;

        return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
    }

    //---

    if( MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        BeginTransaction( LOCTEXT( "MovePlaneMaterialKeyTransaction", "Move Plane Material Keys" ) );

        return FReply::Handled().CaptureMouse( SharedThis( this ) );
    }
    else if( MouseEvent.GetEffectingButton() == EKeys::RightMouseButton )
    {
        FMenuBuilder menu_builder( true, nullptr );
        BuildKeyContextMenu( menu_builder );

        TSharedPtr<SWidget> menu = menu_builder.MakeWidget();
        FWidgetPath WidgetPath = MouseEvent.GetEventPath() != nullptr ? *MouseEvent.GetEventPath() : FWidgetPath();
        FSlateApplication::Get().PushMenu( AsShared(), WidgetPath, menu.ToSharedRef(), MouseEvent.GetScreenSpacePosition(), FPopupTransitionEffect( FPopupTransitionEffect::ContextMenu ) );

        mKeysUnderMouse = nullptr; // doesn't go inside OnMouseButtonUp(), so reset it here

        return FReply::Handled();
    }

    mKeysUnderMouse = nullptr;

    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( HasMouseCapture() && MouseEvent.GetEffectingButton() == EKeys::LeftMouseButton )
    {
        EndTransaction();

        mKeysUnderMouse = nullptr;

        return FReply::Handled().ReleaseMouseCapture();
    }

    return SCompoundWidget::OnMouseButtonDown( MyGeometry, MouseEvent );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() || !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();
    ISequencer*                     sequencer = board_section->GetSequencer().Get();

    FGeometry geometry;
    FTimeToPixel converter = board_section->ConstructConverterForViewRange( &geometry );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = subsection_object->OuterToInnerTransform();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment, this should always be the case (until meta keys selection)
    check( mKeysUnderMouse->NumMetaKeys() == 1 );

    const bool snap = sequencer->GetSequencerSettings()->GetIsSnapEnabled() && sequencer->GetSequencerSettings()->GetSnapKeyTimesToInterval();
    const FFrameRate inner_tick_resolution = subsection_object->GetSequence()->GetMovieScene()->GetTickResolution();
    const FFrameRate inner_display_rate = subsection_object->GetSequence()->GetMovieScene()->GetDisplayRate();

    FFrameTime local_inner_time = mKeysUnderMouse->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate );
    FFrameTime local_time = local_inner_time * OuterToInnerTransform.InverseLinearOnly();

    //---

    // Rebuild the full real meta channel
    // This WON'T rebuild the mKeysUnderMouse as it is a copy of the a part of the real meta channel only available during the drag
    board_section->ReBuildPlanesMaterialMetaChannel();

    //---

    // Modify all sections where keys have been moved (to force update the viewport)
    for( auto pair : mKeysUnderMouse->GetMetaKeys() )
    {
        for( const auto& subkey : pair.Value.mSubKeys )
        {
            UMovieSceneSection* section = subkey.mSection.Get();
            if( !section )
                continue;

            section->TryModify();
        }
    }

    // Update the current frame in the sequencer
    if( sequencer->GetSequencerSettings()->GetIsSnapEnabled() )
    {
        FFrameRate LocalResolution = sequencer->GetFocusedTickResolution();
        FFrameRate LocalDisplayRate = sequencer->GetFocusedDisplayRate();
        local_time = FFrameRate::TransformTime( FFrameRate::TransformTime( local_time, LocalResolution, LocalDisplayRate ).FloorToFrame(), LocalDisplayRate, LocalResolution );
    }
    sequencer->SetLocalTime( local_time );

    return FReply::Handled();
}

void
SCinematicBoardSectionPlaneMaterialKeys::OnMouseEnter( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseEnter" ) );
}

void
SCinematicBoardSectionPlaneMaterialKeys::OnMouseLeave( const FPointerEvent& MouseEvent ) //override
{
    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseLeave" ) );
}

int32
SCinematicBoardSectionPlaneMaterialKeys::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    if( !mBinding.GetGuid().IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

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
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FCinematicBoardSection*         board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection*    subsection_object = &board_section->GetSubSectionObject();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetPlaneMaterialChannelProxy();
    TSharedPtr<FMetaMaterialChannel> meta_channel = board_section->GetPlaneMaterialMetaChannel( mBinding );

    if( !meta_channel.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = board_section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = subsection_object->OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = subsection_object->GetTypedOuter<UMovieScene>();
    check( movie_scene );

    for( const auto& pair : meta_channel->GetMetaKeys() )
    {
        FFrameNumber time = pair.Key;
        FMetaKey meta_key = pair.Value;
        FKeyDrawParams key_draw_param = meta_key.mMetaKeyDrawParam;

        FFrameTime outer_time = time * inner_to_outer_transform;
        double outer_second = FQualifiedFrameTime( outer_time, movie_scene->GetTickResolution() ).AsSeconds();

        const FVector2D KeySize = SequencerSectionConstants::KeySize;

        static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = converter.SecondsToPixel( outer_second );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );
        const FVector2D KeyTranslationBorder( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f - BrushBorderWidth ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f - BrushBorderWidth ) ) );

        key_draw_param.BorderTint = FLinearColor( 0.05f, 0.05f, 0.05f, 1.0f );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            key_draw_param.BorderBrush,
            ESlateDrawEffect::None,
            key_draw_param.BorderTint
        );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize - 2.0f * BrushBorderWidth, FSlateLayoutTransform( key_draw_param.FillOffset + KeyTranslationBorder ) ),
            key_draw_param.FillBrush,
            ESlateDrawEffect::None,
            key_draw_param.FillTint
        );
    }

    LayerId++;

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
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
            SNew( SCinematicBoardSectionPlaneMaterialKeys, iBoardSection )
            .Binding( mBinding )
        ]
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneKeys, iBoardSection )
            .Binding( mBinding )
        ]
    ];
}

void
SCinematicBoardSectionPlane::BuildContextMenu( FMenuBuilder& ioMenuBuilder )
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection* subsection_object = &board_section->GetSubSectionObject();
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FText plane_name = FText::FromString( mBinding.GetName() );
    FText current_frame = FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) );

    //---

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "plane-section-label", "Plane: {0}" ), plane_name ) );

    auto DetachPlane = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        BoardSequenceTools::DetachPlane( sequencer, subsection_object, mBinding.GetGuid() );
    };

    auto CanDetachPlane = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        return BoardSequenceTools::CanDetachPlane( sequencer, subsection_object, mBinding.GetGuid() );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "detach-plane-label", "Detach {0}" ), plane_name ),
        LOCTEXT( "detach-plane-tooltip", "Detach the plane" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.DetachPlane" ),
        FUIAction(
            FExecuteAction::CreateLambda( DetachPlane ),
            FCanExecuteAction::CreateLambda( CanDetachPlane )
        ) );

    //-

    auto CreateDrawing = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::CreateDrawing( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    auto CanCreateDrawing = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateDrawing( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "create-drawing-label", "Create a drawing at {0}" ), current_frame ),
        LOCTEXT( "create-drawing-tooltip", "Create a drawing (set the current frame where to create the drawing keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreateDrawing" ),
        FUIAction(
            FExecuteAction::CreateLambda( CreateDrawing ),
            FCanExecuteAction::CreateLambda( CanCreateDrawing )
        ) );

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

    auto CreatePlane = [this]()
    {
        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        BoardSequenceTools::CreatePlane( sequencer, section_object->GetInclusiveStartFrame() );
    };

    auto CanCreatePlane = [this]() -> bool
    {
        ISequencer* sequencer = mSequencer.Pin().Get();
        UMovieSceneSection* section_object = mBoardSection.Pin()->GetSectionObject();
        return BoardSequenceTools::CanCreatePlane( sequencer, section_object->GetInclusiveStartFrame() );
    };

    FToolBarBuilder MiddleToolbarBuilder( nullptr, FMultiBoxCustomization::None );
    MiddleToolbarBuilder.SetLabelVisibility( EVisibility::Collapsed );
    MiddleToolbarBuilder.SetStyle( &*FEposTracksEditorStyle::Get(), "EposSection.ToolBar" );

    MiddleToolbarBuilder.AddToolBarButton(
        FUIAction(
            FExecuteAction::CreateLambda( CreatePlane ),
            FCanExecuteAction::CreateLambda( CanCreatePlane ),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( CanCreatePlane ) ),
        NAME_None,
        FText::GetEmpty(),
        LOCTEXT( "CreatePlane", "Create a new plane" ),
        FSlateIcon( FEditorStyle::GetStyleSetName(), "Plus" ) );
        //FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreatePlane" ) );
    MiddleToolbarBuilder.AddComboButton(
        FUIAction(
            FExecuteAction(),
            FCanExecuteAction(),
            FGetActionCheckState(),
            FIsActionButtonVisible::CreateLambda( CanCreatePlane ) ),
        FOnGetContent::CreateRaw( this, &SCinematicBoardSectionPlanes::MakeTextureMenu ),
        LOCTEXT( "TextureOptions", "Options" ),
        LOCTEXT( "TextureOptionsToolTip", "Texture Options" ),
        TAttribute<FSlateIcon>(),
        true );

    TSharedRef< SWidget > middle_widget = MiddleToolbarBuilder.MakeWidget();
    // To always keep the real space of the toolbar as hidden keeps space
    // Otherwise the verticalbox is (a little) smaller when the toolbar is collapsed
    middle_widget->SetVisibility( MakeAttributeLambda( [this]() { return mOptionalWidgetsVisibility.Get() == EVisibility::Visible ? EVisibility::Visible : EVisibility::Hidden; } ) );

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
            middle_widget
        ]
    ];

    RebuildPlaneList();
}

TSharedRef<SWidget>
SCinematicBoardSectionPlanes::MakeTextureMenu()
{
    FMenuBuilder MenuBuilder( true, mSequencer.Pin()->GetCommandBindings() );

    EposTracksToolbarHelpers::MakeTextureSettingsEntries( MenuBuilder );

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

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *mSequencer.Pin().Get(), subsection, mSequencer.Pin()->GetFocusedTemplateID() );

    TArray<FGuid> bindings;
    int plane_count = ShotSequenceHelpers::GetAllPlanes( *mSequencer.Pin().Get(), result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAll, nullptr, &bindings );

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
    if( !need_rebuild( bindings ) ) //TOCHECK: check if it's really ok
        return;

    mPossessables.Empty();

    UMovieSceneSequence* inner_sequence = subsection.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    if( !inner_moviescene )
        return;

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
        FMovieScenePossessable possessable = *inner_moviescene->FindPossessable( bindings[i] );

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
