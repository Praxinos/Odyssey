#include "OdysseyPainterEditorVectorObjectView.h"
#include "Undo/OdysseyVectorUndoPropertyChanged.h"

#define LOCTEXT_NAMESPACE "UOdysseyPainterEditorVectorObjectView"

UOdysseyPainterEditorVectorObjectView::~UOdysseyPainterEditorVectorObjectView()
{
}

UOdysseyPainterEditorVectorObjectView::UOdysseyPainterEditorVectorObjectView()
    : mScene( nullptr )
{
}

void
UOdysseyPainterEditorVectorObjectView::ImportParam()
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mScene->GetSelectedObjectList();

    for ( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        ObjectParam = selectedObject->mObjectParam;

        break; // only one
    }
}

void 
UOdysseyPainterEditorVectorObjectView::Update( FOdysseyVectorScene* iScene )
{
    mScene = iScene;

    ImportParam();
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    std::list<FOdysseyVectorObject*>& selectedObjectList = mScene->GetSelectedObjectList();

    for ( std::list<FOdysseyVectorObject*>::iterator it = selectedObjectList.begin(); it != selectedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        // We have to change properties one by one especially in case of multiple selection.
        // We just cannot copy the whole block of properties.

        if( iPropertyName == "TranslationX" )
            selectedObject->mObjectParam.TranslationX = ObjectParam.TranslationX;

        if( iPropertyName == "TranslationY" )
            selectedObject->mObjectParam.TranslationY = ObjectParam.TranslationY;

        if( iPropertyName == "Rotation" )
            selectedObject->mObjectParam.Rotation = ObjectParam.Rotation;

        if( iPropertyName == "ScalingX" )
            selectedObject->mObjectParam.ScalingX = ObjectParam.ScalingX;

        if( iPropertyName == "ScalingY" )
            selectedObject->mObjectParam.ScalingY = ObjectParam.ScalingY;

        if( iPropertyName == "Foreground" )
            selectedObject->mObjectParam.Foreground = ObjectParam.Foreground;

        if( iCategory == "Transform" )
            selectedObject->UpdateMatrix();
    }
}

void
UOdysseyPainterEditorVectorObjectView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mScene )
    {
        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("PropertyChanged","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPropertyChanged( mScene, mScene->GetSelectedObjectList() );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        PropertyChanged( PropertyChangedEvent.GetPropertyName()
                       , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        // call delegates
        //scene->mRefreshLayer.Broadcast(scene);

        mScene->Update( 0 );
        mScene->Signal( FOdysseyVectorScene::SIGNAL_SCENE_REDRAW );
    }
}
