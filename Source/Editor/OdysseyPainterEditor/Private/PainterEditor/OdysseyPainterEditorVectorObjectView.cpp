#include "OdysseyPainterEditorVectorObjectView.h"
//#include "Undo/OdysseyVectorUndoPropertyChanged.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorObjectView::~UOdysseyPainterEditorVectorObjectView()
{
}

UOdysseyPainterEditorVectorObjectView::UOdysseyPainterEditorVectorObjectView()
    : mEditor( nullptr )
    , mScene( nullptr )
    , Name( "VectorObject" )
    , TranslationX ( 0.0f )
    , TranslationY ( 0.0f )
    , Rotation ( 0.0f )
    , ScalingX ( 1.0f )
    , ScalingY ( 1.0f )
    , Visible ( true )
    , ForegroundColorMode ( eForegroundColorMode::SolidColor )
    , ForegroundColor ( FOdysseyVectorObject::FOREGROUNDCOLOR_DEFAULT_R
                      , FOdysseyVectorObject::FOREGROUNDCOLOR_DEFAULT_G
                      , FOdysseyVectorObject::FOREGROUNDCOLOR_DEFAULT_B
                      , FOdysseyVectorObject::FOREGROUNDCOLOR_DEFAULT_A )
    , BackgroundColorMode ( eBackgroundColorMode::SolidColor )
    , BackgroundColor ( FOdysseyVectorObject::BACKGROUNDCOLOR_DEFAULT_R
                      , FOdysseyVectorObject::BACKGROUNDCOLOR_DEFAULT_G
                      , FOdysseyVectorObject::BACKGROUNDCOLOR_DEFAULT_B
                      , FOdysseyVectorObject::BACKGROUNDCOLOR_DEFAULT_A )
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
        Visible = Opacity ? true : false;

        ForegroundColorMode = static_cast<eForegroundColorMode>(focusedObject->GetForegroundBucket().GetColorMode());
        BackgroundColorMode = static_cast<eBackgroundColorMode>(focusedObject->GetBackgroundBucket().GetColorMode());

        ForegroundColor = focusedObject->GetForegroundBucket().GetSolidColor();
        BackgroundColor = focusedObject->GetBackgroundBucket().GetSolidColor();

        if( focusedObject->GetForegroundBucket().GetPaletteEntry() )
        {
            ForegroundPaletteSelection.OdysseyPalette = focusedObject->GetForegroundBucket().GetPaletteEntry()->GetPalette();
            //Careful, we can't select anything else than a color FOR NOW, so the cast is correct, but later, when we'll have material, we should change this accordingly
            ForegroundPaletteSelection.OdysseyPaletteEntryColor = Cast<UOdysseyPaletteEntryColor>( focusedObject->GetForegroundBucket().GetPaletteEntry() );
        }
        else
        {
            ForegroundPaletteSelection.OdysseyPalette = nullptr;
            ForegroundPaletteSelection.OdysseyPaletteEntryColor = nullptr;
        }

        if (focusedObject->GetBackgroundBucket().GetPaletteEntry())
        {
            BackgroundPaletteSelection.OdysseyPalette = focusedObject->GetBackgroundBucket().GetPaletteEntry()->GetPalette();
            //Careful, we can't select anything else than a color FOR NOW, so the cast is correct, but later, when we'll have material, we should change this accordingly
            BackgroundPaletteSelection.OdysseyPaletteEntryColor = Cast<UOdysseyPaletteEntryColor>( focusedObject->GetBackgroundBucket().GetPaletteEntry() );
        }
        else
        {
            BackgroundPaletteSelection.OdysseyPalette = nullptr;
            BackgroundPaletteSelection.OdysseyPaletteEntryColor = nullptr;
        }


        break; // only one
    }
}

void
UOdysseyPainterEditorVectorObjectView::Update( FOdysseyPainterEditor* iEditor
                                             , FOdysseyVectorGroupPaint* iScene
                                             , std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    mEditor = iEditor;
    mScene = iScene;
    mFocusedObjectList = iFocusedObjectList;

    ImportParam();
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory )
{
    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        // We have to change properties one by one especially in case of multiple selection.
        // We just cannot copy the whole block of properties.

        // Category "Identity"
        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Name) )
        {
            selectedObject->SetName( Name );
        }

        // Category "Transform"
        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationX) )
            selectedObject->Translate( TranslationX, selectedObject->GetTranslationY() );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationY) )
            selectedObject->Translate( selectedObject->GetTranslationX(), TranslationY );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Rotation) )
            selectedObject->Rotate( Rotation );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingX) )
            selectedObject->Scale( ScalingX, selectedObject->GetScalingY() );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingY) )
            selectedObject->Scale( selectedObject->GetScalingX(), ScalingY );

        if( iCategory == "Transform" )
            selectedObject->UpdateMatrix();

        // Category "Appearance"
        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Opacity) )
            selectedObject->SetOpacity( Opacity );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Visible) )
            selectedObject->SetOpacity( Visible ? 1.0f : 0.0f );

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColorMode) )
            selectedObject->GetForegroundBucket().SetColorMode( static_cast<eBucketColorMode>(ForegroundColorMode) );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) )
            selectedObject->GetForegroundBucket().SetSolidColor( ForegroundColor );

        if ( (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) )
            selectedObject->GetForegroundBucket().SetPaletteEntry( ForegroundPaletteSelection.OdysseyPaletteEntryColor );

        if ((iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)))
            selectedObject->GetBackgroundBucket().SetPaletteEntry( BackgroundPaletteSelection.OdysseyPaletteEntryColor);

        if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColorMode) )
            selectedObject->GetBackgroundBucket().SetColorMode( static_cast<eBucketColorMode>(BackgroundColorMode) );

        // note: iMemberPropertyName because FColor is a struct
        // and we can edit individual struct members RGBA
        if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) )
            selectedObject->GetBackgroundBucket().SetSolidColor( BackgroundColor );
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
        uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                 | FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW;

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectParam( mScene, mFocusedObjectList, notificationFlags );
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

        mScene->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );

        mScene->GetEngine()->Invalidate( 0 );
    }
}

#undef LOCTEXT_NAMESPACE
