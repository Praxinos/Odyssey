// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane Editor Headers
#include "ArianeEditorObjectProxy.h"
// Ariane Headers
#include "ArianeObject.h"

#define LOCTEXT_NAMESPACE "ArianeEditor"

UArianeEditorObjectProxy::~UArianeEditorObjectProxy()
{
}

UArianeEditorObjectProxy::UArianeEditorObjectProxy()
    : ObjectPropertyBits ( {{0}} )
    //, bDisplayBackgroundProperties( true )
    //, bDisplayForegroundProperties( true )
    , Name( "Object" )
    //, TranslationX ( 0.0f )
    //, TranslationY ( 0.0f )
    //, Rotation ( 0.0f )
    //, ScalingX ( 1.0f )
    //, ScalingY ( 1.0f )
    //, SkewX ( 0.0f )
    //, SkewY ( 0.0f )
    , bVisible ( true )
    //, ForegroundColorMode ( eForegroundColorMode::SolidColor )
    //, ForegroundColor ( FArianeObject::FOREGROUNDCOLOR_DEFAULT_R
    //                  , FArianeObject::FOREGROUNDCOLOR_DEFAULT_G
    //                  , FArianeObject::FOREGROUNDCOLOR_DEFAULT_B
    //                  , FArianeObject::FOREGROUNDCOLOR_DEFAULT_A )
    //, BackgroundColorMode ( eBackgroundColorMode::SolidColor )
    //, BackgroundColor ( FArianeObject::BACKGROUNDCOLOR_DEFAULT_R
    //                  , FArianeObject::BACKGROUNDCOLOR_DEFAULT_G
    //                  , FArianeObject::BACKGROUNDCOLOR_DEFAULT_B
    //                  , FArianeObject::BACKGROUNDCOLOR_DEFAULT_A )
{
}

void
UArianeEditorObjectProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    ObjectPropertyBits = OtherProxy->ObjectPropertyBits;

    //TranslationX = iOtherView->TranslationX;
    //TranslationY = iOtherView->TranslationY;
    //Rotation = iOtherView->Rotation;
    //ScalingX = iOtherView->ScalingX;
    //ScalingY = iOtherView->ScalingY;
    //SkewX = iOtherView->SkewX;
    //SkewY = iOtherView->SkewY;
    //Opacity = iOtherView->Opacity;
    bVisible = OtherProxy->bVisible;
    //ForegroundColorMode = iOtherView->ForegroundColorMode;
    //BackgroundColorMode = iOtherView->BackgroundColorMode;
    //ForegroundColor = iOtherView->ForegroundColor;
    //BackgroundColor = iOtherView->BackgroundColor;
    //ForegroundPaletteSelection = iOtherView->ForegroundPaletteSelection;
    //BackgroundPaletteSelection = iOtherView->BackgroundPaletteSelection;
}

void
UArianeEditorObjectProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    if( ModifiedObjects.Num() )
    {
        FArianeObject* ModifiedObject = ModifiedObjects[0];

        // Category "Identity"
        Name = ModifiedObject->GetName();

        // Category "Transform"
        //TranslationX = focusedObject->GetTranslationX();
        //TranslationY = focusedObject->GetTranslationY();
        //Rotation     = focusedObject->GetRotation();
        //ScalingX     = focusedObject->GetScalingX();
        //ScalingY     = focusedObject->GetScalingY();
        //SkewX        = focusedObject->GetSkewX();
        //SkewY        = focusedObject->GetSkewY();

        // Category "Appearance"
        //Opacity = focusedObject->GetOpacity();
        bVisible = ModifiedObject->IsVisible( false );

        //ForegroundColorMode = static_cast<eForegroundColorMode>(focusedObject->GetForegroundBucket().GetColorMode());
        //BackgroundColorMode = static_cast<eBackgroundColorMode>(focusedObject->GetBackgroundBucket().GetColorMode());

        //ForegroundColor = focusedObject->GetForegroundBucket().GetSolidColor();
        //BackgroundColor = focusedObject->GetBackgroundBucket().GetSolidColor();

        //if( SelectedObject->GetForegroundBucket().GetPaletteEntry() )
        //{
        //    ForegroundPaletteSelection.OdysseyPalette = focusedObject->GetForegroundBucket().GetPaletteEntry()->GetPalette();
            //Careful, we can't select anything else than a color FOR NOW, so the cast is correct, but later, when we'll have material, we should change this accordingly
        //    ForegroundPaletteSelection.OdysseyPaletteEntryColor = Cast<UOdysseyPaletteEntryColor>( focusedObject->GetForegroundBucket().GetPaletteEntry() );
        //}
        //else
        //{
        //    ForegroundPaletteSelection.OdysseyPalette = nullptr;
        //    ForegroundPaletteSelection.OdysseyPaletteEntryColor = nullptr;
        //}

        //if (focusedObject->GetBackgroundBucket().GetPaletteEntry())
        //{
        //    BackgroundPaletteSelection.OdysseyPalette = focusedObject->GetBackgroundBucket().GetPaletteEntry()->GetPalette();
            //Careful, we can't select anything else than a color FOR NOW, so the cast is correct, but later, when we'll have material, we should change this accordingly
        //    BackgroundPaletteSelection.OdysseyPaletteEntryColor = Cast<UOdysseyPaletteEntryColor>( focusedObject->GetBackgroundBucket().GetPaletteEntry() );
        //}
        //else
        //{
        //    BackgroundPaletteSelection.OdysseyPalette = nullptr;
        //    BackgroundPaletteSelection.OdysseyPaletteEntryColor = nullptr;
        //}
    }
}

