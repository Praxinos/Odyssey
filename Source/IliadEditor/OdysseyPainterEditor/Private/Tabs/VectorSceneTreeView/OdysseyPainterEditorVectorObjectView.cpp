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
    , mEditionMode( EObjectViewEditionMode::Direct )
    , mObjectPropertyBits ( {0} )
    , bDisplayBackgroundProperties( true )
    , bDisplayForegroundProperties( true )
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
UOdysseyPainterEditorVectorObjectView::SetEditionMode( EObjectViewEditionMode iEditionMode )
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

bool
UOdysseyPainterEditorVectorObjectView::HasPropertyBits()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( mObjectPropertyBits ); i++  )
    {
        if( mObjectPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return false;
}

void
UOdysseyPainterEditorVectorObjectView::ApplyPropertyBits( FOdysseyVectorObject* iObject )
{
    // Category "Identity"
    if( mObjectPropertyBits.Name )
    {
        iObject->SetName( Name );
    }

    // Category "Transform"
    if( mObjectPropertyBits.TranslationX )
        iObject->Translate( TranslationX, iObject->GetTranslationY() );

    if( mObjectPropertyBits.TranslationY )
        iObject->Translate( iObject->GetTranslationX(), TranslationY );

    if( mObjectPropertyBits.Rotation )
        iObject->Rotate( Rotation );

    if( mObjectPropertyBits.ScalingX )
        iObject->Scale( ScalingX, iObject->GetScalingY() );

    if( mObjectPropertyBits.ScalingY )
        iObject->Scale( iObject->GetScalingX(), ScalingY );

    if( mObjectPropertyBits.TranslationX
     || mObjectPropertyBits.TranslationY
     || mObjectPropertyBits.Rotation
     || mObjectPropertyBits.ScalingX
     || mObjectPropertyBits.ScalingY )
        iObject->UpdateMatrix();

    // Category "Appearance"
    if( mObjectPropertyBits.Opacity )
        iObject->SetOpacity( Opacity );

    if( mObjectPropertyBits.Visible )
        iObject->SetVisible( Visible );

    if( mObjectPropertyBits.ForegroundColorMode )
        iObject->GetForegroundBucket().SetColorMode( static_cast<eBucketColorMode>(ForegroundColorMode) );

    if( mObjectPropertyBits.ForegroundColor )
        iObject->GetForegroundBucket().SetSolidColor( ForegroundColor );

    if ( mObjectPropertyBits.ForegroundPaletteSelection )
        iObject->GetForegroundBucket().SetPaletteEntry( ForegroundPaletteSelection.OdysseyPaletteEntryColor );

    if( mObjectPropertyBits.BackgroundColorMode )
        iObject->GetBackgroundBucket().SetColorMode( static_cast<eBucketColorMode>(BackgroundColorMode) );

    if( mObjectPropertyBits.BackgroundColor )
        iObject->GetBackgroundBucket().SetSolidColor( BackgroundColor );

    if ( mObjectPropertyBits.BackgroundPaletteSelection )
        iObject->GetBackgroundBucket().SetPaletteEntry( BackgroundPaletteSelection.OdysseyPaletteEntryColor );
}

void
UOdysseyPainterEditorVectorObjectView::ValidateProperties()
{
    uint64 notificationFlags = FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
                                | FOdysseyVectorEngine::NOTIFY_UPDATE_HUD;

    if( HasPropertyBits() )
    {
        if( ApplyTo == EObjectViewApplyPolicy::Selection )
        {
            for( FOdysseyVectorObject* selectedObject : mFocusedObjectList )
            {
                ApplyPropertyBits( selectedObject );
            }
        }

        if( ApplyTo == EObjectViewApplyPolicy::AllInCell )
        {
            FOdysseyVectorObject::Traverse( mScene
                                          , 0
                                          , [ this ]( FOdysseyVectorObject* object, uint64 traversalFlags )
                                          {
                                              ApplyPropertyBits( object );

                                              return FOdysseyVectorObject::TRAVERSE_CONTINUE;
                                          } );
        }

        if( ApplyTo == EObjectViewApplyPolicy::AllInAllCells )
        {
            FOdysseyVectorObject::Traverse( mScene->GetLayer()
                                          , 0
                                          , [ this ]( FOdysseyVectorObject* object, uint64 traversalFlags )
                                          {
                                              ApplyPropertyBits( object );

                                              return FOdysseyVectorObject::TRAVERSE_CONTINUE;
                                          } );
        }
    }

    ClearPropertyBits();

    // force redraw the whole screen
    mScene->GetCell()->InvalidateRect();

    mScene->GetLayer()->Update( FOdysseyVectorObject::UPDATE_PAINTGROUPS );
    mScene->GetLayer()->RequestRedraw( mScene->GetCell(), 0 );

    mScene->GetLayer()->Notify( notificationFlags );
}

void
UOdysseyPainterEditorVectorObjectView::ClearPropertyBits()
{
    memset( &mObjectPropertyBits, 0, sizeof( mObjectPropertyBits ) );
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory )
{
    // Category "Identity"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Name) )
        mObjectPropertyBits.Name = 1;

    // Category "Transform"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationX) )
        mObjectPropertyBits.TranslationX = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationY) )
        mObjectPropertyBits.TranslationY = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Rotation) )
        mObjectPropertyBits.Rotation = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingX) )
        mObjectPropertyBits.ScalingX = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingY) )
        mObjectPropertyBits.ScalingY = 1;

    // Category "Appearance"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Opacity) )
        mObjectPropertyBits.Opacity = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Visible) )
        mObjectPropertyBits.Visible = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColorMode) )
        mObjectPropertyBits.ForegroundColorMode = 1;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) )
        mObjectPropertyBits.ForegroundColor = 1;

    if ( (iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection)) )
        mObjectPropertyBits.ForegroundPaletteSelection = 1;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColorMode) )
        mObjectPropertyBits.BackgroundColorMode = 1;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) )
        mObjectPropertyBits.BackgroundColor = 1;

    if ((iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)) || (iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection)))
        mObjectPropertyBits.BackgroundPaletteSelection = 1;
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
        GEditor->BeginTransaction(LOCTEXT("vector-object.transaction.property-changed","Property Changed"));
        if( GUndo )
        {
            FOdysseyVectorUndo *undo = new FOdysseyVectorUndoObjectParam( mScene
                                                                        , mFocusedObjectList
                                                                        , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category")))
                                                                        , FOdysseyPainterEditor::UI_UPDATE_SCENETREEVIEW
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

        if( mEditionMode == EObjectViewEditionMode::Direct )
        {
            ValidateProperties();
        }
    }
}

FOdysseyPainterEditor* UOdysseyPainterEditorVectorObjectView::GetEditor()
{
    return mEditor;
}

#undef LOCTEXT_NAMESPACE
