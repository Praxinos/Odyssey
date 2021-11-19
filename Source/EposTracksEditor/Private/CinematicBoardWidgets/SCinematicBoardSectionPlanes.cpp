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
#include "CinematicBoardWidgets/SMetaKeysArea.h"
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

    // SWidget overrides
    virtual int32 OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const override;

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
        SNew( SVerticalBox )

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            // Background color is done inside the paint function
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

        + SVerticalBox::Slot()
        .AutoHeight()
        [
            // To display the key indications
            SNew( SBox )
            .HeightOverride( 8 )
            //.Visibility( EVisibility::Collapsed )
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

int32
SCinematicBoardSectionPlaneTitle::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
{
    const FSlateBrush* background_brush = FEditorStyle::GetBrush( "Sequencer.AnimationOutliner.TopLevelBorder_Expanded" );

    // Gray title background
    FSlateDrawElement::MakeBox(
        OutDrawElements,
        LayerId++,
        AllottedGeometry.ToPaintGeometry( AllottedGeometry.GetLocalSize(), FSlateLayoutTransform() ),
        background_brush,
        ESlateDrawEffect::None,
        background_brush->GetTint( InWidgetStyle ) * InWidgetStyle.GetColorAndOpacityTint() * GetBackgroundTint().GetColor( InWidgetStyle ) // Same as in SBorder
    );

    if( !mBoardSection.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

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
        static const FVector2D KeyMarkSize = FVector2D( 3.f, 5.f );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry(
                FVector2D(
                    key_position - FMath::CeilToFloat( KeyMarkSize.X / 2.f ),
                    FMath::CeilToFloat( AllottedGeometry.GetLocalSize().Y - KeyMarkSize.Y - 1.f )
                ),
                KeyMarkSize
            ),
            FEditorStyle::GetBrush( "Sequencer.KeyMark" ),
            ESlateDrawEffect::None,
            FLinearColor( 1.f, 1.f, 1.f, 1.f )
        );
    }

    LayerId++;

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
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
        ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "texture-key-section-label", "Texture: {0}" ), FText::FromString( textures[0]->GetName() ) ) );

        ioMenuBuilder.AddMenuEntry( LOCTEXT( "edit-texture-key-label", "Edit..." ),
                                    LOCTEXT( "edit-texture-key-tooltip", "Edit the texture of the current key with its default editor\n(If it's not possible, the texture is already opened)" ),
                                    FSlateIcon(),
                                    FUIAction( FExecuteAction::CreateLambda( EditKey, textures[0] ),
                                               FCanExecuteAction::CreateLambda( CanEditKey, textures[0] ) ) );

        ioMenuBuilder.EndSection();
    }

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "material-key-section-label", "Material: {0}" ), FText::FromString( material_name ) ) );

    ioMenuBuilder.AddMenuEntry( FText::Format( LOCTEXT( "clone-material-key-label", "Clone at {0}" ), FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) ) ),
                                LOCTEXT( "clone-material-key-tooltip", "Clone the current key (material and texture) at the current frame" ),
                                FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Duplicate" ),
                                FUIAction( FExecuteAction::CreateLambda( CloneKey, iKeys ),
                                           FCanExecuteAction::CreateLambda( CanCloneKey, iKeys ) ) );

    ioMenuBuilder.AddMenuEntry( LOCTEXT( "delete-material-key-label", "Delete" ), //TODO: find a way to know the number of "symbolic" keys deleted, 1 symbolic key should represent a key at the same time for all the channels -> see camera key delete
                                LOCTEXT( "delete-material-key-tooltip", "Delete the current key" ),
                                FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
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
                                FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
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
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneOpacityKeys, iBoardSection )
            .Binding( mBinding )
        ]
    ];
}

void
SCinematicBoardSectionPlane::BuildContextMenu( FMenuBuilder& ioMenuBuilder )
{
    FCinematicBoardSection* board_section = mBoardSection.Pin().Get();
    const UMovieSceneSubSection& subsection_object = board_section->GetSubSectionObject();
    UMovieSceneSequence* inner_sequence = subsection_object.GetSequence();
    UMovieScene* inner_moviescene = inner_sequence ? inner_sequence->GetMovieScene() : nullptr;
    ISequencer* sequencer = board_section->GetSequencer().Get();

    FText plane_track_text = inner_moviescene ? inner_moviescene->GetObjectDisplayName( mBinding.GetGuid() ) : FText::GetEmpty();
    FText current_frame_text = FText::FromString( sequencer->GetNumericTypeInterface()->ToString( sequencer->GetLocalTime().Time.AsDecimal() ) );

    //---

    ioMenuBuilder.BeginSection( NAME_None, FText::Format( LOCTEXT( "plane-section-label", "Plane: {0}" ), plane_track_text ) );

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
        FText::Format( LOCTEXT( "detach-plane-label", "Detach {0}" ), plane_track_text ),
        LOCTEXT( "detach-plane-tooltip", "Detach the plane" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.DetachPlane" ),
        FUIAction(
            FExecuteAction::CreateLambda( DetachPlane ),
            FCanExecuteAction::CreateLambda( CanDetachPlane )
        ) );

    ioMenuBuilder.AddSeparator();

    auto DeletePlane = [this]()
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::DeletePlane( sequencer, subsection_object, mBinding.GetGuid() );
    };

    ioMenuBuilder.AddMenuEntry(
        FText::Format( LOCTEXT( "delete-plane-label", "Delete {0}" ), plane_track_text ),
        LOCTEXT( "delete-plane-tooltip", "Delete the plane and its corresponding actor" ),
        FSlateIcon( FCoreStyle::Get().GetStyleSetName(), "GenericCommands.Delete" ),
        FUIAction( FExecuteAction::CreateLambda( DeletePlane ) ) );

    ioMenuBuilder.EndSection();

    //-

    ioMenuBuilder.BeginSection( NAME_None, LOCTEXT( "drawing-section-label", "Drawing" ) );

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
        FText::Format( LOCTEXT( "create-drawing-label", "Create a drawing at {0}" ), current_frame_text ),
        LOCTEXT( "create-drawing-tooltip", "Create a drawing\n(set the current frame where to create the drawing keyframe)" ),
        FSlateIcon( FEposTracksEditorStyle::Get()->GetStyleSetName(), "EposTracksEditor.CreateDrawing" ),
        FUIAction(
            FExecuteAction::CreateLambda( CreateDrawing ),
            FCanExecuteAction::CreateLambda( CanCreateDrawing )
        ) );

    //-

    auto CreateOpacity = [this]( float iOpacity )
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        BoardSequenceTools::CreateOpacity( sequencer, subsection_object, local_frame, mBinding.GetGuid(), iOpacity );
    };

    auto CanCreateOpacity = [this]() -> bool
    {
        ISequencer* sequencer = mBoardSection.Pin()->GetSequencer().Get();
        const UMovieSceneSubSection& subsection_object = mBoardSection.Pin()->GetSubSectionObject();
        FFrameNumber local_frame = sequencer->GetLocalTime().Time.FrameNumber;
        return BoardSequenceTools::CanCreateOpacity( sequencer, subsection_object, local_frame, mBinding.GetGuid() );
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
        FText::Format( LOCTEXT( "create-drawing-opacity-label", "Create Opacity at {0}" ), current_frame_text ),
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
