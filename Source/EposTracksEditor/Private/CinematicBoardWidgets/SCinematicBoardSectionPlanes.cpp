// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

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
};

void
SCinematicBoardSectionPlaneTitle::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    static const FSlateBrush* background_brush = FEditorStyle::GetBrush( "ToolPanel.GroupBorder" );

    ChildSlot
    [
        SNew( SBorder )
        .BorderImage( mBinding.GetGuid().IsValid() ? background_brush : nullptr )
        .HAlign( EHorizontalAlignment::HAlign_Center )
        [
            SNew( STextBlock )
            .Text( mBinding.GetGuid().IsValid() ? FText::FromString( mBinding.GetName() ) : FText::GetEmpty() )
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

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;
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

static
FTimeToPixel
ConstructTimeConverterForSection3( const FGeometry& InSectionGeometry, const UMovieSceneSection& InSection )
{
    FFrameRate     TickResolution = InSection.GetTypedOuter<UMovieScene>()->GetTickResolution();
    double         LowerTime = InSection.GetInclusiveStartFrame() / TickResolution;
    double         UpperTime = InSection.GetExclusiveEndFrame() / TickResolution;

    return FTimeToPixel( InSectionGeometry, TRange<double>( LowerTime, UpperTime ), TickResolution );
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

    TArray<double> keys = section->GetPlaneTransformKeys( mBinding );

    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( auto key : keys )
    {
        const FVector2D KeySize = SequencerSectionConstants::KeySize;
        //static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection3( AllottedGeometry, section->GetSubSectionObject() ).SecondsToPixel( key );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            CircleKeyBrush
        );
    }

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

protected:
    // SWidget overrides.
    virtual FVector2D ComputeDesiredSize( float ) const override;

private:
    TWeakPtr<FCinematicBoardSection> mBoardSection;

    FMovieScenePossessable mBinding;
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

    TArray<double> keys = section->GetPlaneMaterialKeys( mBinding );

    static const FName CircleKeyBrushName( "Sequencer.KeyCircle" );
    static const FName DiamondKeyBrushName( "Sequencer.KeyDiamond" );
    static const FName SquareKeyBrushName( "Sequencer.KeySquare" );
    static const FName TriangleKeyBrushName( "Sequencer.KeyTriangle" );

    const FSlateBrush* CircleKeyBrush = FEditorStyle::GetBrush( CircleKeyBrushName );
    const FSlateBrush* DiamondKeyBrush = FEditorStyle::GetBrush( DiamondKeyBrushName );
    const FSlateBrush* SquareKeyBrush = FEditorStyle::GetBrush( SquareKeyBrushName );
    const FSlateBrush* TriangleKeyBrush = FEditorStyle::GetBrush( TriangleKeyBrushName );

    FVector2D localSectionSize = AllottedGeometry.GetLocalSize();

    for( auto key : keys )
    {
        const FVector2D KeySize = SequencerSectionConstants::KeySize;
        //static const float BrushBorderWidth = 2.0f;
        const float KeyPositionPx = ConstructTimeConverterForSection3( AllottedGeometry, section->GetSubSectionObject() ).SecondsToPixel( key );
        const FVector2D KeyTranslation( KeyPositionPx - FMath::CeilToFloat( KeySize.X / 2.0f ), ( ( AllottedGeometry.GetLocalSize().Y / 2.0f ) - ( KeySize.Y / 2.0f ) ) );

        FSlateDrawElement::MakeBox(
            OutDrawElements,
            LayerId,
            AllottedGeometry.ToPaintGeometry( KeySize, FSlateLayoutTransform( KeyTranslation ) ),
            DiamondKeyBrush
        );
    }

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
        [
            SNew( SCinematicBoardSectionPlaneTitle, iBoardSection )
            .Binding( mBinding )
        ]
        + SVerticalBox::Slot()
        [
            SNew( SCinematicBoardSectionPlaneKeys, iBoardSection )
            .Binding( mBinding )
        ]
        + SVerticalBox::Slot()
        [
            SNew( SCinematicBoardSectionPlaneMaterialKeys, iBoardSection )
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
        //.OnMouseButtonClick( this, &SOdysseyAboutScreen::OnListViewButtonClicked )
        .SelectionMode( ESelectionMode::None )
    ];

    mNeedRebuildPlaneList = true;
}

TSharedRef<ITableRow>
SCinematicBoardSectionPlanes::MakePlaneRow( TSharedRef<FMovieScenePossessable> iItem, const TSharedRef<STableViewBase>& iOwnerTable )
{
    return
        SNew( STableRow< TSharedPtr<FString> >, iOwnerTable )
        [
            mBoardSection.IsValid()
            ?
            SNew( SCinematicBoardSectionPlane, mBoardSection.Pin().ToSharedRef() )
            .Binding( *iItem )
            :
            SNullWidget::NullWidget
        ];
}

void
SCinematicBoardSectionPlanes::Tick( const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime ) //override
{
    SCompoundWidget::Tick( AllottedGeometry, InCurrentTime, InDeltaTime );

    //---

    if( mNeedRebuildPlaneList && mBoardSection.IsValid() )
    {
        TArray<FMovieScenePossessable> possessables( mBoardSection.Pin()->GetPlaneBindings() );

        mPossessables.Empty();

        // This doesn't work because this vertical box won't have the same size for all sections
        // and as the height of a track node is getting from the first section in the array (and not necessary the one at the first position in the gui)
        // if the first section has no (or less) planes than others, all planes in the vertical box won't be displayed
        //int max_planes = possessables.Num();

        int max_planes = mBoardSection.Pin()->GetMaxPlaneBindings();
        for( int i = 0; i < max_planes; i++ )
        {
            mPossessables.Add( MakeShared<FMovieScenePossessable>( possessables.IsValidIndex( i ) ? possessables[i] : FMovieScenePossessable() ) );
        }

        if( mWidgetPlaneList )
            mWidgetPlaneList->RebuildList();

        mNeedRebuildPlaneList = false;
    }
}

#undef LOCTEXT_NAMESPACE
