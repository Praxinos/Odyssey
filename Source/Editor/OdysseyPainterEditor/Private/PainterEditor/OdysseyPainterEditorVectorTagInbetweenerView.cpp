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

        Rigidity = selectedInbetweenerTag->GetARAPRigidity();
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

        if( iPropertyName == "Rigidity" )
        {
            selectedInbetweenerTag->SetARAPRigidity( Rigidity );
        }

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

// static
uint64
UOdysseyPainterEditorVectorTagInbetweenerView::GetSnapshotFlags( const FName& iPropertyName
                                                               , const FName& iMemberPropertyName
                                                               , const FName& iCategory )
{ 
    uint64 snapshotFlags = 0;
/*
    if( iPropertyName == "InbetweenCount" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::INBETWEENCOUNT;
*/
    if( iPropertyName == "InterpolationType" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::INTERPOLATIONTYPE;

    if( iPropertyName == "DivisionX" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::GRIDSIZE;

    if( iPropertyName == "DivisionY" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::GRIDSIZE;

    if( iPropertyName == "Rigidity" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::ARAPRIGIDITY;

    if( iPropertyName == "GridType" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::GRIDTYPE;

    if( iPropertyName == "Color" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::COLOR;

    if( iPropertyName == "MapAsPolyline" )
        snapshotFlags |= FSnapshotFlags::Tag::Inbetweener::MAPASPOLYLINE;


    return snapshotFlags;
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mSelectedInbetweenerTagArray.size() )
    {
        uint64 snapshotFlags = GetSnapshotFlags( PropertyChangedEvent.GetPropertyName()
                                               , PropertyChangedEvent.MemberProperty->GetFName()
                                               , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-tag.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoTagInbetweenerParam( mScene
                                                                                , mSelectedInbetweenerTagArray
                                                                                , snapshotFlags );

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
        // calls delegates
        mScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE
