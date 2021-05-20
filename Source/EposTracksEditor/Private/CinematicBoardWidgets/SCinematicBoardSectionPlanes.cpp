// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"
#include "Channels/MovieSceneChannelProxy.h"
#include "Channels/MovieSceneObjectPathChannel.h"
#include "Materials/MaterialInstanceConstant.h"
#include "Sections/MovieScenePrimitiveMaterialSection.h"

#include "Board/BoardSequenceHelpers.h"
#include "CinematicBoardTrack/CinematicBoardSection.h"
#include "CinematicBoardTrack/MetaChannelProxy.h"
#include "LighttableTools.h"
#include "Shot/ShotSequenceHelpers.h"
#include "Styles/EposTracksEditorStyle.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionPlanes"

//---

class SCinematicBoardSectionPlaneTitle
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneTitle )
        : _Binding()
        {}
        SLATE_ARGUMENT( FMovieScenePossessable, Binding )
    SLATE_END_ARGS()

    // Construct the widget
    void Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection );

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;

    bool mLighttableState;
};

void
SCinematicBoardSectionPlaneTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;
    check( mBinding.GetGuid().IsValid() );

    mLighttableState = false; //PATCH

    static const FSlateBrush* background_brush = FEditorStyle::GetBrush( "ToolPanel.GroupBorder" );
    static const FSlateBrush* lighttable_on_brush = FEposTracksEditorStyle::Get()->GetBrush( "EposTracksEditor.LighttableOn" );
    static const FSlateBrush* lighttable_off_brush = FEposTracksEditorStyle::Get()->GetBrush( "EposTracksEditor.LighttableOff" );

    //---

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( background_brush )
        [
            SNew( SHorizontalBox )
            + SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew( SButton )
                .ButtonStyle( FEditorStyle::Get(), "NoBorder" )
                .Cursor( EMouseCursor::Default )
                .OnClicked_Lambda( [&]()
                    {
                        TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
                        UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

                        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );
                        if( mLighttableState )
                            LighttableTools::Deactivate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );
                        else
                            LighttableTools::Activate( *sequencer, result.mInnerSequence, result.mInnerSequenceId, mBinding.GetGuid() );

                        mLighttableState = !mLighttableState;

                        return FReply::Handled();
                    } )
                .ToolTipText_Lambda( [&]() { return mLighttableState ? LOCTEXT( "lighttable.on", "Lighttable On" ) : LOCTEXT( "lighttable.off", "Lighttable Off" ); } )
                [
                    SNew( SImage )
                    .Image_Lambda( [&]() { return mLighttableState ? lighttable_on_brush : lighttable_off_brush; } )
                ]
            ]
            + SHorizontalBox::Slot()
            .HAlign( HAlign_Center )
            .VAlign( VAlign_Center )
            [
                SNew( STextBlock )
                .Text( FText::FromString( mBinding.GetName() ) )
            ]
        ]
    ];
}

//---

class SCinematicBoardSectionPlaneKeys
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS( SCinematicBoardSectionPlaneKeys )
        : _Binding()
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

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;

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
    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();

    FGeometry geometry;
    FTimeToPixel converter = section->ConstructConverterForViewRange( &geometry );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    TSharedPtr<FMetaFloatChannel> meta_channel = section->GetPlaneTransformMetaChannel( mBinding );
    if( !meta_channel )
        return nullptr;

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
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

    //---

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonDown" ) );
    return FReply::Handled().CaptureMouse( SharedThis( this ) );
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mKeysUnderMouse = nullptr;

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonUp" ) );
    return FReply::Handled().ReleaseMouseCapture();
}

