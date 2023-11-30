#include "OdysseyPainterEditorVectorObjectView.h"
#include "Undo/OdysseyVectorUndoPropertyChanged.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

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
    for( FOdysseyVectorObject* focusedObject : mFocusedObjectList )
    {
        // Category "Identity"
        Name = focusedObject->GetName();

        // Category "Transform"
        TranslationX = focusedObject->GetTranslationX();
        TranslationY = focusedObject->GetTranslationY();
        Rotation     = focusedObject->GetRotation();
        ScalingX     = focusedObject->GetScalingX();
        ScalingY     = focusedObject->GetScalingY();

        // Category "Appearance"
        Opacity         = focusedObject->GetOpacity();
        ForegroundColor = focusedObject->GetForegroundBucket().GetSolidColor();
        BackgroundColor = focusedObject->GetBackgroundBucket().GetSolidColor();

        break; // only one
    }
}

void 
UOdysseyPainterEditorVectorObjectView::Update( FOdysseyVectorGroupPaint* iScene
                                             , std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    mScene = iScene;
    mFocusedObjectList = iFocusedObjectList;

    ImportParam();
}

uint64
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName, const FName& iCategory )
{
    uint64 signalFlags = FOdysseyVectorEngine::SIGNAL_SCENE_REDRAW;

    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        // We have to change properties one by one especially in case of multiple selection.
        // We just cannot copy the whole block of properties.

        // Category "Identity"
        if( iPropertyName == "Name" )
        {
            selectedObject->SetName( Name );

            signalFlags |= FOdysseyVectorEngine::SIGNAL_SCENE_HIERARCHY;
        }

        // Category "Transform"
        if( iPropertyName == "TranslationX" )
            selectedObject->Translate( TranslationX, selectedObject->GetTranslationY() );

        if( iPropertyName == "TranslationY" )
            selectedObject->Translate( selectedObject->GetTranslationY(), TranslationY );

        if( iPropertyName == "Rotation" )
            selectedObject->Rotate( Rotation );

        if( iPropertyName == "ScalingX" )
            selectedObject->Scale( ScalingX, selectedObject->GetScalingY() );

        if( iPropertyName == "ScalingY" )
            selectedObject->Scale( selectedObject->GetScalingY(), ScalingY );

        if( iCategory == "Transform" )
            selectedObject->UpdateMatrix();

        // Category "Appearance"
        if( iPropertyName == "Opacity" )
            selectedObject->SetOpacity( Opacity );

        if( iPropertyName == "ForegroundColor" )
            selectedObject->GetForegroundBucket().SetSolidColor( ForegroundColor );

        if( iPropertyName == "BackgroundColor" )
            selectedObject->GetBackgroundBucket().SetSolidColor( BackgroundColor );

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
        GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
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
