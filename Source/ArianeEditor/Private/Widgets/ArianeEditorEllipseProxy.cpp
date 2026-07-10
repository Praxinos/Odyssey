// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorEllipseProxy.h"
// Ariane Headers
#include "ArianeVertex.h"
#include "ArianeEllipse.h"

UArianeEditorEllipseProxy::~UArianeEditorEllipseProxy()
{
}

UArianeEditorEllipseProxy::UArianeEditorEllipseProxy()
    : UArianeEditorPrimitiveProxy()
    , EllipsePropertyBits( {{ 0 }} )
    , RadiusX ( 100.0f )
    , RadiusY ( 2.0f )
    //, Brush ( nullptr )
{
    //bDisplayBackgroundProperties = false;
    //bDisplayForegroundProperties = true;
}

void
UArianeEditorEllipseProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorEllipseProxy* OtherEllipseView = Cast<UArianeEditorEllipseProxy>(OtherProxy);

    if( OtherEllipseView )
    {
        EllipsePropertyBits = OtherEllipseView->EllipsePropertyBits;

        RadiusX = OtherEllipseView->RadiusX;
        RadiusY = OtherEllipseView->RadiusY;
    }

    Super::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorEllipseProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianeEllipse::StaticClass() ) )
        {
            FArianeEllipse* ModifiedEllipse = static_cast<FArianeEllipse*>(ModifiedObject);

            RadiusX = ModifiedEllipse->GetRadiusX();
            RadiusY = ModifiedEllipse->GetRadiusY();

            break; // only one
        }
    }
}

void
UArianeEditorEllipseProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &EllipsePropertyBits, 0, sizeof( EllipsePropertyBits ) );
}

bool
UArianeEditorEllipseProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( EllipsePropertyBits ); i++  )
    {
        if( EllipsePropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorEllipseProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorEllipseProxy, RadiusX) )
        return EllipsePropertyBits.RadiusX;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorEllipseProxy, RadiusY) )
        return EllipsePropertyBits.RadiusY;


    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorEllipseProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianeEllipse::StaticClass() ) )
    {
        FArianeEllipse* Ellipse = static_cast<FArianeEllipse*>(Object);

        if( EllipsePropertyBits.RadiusX )
            Ellipse->SetRadius( RadiusX, Ellipse->GetRadiusY() );

        if( EllipsePropertyBits.RadiusY )
            Ellipse->SetRadius( Ellipse->GetRadiusY(), RadiusY );
    }
}

void
UArianeEditorEllipseProxy::SetPropertyBit( const FName& PropertyName
                                         , const FName& MemberPropertyName
                                         , const FName& Category
                                         , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorEllipseProxy, RadiusX) )
        EllipsePropertyBits.RadiusX = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorEllipseProxy, RadiusY) )
        EllipsePropertyBits.RadiusY = State;
}
