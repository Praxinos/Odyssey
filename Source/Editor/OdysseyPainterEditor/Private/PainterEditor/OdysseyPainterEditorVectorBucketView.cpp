#include "OdysseyPainterEditorVectorBucketView.h"
#include "Undo/OdysseyVectorUndoBucketParam.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorBucketView::~UOdysseyPainterEditorVectorBucketView()
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView()
    : mBucket( nullptr )
{
}

UOdysseyPainterEditorVectorBucketView::UOdysseyPainterEditorVectorBucketView( FOdysseyVectorBucket* iBucket )
{
    Update( iBucket );
}

void
UOdysseyPainterEditorVectorBucketView::ImportParam()
{
    BucketParam = mBucket->mBucketParam;
}

void 
UOdysseyPainterEditorVectorBucketView::Update( FOdysseyVectorBucket* iBucket )
{
    mBucket = iBucket;

    ImportParam();
}

void
UOdysseyPainterEditorVectorBucketView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    if( mBucket )
    {
        mBucket->mBucketParam = BucketParam;

        mBucket->Invalidate();
    }
}

void
UOdysseyPainterEditorVectorBucketView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mBucket )
    {
        FOdysseyVectorGroupPaint* vectorScene = mBucket->GetOwner()->GetScene();

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-bucket.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoBucketParam( vectorScene, mBucket );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        vectorScene->Update( 0 );
        // calls delegates
        vectorScene->GetEngine()->Signal( FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW );
    }
}

#undef LOCTEXT_NAMESPACE
