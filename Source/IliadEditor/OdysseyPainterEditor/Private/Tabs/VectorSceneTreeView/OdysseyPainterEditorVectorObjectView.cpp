// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorObjectView.h"
//#include "Undo/OdysseyVectorUndoPropertyChanged.h"
#include "Undo/OdysseyVectorUndoObjectParam.h"
#include "OdysseyVectorEngine.h"
#include "OdysseyVectorCell.h"
#include "OdysseyVectorLayer.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorSource.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

UOdysseyPainterEditorVectorObjectView::~UOdysseyPainterEditorVectorObjectView()
{
}

UOdysseyPainterEditorVectorObjectView::UOdysseyPainterEditorVectorObjectView()
    : mEditor( nullptr )
    , mScene( nullptr )
    , mEditionMode( EditionMode::Direct )
    , mPropertyBits ( {0} )
    , bDisplayBackgroundProperties( false )
    , bDisplayForegroundProperties( false )
    , ApplyTo( EObjectViewApplyPolicy::Selection )
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
UOdysseyPainterEditorVectorObjectView::SetEditionMode( EditionMode iEditionMode )
{
    mEditionMode = iEditionMode;
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
        Opacity = focusedObject->GetOpacity();
        Visible = focusedObject->IsVisible( false );

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
UOdysseyPainterEditorVectorObjectView::ValidateProperties()
{
    for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
    {
        // Category "Identity"
        if( mPropertyBits.Name )
        {
            selectedObject->SetName( Name );
        }

        // Category "Transform"
        if( mPropertyBits.TranslationX )
            selectedObject->Translate( TranslationX, selectedObject->GetTranslationY() );

        if( mPropertyBits.TranslationY )
            selectedObject->Translate( selectedObject->GetTranslationX(), TranslationY );

        if( mPropertyBits.Rotation )
            selectedObject->Rotate( Rotation );

        if( mPropertyBits.ScalingX )
            selectedObject->Scale( ScalingX, selectedObject->GetScalingY() );

        if( mPropertyBits.ScalingY )
            selectedObject->Scale( selectedObject->GetScalingX(), ScalingY );

        if( mPropertyBits.TranslationX
         || mPropertyBits.TranslationY
         || mPropertyBits.Rotation
         || mPropertyBits.ScalingX
         || mPropertyBits.ScalingY )
            selectedObject->UpdateMatrix();

        // Category "Appearance"
        if( mPropertyBits.Opacity )
            selectedObject->SetOpacity( Opacity );

        if( mPropertyBits.Visible )
            selectedObject->SetVisible( Visible );

        if( mPropertyBits.ForegroundColorMode )
            selectedObject->GetForegroundBucket().SetColorMode( static_cast<eBucketColorMode>(ForegroundColorMode) );

        if( mPropertyBits.ForegroundColor )
            selectedObject->GetForegroundBucket().SetSolidColor( ForegroundColor );

        if ( mPropertyBits.ForegroundPaletteSelection )
        {
            selectedObject->GetForegroundBucket().SetPaletteEntry( ForegroundPaletteSelection.OdysseyPaletteEntryColor );
        }

        if( mPropertyBits.BackgroundColorMode )
            selectedObject->GetBackgroundBucket().SetColorMode( static_cast<eBucketColorMode>(BackgroundColorMode) );

        if( mPropertyBits.BackgroundColor )
            selectedObject->GetBackgroundBucket().SetSolidColor( BackgroundColor );

        if ( mPropertyBits.BackgroundPaletteSelection )
        {
            selectedObject->GetBackgroundBucket().SetPaletteEntry( BackgroundPaletteSelection.OdysseyPaletteEntryColor );
        }
    }

    memset( &mPropertyBits, 0, sizeof( mPropertyBits ) );
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory )
{
    // Category "Identity"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Name) )
        mPropertyBits.Name = 1;

    // Category "Transform"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationX) )
        mPropertyBits.TranslationX = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationY) )
        mPropertyBits.TranslationY = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Rotation) )
        mPropertyBits.Rotation = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingX) )
        mPropertyBits.ScalingX = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingY) )
        mPropertyBits.ScalingY = 1;

    // Category "Appearance"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Opacity) )
        mPropertyBits.Opacity = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Visible) )
        mPropertyBits.Visible = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColorMode) )
        mPropertyBits.ForegroundColorMode = 1;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) )
        mPropertyBits.ForegroundColor = 1;

    if ( (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) )
        mPropertyBits.ForegroundPaletteSelection = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColorMode) )
        mPropertyBits.BackgroundColorMode = 1;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) )
        mPropertyBits.BackgroundColor = 1;

    if ((iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)))
        mPropertyBits.BackgroundPaletteSelection = 1;

    if( mEditionMode == EditionMode::Direct )
    {
        ValidateProperties();
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
        uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                 | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

        // needed for valid GUndo pointer
        GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectParam( mScene
                                                                        , mFocusedObjectList
                                                                        , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category")))
                                                                        , notificationFlags
                                                                        | FOdysseyPainterEditor::UI_UPDATE_OBJECTDETAILS
                                                                        | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD );
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

        // force redraw the whole screen
        mScene->GetCell()->InvalidateRect();

        mScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
        mScene->GetLayer()->RequestRedraw( mScene->GetCell(), 0 );

        mScene->GetLayer()->Notify( notificationFlags );
    }
}

FOdysseyPainterEditor* UOdysseyPainterEditorVectorObjectView::GetEditor()
{
    return mEditor;
}

#undef LOCTEXT_NAMESPACE