void
UArianeEditorObjectProxy::Update( const TArray<FArianeObject*>& ModifiedObjects )
{
    ImportParam( ModifiedObjects );
}

bool
UArianeEditorObjectProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( ObjectPropertyBits ); i++  )
    {
        if( ObjectPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return false;
}

bool
UArianeEditorObjectProxy::HasProperty( const FName& PropertyName )
{
    for ( TFieldIterator<FProperty> it(GetClass()); it; ++it )
    {
        FProperty* property = (*it);

        if( property->GetName() == PropertyName )
        {
            return true;
        }
    }

    return false;
}


bool
UArianeEditorObjectProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Name) )
        return ObjectPropertyBits.Name;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, TranslationX) )
    //    return ObjectPropertyBits.TranslationX;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, TranslationY) )
    //    return ObjectPropertyBits.TranslationY;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Rotation) )
    //    return ObjectPropertyBits.Rotation;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ScalingX) )
    //    return ObjectPropertyBits.ScalingX;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ScalingY) )
    //    return ObjectPropertyBits.ScalingY;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, SkewX) )
    //    return ObjectPropertyBits.SkewX;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, SkewY) )
    //    return ObjectPropertyBits.SkewY;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Opacity) )
    //    return ObjectPropertyBits.Opacity;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, bVisible) )
        return ObjectPropertyBits.Visible;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundColorMode) )
    //    return ObjectPropertyBits.ForegroundColorMode;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundColor) )
    //    return ObjectPropertyBits.ForegroundColor;

    //if ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundPaletteSelection) )
    //    return ObjectPropertyBits.ForegroundPaletteSelection;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundColorMode) )
    //    return ObjectPropertyBits.BackgroundColorMode;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundColor) )
    //    return ObjectPropertyBits.BackgroundColor;

    //if ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundPaletteSelection) )
    //    return ObjectPropertyBits.BackgroundPaletteSelection;

    return false;
}

void
UArianeEditorObjectProxy::ApplyPropertyBits( FArianeObject* Object )
{
    // Category "Identity"
    if( ObjectPropertyBits.Name )
    {
        Object->SetName( Name );
    }

    // Category "Transform"
    //if( ObjectPropertyBits.TranslationX )
    //    Object->Translate( TranslationX, Object->GetTranslationY() );

    //if( ObjectPropertyBits.TranslationY )
    //    Object->Translate( Object->GetTranslationX(), TranslationY );

    //if( ObjectPropertyBits.Rotation )
    //    Object->Rotate( Rotation );

    //if( ObjectPropertyBits.ScalingX )
    //    Object->Scale( ScalingX, Object->GetScalingY() );

    //if( ObjectPropertyBits.ScalingY )
    //    Object->Scale( Object->GetScalingX(), ScalingY );

    //if( ObjectPropertyBits.SkewX )
    //    Object->Skew( SkewX, Object->GetSkewY() );

    //if( ObjectPropertyBits.SkewY )
    //    Object->Skew( Object->GetSkewX(), SkewY );

    //if( ObjectPropertyBits.TranslationX
    // || ObjectPropertyBits.TranslationY
    // || ObjectPropertyBits.Rotation
    // || ObjectPropertyBits.ScalingX
    // || ObjectPropertyBits.ScalingY
    // || ObjectPropertyBits.SkewX
    // || ObjectPropertyBits.SkewY )
    //    Object->UpdateMatrix();

    // Category "Appearance"
    //if( ObjectPropertyBits.Opacity )
    //    Object->SetOpacity( Opacity );

    if( ObjectPropertyBits.Visible )
        Object->SetVisible( bVisible );

    //if( ObjectPropertyBits.ForegroundColorMode )
    //    Object->GetForegroundBucket().SetColorMode( static_cast<eBucketColorMode>(ForegroundColorMode) );

    //if( ObjectPropertyBits.ForegroundColor )
    //    Object->GetForegroundBucket().SetSolidColor( ForegroundColor );

    //if ( ObjectPropertyBits.ForegroundPaletteSelection )
    //    Object->GetForegroundBucket().SetPaletteEntry( ForegroundPaletteSelection.OdysseyPaletteEntryColor );

    //if( ObjectPropertyBits.BackgroundColorMode )
    //    Object->GetBackgroundBucket().SetColorMode( static_cast<eBucketColorMode>(BackgroundColorMode) );

    //if( ObjectPropertyBits.BackgroundColor )
    //    Object->GetBackgroundBucket().SetSolidColor( BackgroundColor );

    //if ( ObjectPropertyBits.BackgroundPaletteSelection )
    //    Object->GetBackgroundBucket().SetPaletteEntry( BackgroundPaletteSelection.OdysseyPaletteEntryColor );
}

