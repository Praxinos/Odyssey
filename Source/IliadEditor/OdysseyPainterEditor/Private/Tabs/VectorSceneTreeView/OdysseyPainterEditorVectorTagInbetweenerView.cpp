// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"
#include "Misc/MessageDialog.h"

#define LOCTEXT_NAMESPACE "PainterEditor"
#define WARNING_INTERP_ROUTE_REMOVAL "Trajectories are only valid with ARAP interpolation. Existing trajectories will be removed. Proceed ?"
#define WARNING_GRIDSIZE_ROUTE_REMOVAL "Changing grid size will remove existing trajectories. Proceed ?"
#define WARNING_GRID_DEFORMATION_RESET "Changing the grid's size will reset its deformation. Proceed ?"
#define WARNING_SQUARE_ROUTE_REMOVAL "Changing grid shape will remove existing trajectories. Proceed ?"

UOdysseyPainterEditorVectorTagInbetweenerView::~UOdysseyPainterEditorVectorTagInbetweenerView()
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView()
    : mEditor( nullptr )
    , mScene( nullptr )
    , InterpolationType ( eInbetweenerInterpolationType::ARAP )
    , GridType ( eInbetweenerGridType::ARAP )
    , DivisionX ( 24 )
    , DivisionY ( 24 )
    , Divisions ( 24 )
    //, Rigidity ( 10 )
    , MappingMode( eInbetweenerMappingMode::Polyline )
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
        MappingMode = selectedInbetweenerTag->GetMapAsPolyline() ? eInbetweenerMappingMode::Polyline
                                                                 : eInbetweenerMappingMode::KeepTopology ;
        WithThickness = selectedInbetweenerTag->GetWithThickness();
        ConstantWidth = selectedInbetweenerTag->HasConstantWidth();
        Square = selectedInbetweenerTag->IsSquare();

        DivisionX = selectedInbetweenerTag->GetGridNumQuadX();
        DivisionY = selectedInbetweenerTag->GetGridNumQuadY();
        Divisions = selectedInbetweenerTag->GetGridNumQuadX();

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
    mSelectedInbetweenerTagArray.reserve( iScene->GetLayer()->GetSharedTagList().size() );

    for( FOdysseyVectorTag* tag : iScene->GetLayer()->GetSharedTagList() )
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
    static bool warningGridDeformationResetShown = false;

    if( SelectionHasRoutes() )
    {
        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, InterpolationType ) )
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

        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionX ) )
          ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionY ) )
          ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Divisions ) ) )
        {
            FText dialogText = FText::FromString( TEXT ( WARNING_GRIDSIZE_ROUTE_REMOVAL ) );

            if( FMessageDialog::Open( EAppMsgType::OkCancel, dialogText ) == EAppReturnType::Cancel )
            {
                // restore displayed values
                ImportParam();

                return;
            }
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Square ) )
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

    // popup will display only once
    if( warningGridDeformationResetShown == false )
    {
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionX ) )
          ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionY ) )
          ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Divisions ) )
          ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Square    ) ) )
        {
            FText dialogText = FText::FromString( TEXT ( WARNING_GRID_DEFORMATION_RESET ) );

            if( FMessageDialog::Open( EAppMsgType::OkCancel, dialogText ) == EAppReturnType::Cancel )
            {
                // restore displayed values
                ImportParam();

                return;
            }
        }

        warningGridDeformationResetShown = true;
    }

    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : mSelectedInbetweenerTagArray )
    {
        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, InterpolationType ) )
        {
            selectedInbetweenerTag->SetInterpolationType( InterpolationType );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionX ) )
        {
            selectedInbetweenerTag->SetGridNumQuad( DivisionX, selectedInbetweenerTag->GetGridNumQuadY(), Square );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionY ) )
        {
            selectedInbetweenerTag->SetGridNumQuad( selectedInbetweenerTag->GetGridNumQuadX(), DivisionY, Square );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Divisions ) )
        {
            selectedInbetweenerTag->SetGridNumQuad( Divisions, Divisions, Square );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, MappingMode ) )
        {
            selectedInbetweenerTag->SetMapAsPolyline( ( MappingMode == eInbetweenerMappingMode::Polyline ) ? true : false );

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

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, WithThickness ) )
        {
            selectedInbetweenerTag->SetWithThickness( WithThickness );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, ConstantWidth ) )
        {
            selectedInbetweenerTag->SetConstantWidth( ConstantWidth );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Square ) )
        {
            if( Square == true )
            {
                Divisions = DivisionX;
                DivisionY = DivisionX;
            }
            else
            {
                DivisionX = Divisions;
                DivisionY = Divisions;
            }

            selectedInbetweenerTag->SetGrid( GridType, DivisionX, DivisionY, Square );
        }

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, InbetweenColor ) )
            selectedInbetweenerTag->SetInbetweenColor( InbetweenColor );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, ChartColor ) )
            selectedInbetweenerTag->SetChartColor( ChartColor );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, GridColor ) )
            selectedInbetweenerTag->SetGridColor( GridColor );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, TrajectoryColor ) )
            selectedInbetweenerTag->SetTrajectoryColor( TrajectoryColor );

        // must be last to be able to update correctly grid type-dependent fields
        if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, GridType ) )
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

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, InterpolationType ) )
        return new FOdysseyVectorUndoTagInbetweenerInterpolationType( mScene
                                                                    , mSelectedInbetweenerTagArray
                                                                    , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionX ) )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, DivisionY ) )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, GridType ) )
        return new FOdysseyVectorUndoTagInbetweenerGridType( mScene
                                                           , mSelectedInbetweenerTagArray
                                                           , notificationFlags );

    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, InbetweenColor  ) )
      ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, ChartColor      ) )
      ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, GridColor       ) )
      ||( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, TrajectoryColor ) ) )
        return new FOdysseyVectorUndoTagInbetweenerColor( mScene
                                                        , mSelectedInbetweenerTagArray
                                                        , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, MappingMode ) )
        return new FOdysseyVectorUndoTagInbetweenerMapAsPolyline( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, WithThickness ) )
        return new FOdysseyVectorUndoTagInbetweenerWithThickness( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, ConstantWidth ) )
        return new FOdysseyVectorUndoTagInbetweenerConstantWidth( mScene
                                                                , mSelectedInbetweenerTagArray
                                                                , notificationFlags );

    if( iPropertyName == GET_MEMBER_NAME_CHECKED( UOdysseyPainterEditorVectorTagInbetweenerView, Square ) )
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

        // redraw
        mScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mScene->GetLayer()->RequestRedraw( mScene->GetCell(), 0 );
    }
}

#undef LOCTEXT_NAMESPACE
