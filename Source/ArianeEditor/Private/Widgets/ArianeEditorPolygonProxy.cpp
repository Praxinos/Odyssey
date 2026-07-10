// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorPolygonProxy.h"
// Ariane Headers
#include "ArianePolygon.h"

UArianeEditorPolygonProxy::~UArianeEditorPolygonProxy()
{
}

UArianeEditorPolygonProxy::UArianeEditorPolygonProxy()
    : UArianeEditorPrimitiveProxy()
    , PolygonPropertyBits( {{ 0 }} )
    , CornerCount ( 3 )
    , Radius ( 0.0f )
{
}

void
UArianeEditorPolygonProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorPolygonProxy* OtherPathView = Cast<UArianeEditorPolygonProxy>(OtherProxy);

    if( OtherPathView )
    {
        PolygonPropertyBits = OtherPathView->PolygonPropertyBits;

        CornerCount = OtherPathView->CornerCount;
        Radius = OtherPathView->Radius;
    }

    Super::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorPolygonProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePolygon* ModifiedPolygon = static_cast<FArianePolygon*>(ModifiedObject);

            CornerCount = ModifiedPolygon->GetCornerCount();
            Radius = ModifiedPolygon->GetRadius();

            break; // only one
        }
    }
}

void
UArianeEditorPolygonProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &PolygonPropertyBits, 0, sizeof( PolygonPropertyBits ) );
}

bool
UArianeEditorPolygonProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( PolygonPropertyBits ); i++  )
    {
        if( PolygonPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorPolygonProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPolygonProxy, CornerCount) )
        return PolygonPropertyBits.CornerCount;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPolygonProxy, Radius) )
        return PolygonPropertyBits.Radius;


    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorPolygonProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianePath::StaticClass() ) )
    {
        FArianePolygon* Polygon = static_cast<FArianePolygon*>(Object);

        if( PolygonPropertyBits.CornerCount )
            Polygon->SetCornerCount( CornerCount );

        if( PolygonPropertyBits.Radius )
            Polygon->SetRadius( Radius );
    }
}

void
UArianeEditorPolygonProxy::SetPropertyBit( const FName& PropertyName
                                         , const FName& MemberPropertyName
                                         , const FName& Category
                                         , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPolygonProxy, CornerCount) )
        PolygonPropertyBits.CornerCount = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPolygonProxy, Radius) )
        PolygonPropertyBits.Radius = State;
}