void
UArianeEditorObjectProxy::ValidateProperties( TArray<FArianeObject*>& ModifiedObjects, bool bClearBits )
{
    //TSet<FOdysseyVectorCell*> cellSet;

    if( HasAnyPropertyBit() )
    {
        for( FArianeObject* ModifiedObject : ModifiedObjects )
        {
            ApplyPropertyBits( ModifiedObject );

            //cellSet.Add( selectedObject->GetCell() );
        }
    }

    if( bClearBits )
    {
        ClearPropertyBits();
    }
}

void
UArianeEditorObjectProxy::ClearPropertyBits()
{
    memset( &ObjectPropertyBits, 0, sizeof( ObjectPropertyBits ) );
}

void
UArianeEditorObjectProxy::SetPropertyBit( const FName& PropertyName
                                        , const FName& MemberPropertyName
                                        , const FName& Category
                                        , bool bState )
{
    // Category "Identity"
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Name) )
        ObjectPropertyBits.Name = bState;

    // Category "Transform"
    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, TranslationX) )
    //    ObjectPropertyBits.TranslationX = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, TranslationY) )
    //    ObjectPropertyBits.TranslationY = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Rotation) )
    //    ObjectPropertyBits.Rotation = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ScalingX) )
    //    ObjectPropertyBits.ScalingX = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ScalingY) )
    //    ObjectPropertyBits.ScalingY = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, SkewX) )
    //    ObjectPropertyBits.SkewX = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, SkewY) )
    //    ObjectPropertyBits.SkewY = bState;

    // Category "Appearance"
    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, Opacity) )
    //    ObjectPropertyBits.Opacity = bState;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, bVisible) )
        ObjectPropertyBits.Visible = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundColorMode) )
    //    ObjectPropertyBits.ForegroundColorMode = bState;

    // note: MemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    //if( ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundColor) )
    // || ( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundColor) ) )
    //    ObjectPropertyBits.ForegroundColor = bState;

    //if ( ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundPaletteSelection) )
    //  || ( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, ForegroundPaletteSelection) ) )
    //    ObjectPropertyBits.ForegroundPaletteSelection = bState;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundColorMode) )
    //    ObjectPropertyBits.BackgroundColorMode = bState;

    // note: MemberPropertyName because FColor is a struct
    // and we can edit individual struct members RGBA
    //if( ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundColor) )
    // || ( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundColor) ) )
    //    ObjectPropertyBits.BackgroundColor = bState;

    //if ( ( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundPaletteSelection) )
    //  || ( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorObjectProxy, BackgroundPaletteSelection) ) )
    //    ObjectPropertyBits.BackgroundPaletteSelection = bState;
}

void
UArianeEditorObjectProxy::PropertyChanged( const FName& PropertyName
                                         , const FName& MemberPropertyName
                                         , const FName& Category )
{
    SetPropertyBit( PropertyName, MemberPropertyName, Category, true );
}

void
UArianeEditorObjectProxy::PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent )
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive )
        return;

    PropertyChanged( PropertyChangedEvent.GetPropertyName()
                   , PropertyChangedEvent.MemberProperty->GetFName()
                   , FName(PropertyChangedEvent.Property->GetMetaData(TEXT("Category"))) );
}

#undef LOCTEXT_NAMESPACE
