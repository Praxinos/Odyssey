#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
#include "Undo/OdysseyVectorUndoTagInbetweenerParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    , Color( FOdysseyVectorTagInbetweener::DEFAULT_RED_UINT8
           , FOdysseyVectorTagInbetweener::DEFAULT_GREEN_UINT8
           , FOdysseyVectorTagInbetweener::DEFAULT_BLUE_UINT8
           , FOdysseyVectorTagInbetweener::DEFAULT_ALPHA_UINT8 )
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                                                            , FOdysseyVectorGroupPaint* iScene )
{
    std::list<FOdysseyVectorTagInbetweener*> emptyList;

    Update( iEditor, iScene, emptyList );
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
        Color = selectedInbetweenerTag->GetColor();
        MapAsPolyline = selectedInbetweenerTag->GetMapAsPolyline();

        DivisionX = selectedInbetweenerTag->GetGridNumQuadX();
        DivisionY = selectedInbetweenerTag->GetGridNumQuadY();

        //Rigidity = selectedInbetweenerTag->GetARAPRigidity();
    }
}

void 
UOdysseyPainterEditorVectorTagInbetweenerView::Update( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorGroupPaint* iScene
                                                     , const std::list<FOdysseyVectorTagInbetweener*>& iSelectedInbetweenerTagList )
{
    mEditor = iEditor;
    mScene = iScene;

    mSelectedInbetweenerTagArray.clear();
    mSelectedInbetweenerTagArray.reserve( iSelectedInbetweenerTagList.size() );

    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : iSelectedInbetweenerTagList )
    {
        mSelectedInbetweenerTagArray.push_back( selectedInbetweenerTag );
    }

    ImportParam();
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PropertyChanged( const FName& iPropertyName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iCategory)
{
    for( FOdysseyVectorTagInbetweener* selectedInbetweenerTag : mSelectedInbetweenerTagArray )
    {
        //////////
/*        if( iPropertyName == "InbetweenCount" )
            selectedInbetweenerTag->SetDrawingCount( DrawingCount );
*/
        if( iPropertyName == "InterpolationType" )
            selectedInbetweenerTag->SetInterpolationType( InterpolationType );

        if( iPropertyName == "DivisionX" )
            selectedInbetweenerTag->SetGridNumQuad( DivisionX, selectedInbetweenerTag->GetGridNumQuadY() );

        if( iPropertyName == "DivisionY" )
            selectedInbetweenerTag->SetGridNumQuad( selectedInbetweenerTag->GetGridNumQuadX(), DivisionY );
/*
        if( iPropertyName == "Rigidity" )
        {
            selectedInbetweenerTag->SetARAPRigidity( Rigidity );
        }
*/
        if( iPropertyName == "MapAsPolyline" )
            selectedInbetweenerTag->SetMapAsPolyline( MapAsPolyline );

        if( iPropertyName == "Color" )
            selectedInbetweenerTag->SetColor( Color );

        // must be last to be able to update correctly grid type-dependent fields
        if( iPropertyName == "GridType" )
        {
            selectedInbetweenerTag->SetGrid( GridType, DivisionX, DivisionY );

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
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_TIMELINE;

/*
    if( iPropertyName == "InbetweenCount" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT;
*/
    if( iPropertyName == "InterpolationType" )
        return new FOdysseyVectorUndoTagInbetweenerInterpolationType( mScene, mSelectedInbetweenerTagArray, notificationFlags );

    if( iPropertyName == "DivisionX" )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene, mSelectedInbetweenerTagArray, notificationFlags );

    if( iPropertyName == "DivisionY" )
        return new FOdysseyVectorUndoTagInbetweenerGridSize( mScene, mSelectedInbetweenerTagArray, notificationFlags );
/*
    if( iPropertyName == "Rigidity" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY;
*/
    if( iPropertyName == "GridType" )
        return new FOdysseyVectorUndoTagInbetweenerGridType( mScene, mSelectedInbetweenerTagArray, notificationFlags );

    if( iPropertyName == "Color" )
        return new FOdysseyVectorUndoTagInbetweenerColor( mScene, mSelectedInbetweenerTagArray, notificationFlags );

    if( iPropertyName == "MapAsPolyline" )
        return new FOdysseyVectorUndoTagInbetweenerMapAsPolyline( mScene, mSelectedInbetweenerTagArray, notificationFlags );


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

        mScene->Update( 0 );
        // redraw
        mScene->GetEngine()->Invalidate( 0 );
    }
}

#undef LOCTEXT_NAMESPACE
