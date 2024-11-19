// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorSharedEnv.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"
#define WARNING_INTERP_ROUTE_REMOVAL "Trajectories are only valid with ARAP interpolation. Existing trajectories will be removed. Proceed ?"
#define WARNING_GRIDSIZE_ROUTE_REMOVAL "Changing grid size will remove existing trajectories. Proceed ?"
#define WARNING_SQUARE_ROUTE_REMOVAL "Changing grid shape will remove existing trajectories. Proceed ?"

UOdysseyPainterEditorVectorTagInbetweenerView::~UOdysseyPainterEditorVectorTagInbetweenerView()
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView()
    : mEditor( nullptr )
    , mScene( nullptr )
    , InterpolationType ( eInbetweenerInterpolationType::ARAP )
    , GridType ( eInbetweenerGridType::ARAP )
    , DivisionX ( 8 )
    , DivisionY ( 8 )
    //, Rigidity ( 10 )
    , MapAsPolyline( true )
    , WithThickness( true )
    , ConstantWidth( false )
    , Square( true )
    , InbetweenColor( FOdysseyVectorTagInbetweener::INBETWEEN_DEFAULT_RED_UINT8
                    , FOdysseyVectorTagInbetweener::INBETWEEN_DEFAULT_GREEN_UINT8
                    , FOdysseyVectorTagInbetweener::INBETWEEN_DEFAULT_BLUE_UINT8
                    , FOdysseyVectorTagInbetweener::INBETWEEN_DEFAULT_ALPHA_UINT8 )
    , ChartColor( FOdysseyVectorTagInbetweener::CHART_DEFAULT_RED_UINT8
                , FOdysseyVectorTagInbetweener::CHART_DEFAULT_GREEN_UINT8
                , FOdysseyVectorTagInbetweener::CHART_DEFAULT_BLUE_UINT8
                , FOdysseyVectorTagInbetweener::CHART_DEFAULT_ALPHA_UINT8 )
    , GridColor( FOdysseyVectorTagInbetweener::GRID_DEFAULT_RED_UINT8
               , FOdysseyVectorTagInbetweener::GRID_DEFAULT_GREEN_UINT8
               , FOdysseyVectorTagInbetweener::GRID_DEFAULT_BLUE_UINT8
               , FOdysseyVectorTagInbetweener::GRID_DEFAULT_ALPHA_UINT8 )
    , TrajectoryColor( FOdysseyVectorTagInbetweener::TRAJECTORY_DEFAULT_RED_UINT8
                     , FOdysseyVectorTagInbetweener::TRAJECTORY_DEFAULT_GREEN_UINT8
                     , FOdysseyVectorTagInbetweener::TRAJECTORY_DEFAULT_BLUE_UINT8
                     , FOdysseyVectorTagInbetweener::TRAJECTORY_DEFAULT_ALPHA_UINT8 )
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                                                            , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTagInbetweener*> emptyList;

    Update( iEditor, iScene );
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::ImportParam()
{
    if( mSelectedInbetweenerTagArray.size() )
    {
        FOdysseyVectorTagInbetweener* selectedInbetweenerTag = mSelectedInbetweenerTagArray[0];

        InterpolationType = selectedInbetweenerTag->GetInterpolationType();
        GridType = selectedInbetweenerTag->GetGridType();
        //DrawingCount = selectedInbetweenerTag->GetDrawingCount();
        InbetweenColor = selectedInbetweenerTag->GetInbetweenColor();
        ChartColor = selectedInbetweenerTag->GetChartColor();
        GridColor = selectedInbetweenerTag->GetGridColor();
        TrajectoryColor = selectedInbetweenerTag->GetTrajectoryColor();
        MapAsPolyline = selectedInbetweenerTag->GetMapAsPolyline();
        WithThickness = selectedInbetweenerTag->GetWithThickness();
        ConstantWidth = selectedInbetweenerTag->HasConstantWidth();
        Square = selectedInbetweenerTag->IsSquare();

        DivisionX = selectedInbetweenerTag->GetGridNumQuadX();
        DivisionY = selectedInbetweenerTag->GetGridNumQuadY();

        //Rigidity = selectedInbetweenerTag->GetARAPRigidity();
    }
}

bool
UOdysseyPainterEditorVectorTagInbetweenerView::Update( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorGroupPaint* iScene )
{
    mEditor = iEditor;
    mScene = iScene;

    mSelectedInbetweenerTagArray.clear();
    // note: it may reserve more than needed.
    mSelectedInbetweenerTagArray.reserve( iScene->GetSharedEnv()->GetSharedTagList().size() );

    for( FOdysseyVectorTag* tag : iScene->GetSharedEnv()->GetSharedTagList() )
    {
        if( tag->GetOwner()->IsSelected() )
        {
            if( tag->GetClass() == FOdysseyVectorTagInbetweener::StaticClass() )
            {
                FOdysseyVectorTagInbetweener* inbetweenerTag = static_cast<FOdysseyVectorTagInbetweener*>(tag);

                mSelectedInbetweenerTagArray.push_back( inbetweenerTag );
            }
        }
    }

    ImportParam();

    return ( mSelectedInbetweenerTagArray.size() > 0 );
}

