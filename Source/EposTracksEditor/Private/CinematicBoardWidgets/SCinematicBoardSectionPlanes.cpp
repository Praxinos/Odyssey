// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "CinematicBoardWidgets/SCinematicBoardSectionPlanes.h"

#include "Brushes/SlateColorBrush.h"

#include "CinematicBoardTrack/CinematicBoardSection.h"

#define LOCTEXT_NAMESPACE "SCinematicBoardSectionPlanes"

//---

void
SCinematicBoardSectionPlane::Construct( const FArguments& InArgs, TSharedRef<FCinematicBoardSection> iBoardSection )
{
    mBoardSection = iBoardSection;

    mBinding = InArgs._Binding;

    ChildSlot
    [
        SNew( SBox )
        [
            SNew( STextBlock )
            .Text( FText::FromString( mBinding.GetName() ) )
        ]
    ];
}

FVector2D
SCinematicBoardSectionPlane::ComputeDesiredSize( float ) const //override
{
    FVector2D size = GetDesiredSize();
    size.Y = SequencerSectionConstants::DefaultSectionHeight + 5.f;

    return size;
}

int32
SCinematicBoardSectionPlane::OnPaint( const FPaintArgs& Args, const FGeometry& AllottedGeometry, const FSlateRect& MyCullingRect, FSlateWindowElementList& OutDrawElements, int32 LayerId, const FWidgetStyle& InWidgetStyle, bool bParentEnabled ) const //override
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

    //---

    return SCompoundWidget::OnPaint( Args, AllottedGeometry, MyCullingRect, OutDrawElements, LayerId, InWidgetStyle, bParentEnabled );
}

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
