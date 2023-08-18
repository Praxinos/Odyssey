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
    std::list<FOdysseyVectorObject*>::iterator it;

    for( it = mFocusedObjectList.begin(); it != mFocusedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* focusedObject = (*it);

        ObjectParam = focusedObject->mObjectParam;

        ForegroundColor = focusedObject->GetForegroundBucket().GetSolidColor();
        BackgroundColor = focusedObject->GetBackgroundBucket().GetSolidColor();

        break; // only one
    }
}

void 
UOdysseyPainterEditorVectorObjectView::Update( FOdysseyVectorScene* iScene
                                             , std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    mScene = iScene;
    mFocusedObjectList = iFocusedObjectList;

    ImportParam();
}

uint64
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    std::list<FOdysseyVectorObject*>::iterator it;
    uint64 signalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    for( it = mFocusedObjectList.begin(); it != mFocusedObjectList.end(); ++it )
    {
        FOdysseyVectorObject* selectedObject = (*it);

        // We have to change properties one by one especially in case of multiple selection.
        // We just cannot copy the whole block of properties.

        if( iPropertyName == "Name" )
        {
            selectedObject->mObjectParam.Name = ObjectParam.Name;

            signalFlags |= FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY;
        }

        if( iPropertyName == "TranslationX" )
            selectedObject->mObjectParam.TranslationX = ObjectParam.TranslationX;

        if( iPropertyName == "TranslationY" )
            selectedObject->mObjectParam.TranslationY = ObjectParam.TranslationY;

        //if( iPropertyName == "TranslationZ" )
        //    selectedObject->mObjectParam.TranslationZ = ObjectParam.TranslationZ;

        if( iPropertyName == "Rotation" )
            selectedObject->mObjectParam.Rotation = ObjectParam.Rotation;

        if( iPropertyName == "ScalingX" )
            selectedObject->mObjectParam.ScalingX = ObjectParam.ScalingX;

        if( iPropertyName == "ScalingY" )
            selectedObject->mObjectParam.ScalingY = ObjectParam.ScalingY;

        if( iPropertyName == "ForegroundColor" )
            selectedObject->GetForegroundBucket().SetSolidColor( ForegroundColor );

        if( iPropertyName == "BackgroundColor" )
            selectedObject->GetBackgroundBucket().SetSolidColor( BackgroundColor );

        if( iCategory == "Transform" )
            selectedObject->UpdateMatrix();
    }

    return signalFlags;
}

void
UOdysseyPainterEditorVectorObjectView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    if( mScene )
    {
        uint64 signalFlags;

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("PropertyChanged","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoPropertyChanged( mScene, mFocusedObjectList );
            // We use GEditor as the UObject, otherwise if we use "this", at each UNDO, PostEditChangeProperty() will be called
            // which will again call StoreUndo + this will lead to a crash. I don't know however what will be the consequences
            // of a call to GEditor::PostEditChangeProperty()
            GUndo->StoreUndo( GEditor, TUniquePtr<FOdysseyVectorUndo>(undo) );
        }
        GEditor->EndTransaction();

        signalFlags = PropertyChanged( PropertyChangedEvent.GetPropertyName()
                                     , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );

        mScene->Update( FOdysseyVectorObject::UPDATEPAINTGROUPS );

        mScene->GetEngine()->Signal( signalFlags );
    }
}

#undef LOCTEXT_NAMESPACE