bool
UOdysseyPainterEditorVectorTagInbetweenerView::SelectionHasRoutes()
{
    for( FOdysseyVectorTagInbetweener* inbetweenerTag : mSelectedInbetweenerTagArray )
    {
        if( inbetweenerTag->GetRouteList().size() )
        {
            return true;
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PropertyChanged( const FName& iPropertyName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iCategory)
{
    if( SelectionHasRoutes() )
    {
        if( iPropertyName == "InterpolationType" )
        {
            if( InterpolationType == eInbetweenerInterpolationType::Linear )
            {
                FText dialogText = FText::FromString( TEXT ( WARNING_INTERP_ROUTE_REMOVAL ) );

                if( FMessageDialog::Open( EAppMsgType::OkCancel, dialogText ) == EAppReturnType::Cancel )
                {
                    // restore displayed values
                    ImportParam();

                    return;
                }
            }
        }

        if( ( iPropertyName == "DivisionX" )
          ||( iPropertyName == "DivisionY" ) )
        {
            FText dialogText = FText::FromString( TEXT ( WARNING_GRIDSIZE_ROUTE_REMOVAL ) );

            if( FMessageDialog::Open( EAppMsgType::OkCancel, dialogText ) == EAppReturnType::Cancel )
            {
                // restore displayed values
                ImportParam();

                return;
            }
        }

        if( iPropertyName == "Square" )
        {
            FText dialogText = FText::FromString( TEXT ( WARNING_SQUARE_ROUTE_REMOVAL ) );

            if( FMessageDialog::Open( EAppMsgType::OkCancel, dialogText ) == EAppReturnType::Cancel )
            {
                // restore displayed values
                ImportParam();

                return;
            }
        }
    }

    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : mSelectedInbetweenerTagArray )
    {
        if( iPropertyName == "InterpolationType" )
            selectedInbetweenerTag->SetInterpolationType( InterpolationType );

        if( iPropertyName == "DivisionX" )
            selectedInbetweenerTag->SetGridNumQuad( DivisionX, selectedInbetweenerTag->GetGridNumQuadY(), Square );

        if( iPropertyName == "DivisionY" )
            selectedInbetweenerTag->SetGridNumQuad( selectedInbetweenerTag->GetGridNumQuadX(), DivisionY, Square );

        if( iPropertyName == "MapAsPolyline" )
        {
            selectedInbetweenerTag->SetMapAsPolyline( MapAsPolyline );

            // regularize the groid at least once after remapping or else some grid points that were not moved before
            // will stay at there position
            if( selectedInbetweenerTag->GetGridType() == eInbetweenerGridType::ARAP )
            {
                for( FInbetweenerBreakdown* breakdown : selectedInbetweenerTag->GetBreakdownList() )
                {
                    FInbetweenerGridARAP* arapGrid = static_cast<FInbetweenerGridARAP*>(breakdown->GetGrid());

                    arapGrid->Regularize( 1 );
                }
            }
        }

        if( iPropertyName == "WithThickness" )
        {
            selectedInbetweenerTag->SetWithThickness( WithThickness );
        }

        if( iPropertyName == "ConstantWidth" )
        {
            selectedInbetweenerTag->SetConstantWidth( ConstantWidth );
        }

        if( iPropertyName == "Square" )
        {
            selectedInbetweenerTag->SetGrid( GridType, DivisionX, DivisionY, Square );
        }

        if( iPropertyName == "InbetweenColor" )
            selectedInbetweenerTag->SetInbetweenColor( InbetweenColor );

        if( iPropertyName == "ChartColor" )
            selectedInbetweenerTag->SetChartColor( ChartColor );

        if( iPropertyName == "GridColor" )
            selectedInbetweenerTag->SetGridColor( GridColor );

        if( iPropertyName == "TrajectoryColor" )
            selectedInbetweenerTag->SetTrajectoryColor( TrajectoryColor );

        // must be last to be able to update correctly grid type-dependent fields
        if( iPropertyName == "GridType" )
        {
            selectedInbetweenerTag->SetGrid( GridType, DivisionX, DivisionY, Square );

            // updates grid type-dependent fields
            ImportParam();
        }
    }
}


FOdysseyVectorUndo*
UOdysseyPainterEditorVectorTagInbetweenerView::MakeUndo( const FName& iPropertyName
                                                       , const FName& iMemberPropertyName
                                                       , const FName& iCategory )
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE
                             | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS;

    if( iPropertyName == "InterpolationType" )
        return new FOdysseyVectorUndoTagInbetweenerInterpolationType( mScene
                                                                    , mSelectedInbetweenerTagArray
                                                                    , notificationFlags );

    if( iPropertyName == "DivisionX" )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( iPropertyName == "DivisionY" )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( iPropertyName == "GridType" )
        return new FOdysseyVectorUndoTagInbetweenerGridType( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( ( iPropertyName == "Color"      )
      ||( iPropertyName == "ChartColor" )
      ||( iPropertyName == "GridColor"  )
      ||( iPropertyName == "TrajectoryColor"  ) )
        return new FOdysseyVectorUndoTagInbetweenerColor( mScene
                                                        , mSelectedInbetweenerTagArray
                                                        , notificationFlags );

    if( iPropertyName == "MapAsPolyline" )
        return new FOdysseyVectorUndoTagInbetweenerMapAsPolyline( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == "WithThickness" )
        return new FOdysseyVectorUndoTagInbetweenerWithThickness( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == "ConstantWidth" )
        return new FOdysseyVectorUndoTagInbetweenerConstantWidth( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == "Square" )
        return new FOdysseyVectorUndoTagInbetweenerSquare( mScene
                                                         , mSelectedInbetweenerTagArray
                                                         , notificationFlags );

    return nullptr;
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mSelectedInbetweenerTagArray.size() )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-tag.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = MakeUndo( PropertyChangedEvent.GetPropertyName()
                                               , PropertyChangedEvent.MemberProperty->GetFName()
                                               , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );

            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        mScene->GetSharedEnv()->Update( 0 );
        // redraw
        mScene->GetEngine()->Invalidate( 0 );
    }
}

#undef LOCTEXT_NAMESPACE
