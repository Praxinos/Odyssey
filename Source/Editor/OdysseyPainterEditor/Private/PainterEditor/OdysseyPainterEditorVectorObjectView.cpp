#include "OdysseyPainterEditorVectorObjectView.h"
#include "Undo/OdysseyVectorUndoPropertyChanged.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectView"

UOdysseyPainterEditorVectorObjectView::~UOdysseyPainterEditorVectorObjectView()
{
}

UOdysseyPainterEditorVectorObjectView::UOdysseyPainterEditorVectorObjectView()
    : mObject( nullptr )
{
}

void
UOdysseyPainterEditorVectorObjectView::ImportParam( FOdysseyVectorObject* iObject )
{
     ObjectParam = iObject->mObjectParam;
}

void
UOdysseyPainterEditorVectorObjectView::ExportParam( FOdysseyVectorObject* iObject )
{
    iObject->mObjectParam = ObjectParam;
}

void 
UOdysseyPainterEditorVectorObjectView::Update( FOdysseyVectorObject* iObject )
{
    mObject = iObject;

    ImportParam( mObject );
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    if( iCategory == "Transform" )
    {
        mObject->UpdateMatrix();
    }
}

void
UOdysseyPainterEditorVectorObjectView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mObject )
    {
        FOdysseyVectorScene* scene = mObject->GetScene();

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("PropertyChanged","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPropertyChanged( scene, mObject );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        ExportParam( mObject );

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        // call delegates
        //scene->mRefreshLayer.Broadcast(scene);

        scene->Update( 0 );
        scene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    }
}
