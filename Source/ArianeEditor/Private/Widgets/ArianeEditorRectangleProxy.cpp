// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorRectangleProxy.h"
// Ariane Headers
#include "ArianeVertex.h"
#include "ArianeRectangle.h"

UArianeEditorRectangleProxy::~UArianeEditorRectangleProxy()
{
}

UArianeEditorRectangleProxy::UArianeEditorRectangleProxy()
    : UArianeEditorPrimitiveProxy()
    , RectanglePropertyBits( {{ 0 }} )
    , Width( 0.0f )
    , Height( 0.0f )
{
}

void
UArianeEditorRectangleProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorRectangleProxy* OtherRectangleProxy = Cast<UArianeEditorRectangleProxy>(OtherProxy);

    if( OtherRectangleProxy )
    {
        RectanglePropertyBits = OtherRectangleProxy->RectanglePropertyBits;

        Width = OtherRectangleProxy->Width;
        Height = OtherRectangleProxy->Height;
    }

    Super::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorRectangleProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianeRectangle::StaticClass() ) )
        {
            FArianeRectangle* ModifiedRectangle = static_cast<FArianeRectangle*>(ModifiedObject);

            Width = ModifiedRectangle->GetWidth();
            Height = ModifiedRectangle->GetHeight();

            break; // only one
        }
    }
}

void
UArianeEditorRectangleProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &RectanglePropertyBits, 0, sizeof( RectanglePropertyBits ) );
}

bool
UArianeEditorRectangleProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( RectanglePropertyBits ); i++  )
    {
        if( RectanglePropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorRectangleProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorRectangleProxy, Width) )
        return RectanglePropertyBits.Width;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorRectangleProxy, Height) )
        return RectanglePropertyBits.Height;


    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorRectangleProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianeRectangle::StaticClass() ) )
    {
        FArianeRectangle* Rectangle = static_cast<FArianeRectangle*>(Object);

        if( RectanglePropertyBits.Width )
            Rectangle->SetSize( Width, Rectangle->GetHeight() );

        if( RectanglePropertyBits.Height )
            Rectangle->SetSize( Rectangle->GetWidth(), Height );
    }
}

void
UArianeEditorRectangleProxy::SetPropertyBit( const FName& PropertyName
                                           , const FName& MemberPropertyName
                                           , const FName& Category
                                           , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorRectangleProxy, Width) )
        RectanglePropertyBits.Width = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorRectangleProxy, Height) )
        RectanglePropertyBits.Height = State;
}