FReply
SCinematicBoardSectionPlaneKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() || !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry;
    FTimeToPixel converter = section->ConstructConverterForViewRange( &geometry );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment should always be the case
    check( mKeysUnderMouse->NumMetaKeys() == 1 );

    const bool snap = section->GetSequencer()->GetSequencerSettings()->GetIsSnapEnabled() && section->GetSequencer()->GetSequencerSettings()->GetSnapKeyTimesToInterval();
    const FFrameRate inner_tick_resolution = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetTickResolution();
    const FFrameRate inner_display_rate = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetDisplayRate();

    mKeysUnderMouse->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate );

    section->ReBuildPlanesTransformMetaChannel();

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseMove" ) );
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

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetPlaneTransformChannelProxy();
    TSharedPtr<FMetaFloatChannel> meta_channel = section->GetPlaneTransformMetaChannel( mBinding );

    if( !meta_channel.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = section->GetSubSectionObject().OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = section->GetSubSectionObject().GetTypedOuter<UMovieScene>();
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
        : _Binding()
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

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;

    TSharedPtr<FMetaMaterialChannel>       mKeysUnderMouse;
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
    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();

    FGeometry geometry;
    FTimeToPixel converter = section->ConstructConverterForViewRange( &geometry );
    FFrameTime clicked_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    const FFrameTime HalfKeySizeFrames = converter.PixelDeltaToFrame( SequencerSectionConstants::KeySize.X * .5f );
    TRange<FFrameNumber> inner_range_tolerance( ( ( clicked_frame - HalfKeySizeFrames ) * OuterToInnerTransform ).FloorToFrame(), ( ( clicked_frame + HalfKeySizeFrames ) * OuterToInnerTransform ).CeilToFrame() );
    FFrameNumber inner_tolerance = inner_range_tolerance.Size<FFrameNumber>() / 2;

    //---

    FFrameTime inner_clicked_frame = clicked_frame * OuterToInnerTransform;

    //---

    TSharedPtr<FMetaMaterialChannel> meta_channel = section->GetPlaneMaterialMetaChannel( mBinding );
    if( !meta_channel )
        return nullptr;

    return meta_channel->CreateFromTime( inner_clicked_frame, inner_tolerance );
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

    //---

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonDown" ) );
    return FReply::Handled().CaptureMouse( SharedThis( this ) );
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseButtonUp( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    mKeysUnderMouse = nullptr;

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseButtonUp" ) );
    return FReply::Handled().ReleaseMouseCapture();
}

FReply
SCinematicBoardSectionPlaneMaterialKeys::OnMouseMove( const FGeometry& MyGeometry, const FPointerEvent& MouseEvent ) //override
{
    if( !HasMouseCapture() || !mKeysUnderMouse.IsValid() || !mKeysUnderMouse->NumMetaKeys() )
    {
        //return FReply::Handled();
        return SCompoundWidget::OnMouseMove( MyGeometry, MouseEvent );
    }

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    FGeometry geometry;
    FTimeToPixel converter = section->ConstructConverterForViewRange( &geometry );
    FFrameTime moved_frame = converter.PixelToFrame( geometry.AbsoluteToLocal( MouseEvent.GetScreenSpacePosition() ).X );

    //---

    const FMovieSceneSequenceTransform OuterToInnerTransform = section->GetSubSectionObject().OuterToInnerTransform();
    FFrameTime inner_moved_frame = moved_frame * OuterToInnerTransform;

    // For the moment should always be the case
    check( mKeysUnderMouse->NumMetaKeys() == 1 );

    const bool snap = section->GetSequencer()->GetSequencerSettings()->GetIsSnapEnabled() && section->GetSequencer()->GetSequencerSettings()->GetSnapKeyTimesToInterval();
    const FFrameRate inner_tick_resolution = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetTickResolution();
    const FFrameRate inner_display_rate = section->GetSubSectionObject().GetSequence()->GetMovieScene()->GetDisplayRate();

    mKeysUnderMouse->Move( inner_moved_frame, snap, inner_tick_resolution, inner_display_rate );

    section->ReBuildPlanesMaterialMetaChannel();

    //UE_LOG( LogTemp, Warning, TEXT( "OnMouseMove" ) );
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

    TSharedPtr<FCinematicBoardSection> section = mBoardSection.Pin();

    //TSharedPtr<FMovieSceneChannelProxy> channel_proxy = section->GetPlaneMaterialChannelProxy();
    TSharedPtr<FMetaMaterialChannel> meta_channel = section->GetPlaneMaterialMetaChannel( mBinding );

    if( !meta_channel.IsValid() )
        return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );

    //---

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();
    FTimeToPixel converter = section->ConstructConverterForSection( AllottedGeometry );
    const FMovieSceneSequenceTransform inner_to_outer_transform = section->GetSubSectionObject().OuterToInnerTransform().InverseLinearOnly();
    const UMovieScene* movie_scene = section->GetSubSectionObject().GetTypedOuter<UMovieScene>();
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

void
SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .AutoHeight()
        [
            SNew( SCinematicBoardSectionPlaneTitle, iBoardSection )
            .Binding( mBinding )
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

//---
//---
//---

SCinematicBoardSectionPlanes::SCinematicBoardSectionPlanes()
    : mNeedRebuildPlaneList( true )
{
}

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

    mRebuildPlaneListHandle = mSequencer.Pin()->OnMovieSceneDataChanged().AddSP( this, &SCinematicBoardSectionPlanes::RebuildPlaneList );

    //---

    check( !mPossessables.Num() );

    //---

    ChildSlot
    [
        SAssignNew( mWidgetPlaneList, SListView<TSharedRef<FMovieScenePossessable>> )
        .ListItemsSource( &mPossessables )
        .OnGenerateRow( this, &SCinematicBoardSectionPlanes::MakePlaneRow )
        .SelectionMode( ESelectionMode::None )
    ];

    RebuildPlaneList();
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
        ];
}

static
int
GetMaxPlaneCount( IMovieScenePlayer& iPlayer, const UMovieSceneTrack* iTrack, FMovieSceneSequenceIDRef iSequenceID )
{
    int count = 0;
    for( auto section : iTrack->GetAllSections() )
    {
        UMovieSceneSubSection* subsection = Cast<UMovieSceneSubSection>( section );

        BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( iPlayer, *subsection, iSequenceID );
        TArray<AStaticMeshActor*> planes;
        TArray<FGuid> guids;
        int plane_count = ShotSequenceHelpers::GetAllPlanes( iPlayer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAlwaysAll, &planes, &guids );

        count = FMath::Max( count, plane_count );
    }

    return count;
}

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

    TSharedPtr<ISequencer> sequencer = mBoardSection.Pin()->GetSequencer();
    UMovieSceneSubSection& subsection = mBoardSection.Pin()->GetSubSectionObject();

    BoardSequenceHelpers::FInnerSequenceResult result = BoardSequenceHelpers::GetInnerSequence( *sequencer, subsection, sequencer->GetFocusedTemplateID() );

    TArray<AStaticMeshActor*> planes;
    TArray<FGuid> bindings;
    int plane_count = ShotSequenceHelpers::GetAllPlanes( *sequencer, result.mInnerSequence, result.mInnerSequenceId, EGetPlane::kAlwaysAll, &planes, &bindings );

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
