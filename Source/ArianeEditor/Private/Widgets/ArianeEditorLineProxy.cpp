// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorLineProxy.h"
// Ariane Headers
#include "ArianeLine.h"

UArianeEditorLineProxy::~UArianeEditorLineProxy()
{
}

UArianeEditorLineProxy::UArianeEditorLineProxy()
    : UArianeEditorPrimitiveProxy()
    , LinePropertyBits( {{ 0 }} )
{
}

void
UArianeEditorLineProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorLineProxy* OtherPathView = Cast<UArianeEditorLineProxy>(OtherProxy);

    if( OtherPathView )
    {
        LinePropertyBits = OtherPathView->LinePropertyBits;

        StartPoint = OtherPathView->StartPoint;
        EndPoint = OtherPathView->EndPoint;
    }

    Super::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorLineProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianeLine* ModifiedLine = static_cast<FArianeLine*>(ModifiedObject);

            StartPoint = ModifiedLine->GetStartPoint();
            EndPoint = ModifiedLine->GetEndPoint();

            break; // only one
        }
    }
}

void
UArianeEditorLineProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &LinePropertyBits, 0, sizeof( LinePropertyBits ) );
}

bool
UArianeEditorLineProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( LinePropertyBits ); i++  )
    {
        if( LinePropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorLineProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorLineProxy, StartPoint) )
        return LinePropertyBits.StartPoint;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorLineProxy, EndPoint) )
        return LinePropertyBits.EndPoint;


    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorLineProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianePath::StaticClass() ) )
    {
        FArianeLine* Line = static_cast<FArianeLine*>(Object);

        if( LinePropertyBits.StartPoint )
            Line->SetStartPoint( StartPoint );

        if( LinePropertyBits.EndPoint )
            Line->SetEndPoint( EndPoint );
    }
}

void
UArianeEditorLineProxy::SetPropertyBit( const FName& PropertyName
                                      , const FName& MemberPropertyName
                                      , const FName& Category
                                      , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );
    // MemberPropertyName instead of PropertyName because StartPoint is a struct
    if( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorLineProxy, StartPoint) )
        LinePropertyBits.StartPoint = State;

    // MemberPropertyName instead of PropertyName because EndPoint is a struct
    if( MemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorLineProxy, EndPoint) )
        LinePropertyBits.EndPoint = State;
}
