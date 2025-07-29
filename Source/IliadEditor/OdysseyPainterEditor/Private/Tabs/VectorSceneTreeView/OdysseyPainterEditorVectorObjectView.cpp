// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorVectorObjectView.h"
//#include "Undo/OdysseyVectorUndoPropertyChanged.h"
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
    : mVectorLayer( nullptr )
    , mObjectPropertyBits ( {0} )
    , bDisplayBackgroundProperties( true )
    , bDisplayForegroundProperties( true )
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
UOdysseyPainterEditorVectorObjectView::ImportParam( const std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    for( FOdysseyVectorObject* focusedObject : iFocusedObjectList )
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
UOdysseyPainterEditorVectorObjectView::Update( const std::list<FOdysseyVectorObject*>& iFocusedObjectList )
{
    ImportParam( iFocusedObjectList );
}

void
UOdysseyPainterEditorVectorObjectView::SetVectorLayer( TSharedPtr<FOdysseyVectorLayer> iVectorLayer )
{
    mVectorLayer = iVectorLayer;
}

TSharedPtr<FOdysseyVectorLayer>
UOdysseyPainterEditorVectorObjectView::GetVectorLayer()
{
    return mVectorLayer;
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

bool
UOdysseyPainterEditorVectorObjectView::HasProperty( const FName& iPropertyName )
{
    for ( TFieldIterator<FProperty> it(GetClass()); it; ++it )
    {
        FProperty* property = (*it);

        if( property->GetName() == iPropertyName )
        {
            return true;
        }
    }

    return false;
}


bool
UOdysseyPainterEditorVectorObjectView::GetPropertyBit( const FName& iPropertyName )
{
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Name) )
        return mObjectPropertyBits.Name;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationX) )
        return mObjectPropertyBits.TranslationX;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationY) )
        return mObjectPropertyBits.TranslationY;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Rotation) )
        return mObjectPropertyBits.Rotation;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingX) )
        return mObjectPropertyBits.ScalingX;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingY) )
        return mObjectPropertyBits.ScalingY;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Opacity) )
        return mObjectPropertyBits.Opacity;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Visible) )
        return mObjectPropertyBits.Visible;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColorMode) )
        return mObjectPropertyBits.ForegroundColorMode;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) )
        return mObjectPropertyBits.ForegroundColor;

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection) )
        return mObjectPropertyBits.ForegroundPaletteSelection;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColorMode) )
        return mObjectPropertyBits.BackgroundColorMode;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) )
        return mObjectPropertyBits.BackgroundColor;

    if ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection) )
        return mObjectPropertyBits.BackgroundPaletteSelection;

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
UOdysseyPainterEditorVectorObjectView::ValidateProperties( const std::list<FOdysseyVectorObject*>& iObjectList )
{
    TSet<FOdysseyVectorCell*> cellSet;

    if( HasPropertyBits() )
    {
        for( FOdysseyVectorObject* selectedObject : iObjectList )
        {
            ApplyPropertyBits( selectedObject );

            cellSet.Add( selectedObject->GetCell() );
        }
    }

    ClearPropertyBits();

    for( FOdysseyVectorCell* cell : cellSet )
    {
        // force redraw the whole screen
        cell->InvalidateRect();
    }
}

void
UOdysseyPainterEditorVectorObjectView::ClearPropertyBits()
{
    memset( &mObjectPropertyBits, 0, sizeof( mObjectPropertyBits ) );
}

void
UOdysseyPainterEditorVectorObjectView::SetPropertyBit( const FName& iPropertyName
                                                     , const FName& iMemberPropertyName
                                                     , const FName& iCategory
                                                     , bool iState )
{
    // Category "Identity"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Name) )
        mObjectPropertyBits.Name = iState;

    // Category "Transform"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationX) )
        mObjectPropertyBits.TranslationX = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, TranslationY) )
        mObjectPropertyBits.TranslationY = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Rotation) )
        mObjectPropertyBits.Rotation = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingX) )
        mObjectPropertyBits.ScalingX = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ScalingY) )
        mObjectPropertyBits.ScalingY = iState;

    // Category "Appearance"
    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Opacity) )
        mObjectPropertyBits.Opacity = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, Visible) )
        mObjectPropertyBits.Visible = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColorMode) )
        mObjectPropertyBits.ForegroundColorMode = iState;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) )
     || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundColor) ) )
        mObjectPropertyBits.ForegroundColor = iState;

    if ( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection) )
      || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, ForegroundPaletteSelection) ) )
        mObjectPropertyBits.ForegroundPaletteSelection = iState;

    if( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColorMode) )
        mObjectPropertyBits.BackgroundColorMode = iState;

    // note: iMemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) )
     || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundColor) ) )
        mObjectPropertyBits.BackgroundColor = iState;

    if ( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection) )
      || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UOdysseyPainterEditorVectorObjectView, BackgroundPaletteSelection) ) )
        mObjectPropertyBits.BackgroundPaletteSelection = iState;
}

void
UOdysseyPainterEditorVectorObjectView::PropertyChanged( const FName& iPropertyName
                                                      , const FName& iMemberPropertyName
                                                      , const FName& iCategory )
{
    SetPropertyBit( iPropertyName, iMemberPropertyName, iCategory, true );
}

void
UOdysseyPainterEditorVectorObjectView::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    PropertyChanged( PropertyChangedEvent.GetPropertyName()
                   , PropertyChangedEvent.MemberProperty->GetFName()
                   , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );
}

#undef LOCTEXT_NAMESPACE
