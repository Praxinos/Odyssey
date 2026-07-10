// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorPrimitiveProxy.h"
// Ariane Headers
#include "ArianeVertex.h"
#include "ArianePrimitive.h"

UArianeEditorPrimitiveProxy::~UArianeEditorPrimitiveProxy()
{
}

UArianeEditorPrimitiveProxy::UArianeEditorPrimitiveProxy()
    : UArianeEditorPathProxy()
    , PrimitivePropertyBits( {{ 0 }} )
{
}

void
UArianeEditorPrimitiveProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorPrimitiveProxy* OtherPrimitiveProxy = Cast<UArianeEditorPrimitiveProxy>(OtherProxy);

    if( OtherPrimitiveProxy )
    {
        PrimitivePropertyBits = OtherPrimitiveProxy->PrimitivePropertyBits;

        StrokeWidth = OtherPrimitiveProxy->StrokeWidth;
    }

    UArianeEditorObjectProxy::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorPrimitiveProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianePrimitive::StaticClass() ) )
        {
            FArianePrimitive* ModifiedPrimitive = static_cast<FArianePrimitive*>(ModifiedObject);

            StrokeWidth = ModifiedPrimitive->GetStrokeWidth();

            break; // only one
        }
    }
}

void
UArianeEditorPrimitiveProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &PrimitivePropertyBits, 0, sizeof( PrimitivePropertyBits ) );
}

bool
UArianeEditorPrimitiveProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( PrimitivePropertyBits ); i++  )
    {
        if( PrimitivePropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorPrimitiveProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPrimitiveProxy, StrokeWidth) )
    {
        return true;
    }

    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorPrimitiveProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianePrimitive::StaticClass() ) )
    {
        FArianePrimitive* Primitive = static_cast<FArianePrimitive*>(Object);

        if( PrimitivePropertyBits.StrokeWidth )
        {
            Primitive->SetStrokeWidth( StrokeWidth );
        }
    }
}

void
UArianeEditorPrimitiveProxy::SetPropertyBit( const FName& PropertyName
                                           , const FName& MemberPropertyName
                                           , const FName& Category
                                           , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPrimitiveProxy, StrokeWidth) )
    {
        PrimitivePropertyBits.StrokeWidth = State;
    }
}
