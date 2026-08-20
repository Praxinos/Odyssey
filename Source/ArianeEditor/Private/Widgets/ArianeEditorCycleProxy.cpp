// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorCycleProxy.h"
// Ariane Headers
#include "ArianeCycle.h"

UArianeEditorCycleProxy::~UArianeEditorCycleProxy()
{
}

UArianeEditorCycleProxy::UArianeEditorCycleProxy()
    : UArianeEditorObjectProxy()
    , CyclePropertyBits( {{ 0 }} )
    , Color ( FColor::Black.WithAlpha(255) )
    , Material ( nullptr )
    //, Brush ( nullptr )
{
    //bDisplayBackgroundProperties = false;
    //bDisplayForegroundProperties = true;
}

void
UArianeEditorCycleProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorCycleProxy* OtherCycleView = Cast<UArianeEditorCycleProxy>(OtherProxy);

    if( OtherCycleView )
    {
        CyclePropertyBits = OtherCycleView->CyclePropertyBits;

        Material = OtherCycleView->Material;
        Color = OtherCycleView->Color;
    }

    UArianeEditorObjectProxy::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorCycleProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianeCycle::StaticClass() ) )
        {
            FArianeCycle* ModifiedCycle = static_cast<FArianeCycle*>(ModifiedObject);

            Material = ModifiedCycle->GetMaterial();
            Color    = ModifiedCycle->GetColor();

            break; // only one
        }
    }
}

void
UArianeEditorCycleProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &CyclePropertyBits, 0, sizeof( CyclePropertyBits ) );
}

bool
UArianeEditorCycleProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( CyclePropertyBits ); i++  )
    {
        if( CyclePropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorCycleProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorCycleProxy, Material) )
        return CyclePropertyBits.Material;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorCycleProxy, Color) )
        return CyclePropertyBits.Color;

    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorCycleProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianeCycle::StaticClass() ) )
    {
        FArianeCycle* Cycle = static_cast<FArianeCycle*>(Object);

        if( CyclePropertyBits.Material )
            Cycle->SetMaterial( Material );

        if( CyclePropertyBits.Color )
            Cycle->SetColor( Color );
    }
}

void
UArianeEditorCycleProxy::SetPropertyBit( const FName& PropertyName
                                      , const FName& MemberPropertyName
                                      , const FName& Category
                                      , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorCycleProxy, Material) )
        CyclePropertyBits.Material = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorCycleProxy, Color) )
        CyclePropertyBits.Color = State;
}
