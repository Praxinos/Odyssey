#include "OdysseyPainterEditorVectorTagInbetweenerView.h"
//#include "Undo/OdysseyVectorUndoBucketParam.h"
#include "OdysseyVectorEngine.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorTagInbetweenerView::~UOdysseyPainterEditorVectorTagInbetweenerView()
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView()
    : mEditor( nullptr )
    , mInbetweenerTag( nullptr )
{
}

UOdysseyPainterEditorVectorTagInbetweenerView::UOdysseyPainterEditorVectorTagInbetweenerView( FOdysseyPainterEditor* iEditor
                                                                                            , FOdysseyVectorTagInbetweener* iBucket )
{
    Update( iEditor, iBucket );
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::ImportParam()
{
    GridType = mInbetweenerTag->GetGridType();
    DivisionX = mInbetweenerTag->GetFFDNumCellX();
    DivisionY  = mInbetweenerTag->GetFFDNumCellY();
    InbetweenCount = mInbetweenerTag->GetInbetweenCount();
}

void 
UOdysseyPainterEditorVectorTagInbetweenerView::Update( FOdysseyPainterEditor* iEditor
                                                     , FOdysseyVectorTagInbetweener* iInbetweenerTag )
{
    mEditor = iEditor;
    mInbetweenerTag = iInbetweenerTag;

    ImportParam();
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PropertyChanged( const FName& iPropertyName
                                                              , const FName& iMemberPropertyName
                                                              , const FName& iCategory)
{
    if( mInbetweenerTag )
    {
        if( iPropertyName == "InbetweenCount" )
            mInbetweenerTag->SetInbetweenCount( InbetweenCount );

        if( iPropertyName == "GridType" )
            mInbetweenerTag->SetGridType( GridType );

        if( iPropertyName == "DivisionX" )
            mInbetweenerTag->SetFFDNumCellX( DivisionX );

        if( iPropertyName == "DivisionY" )
            mInbetweenerTag->SetFFDNumCellY( DivisionY );
    }
}

void
UOdysseyPainterEditorVectorTagInbetweenerView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mInbetweenerTag )
    {
        FOdysseyVectorGroupPaint* vectorScene = mInbetweenerTag->GetOwner()->GetScene();

/*
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-bucket.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoBucketParam( vectorScene, mBucket );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
                
            TSharedPtr<FOdysseyPainterEditorSource> source = mEditor->GetSource();
            if (source)
                source->RecordCurrentFrameUndo();
        }
        GEditor->EndTransaction();
*/

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , PropertyChangedEvent.MemberProperty->GetFName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        vectorScene->Update( 0 );
        // calls delegates
        vectorScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE
