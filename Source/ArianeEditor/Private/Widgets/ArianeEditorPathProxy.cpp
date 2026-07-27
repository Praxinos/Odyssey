// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Arien Editor Headers
#include "ArianeEditorPathProxy.h"
// Ariane Headers
#include "ArianeVertex.h"
#include "ArianePath.h"

UArianeEditorPathProxy::~UArianeEditorPathProxy()
{
}

UArianeEditorPathProxy::UArianeEditorPathProxy()
    : UArianeEditorObjectProxy()
    , PathPropertyBits( {{ 0 }} )
    , bDisplayWideningOptions( false )
    , WideningMode ( EArianeEditorPathProxyWideningMode::Percent )
    , PathWidthInPercent ( 100.0f )
    , PathWidthInUnits ( 2.0f )
    , Material ( nullptr )
    , LineType( EArianePathLineType::Tube )
    //, Brush ( nullptr )
{
    //bDisplayBackgroundProperties = false;
    //bDisplayForegroundProperties = true;
}

void
UArianeEditorPathProxy::SetDisplayWideningOptions( bool iValue )
{
    bDisplayWideningOptions = iValue;
}

void
UArianeEditorPathProxy::ImportParamFromOtherProxy( UArianeEditorObjectProxy* OtherProxy )
{
    UArianeEditorPathProxy* OtherPathView = Cast<UArianeEditorPathProxy>(OtherProxy);

    if( OtherPathView )
    {
        PathPropertyBits = OtherPathView->PathPropertyBits;

        WideningMode = OtherPathView->WideningMode;
        PathWidthInPercent = OtherPathView->PathWidthInPercent;
        PathWidthInUnits = OtherPathView->PathWidthInUnits;
        Material = OtherPathView->Material;
        LineType = OtherPathView->LineType;
        //Brush = otherPathView->Brush;
    }

    UArianeEditorObjectProxy::ImportParamFromOtherProxy( OtherProxy );
}

void
UArianeEditorPathProxy::ImportParam( const TArray<FArianeObject*>& ModifiedObjects )
{
    Super::ImportParam( ModifiedObjects );

    for( FArianeObject* ModifiedObject : ModifiedObjects )
    {
        if( ModifiedObject->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* ModifiedPath = static_cast<FArianePath*>(ModifiedObject);


            Material = ModifiedPath->GetMaterial();
            LineType = ModifiedPath->GetLineType();
            //JointType  = ModifiedPath->GetJointType();
            //Brush      = ModifiedPath->GetBrush();
            //MiterLimit = ModifiedPath->GetMiterLimit();

            break; // only one
        }
    }
}

void
UArianeEditorPathProxy::ClearPropertyBits()
{
    Super::ClearPropertyBits();

    memset( &PathPropertyBits, 0, sizeof( PathPropertyBits ) );
}

bool
UArianeEditorPathProxy::HasAnyPropertyBit()
{
    // we use a loop so that we don't forget any flags, even the ones that will be added later
    for( uint32 i = 0; i < sizeof( PathPropertyBits ); i++  )
    {
        if( PathPropertyBits.raw[i] )
        {
            return true;
        }
    }

    return Super::HasAnyPropertyBit();
}

bool
UArianeEditorPathProxy::GetPropertyBit( const FName& PropertyName )
{
    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, WideningMode) )
        return true;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, PathWidthInPercent) )
        return PathPropertyBits.PathWidthInPercent;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, PathWidthInUnits) )
        return PathPropertyBits.PathWidthInUnits;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, Material) )
        return PathPropertyBits.Material;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, LineType) )
        return PathPropertyBits.LineType;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, JointType) )
    //    return PathPropertyBits.JointType;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, Brush) )
    //    return PathPropertyBits.Brush;

    //if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, MiterLimit) )
    //    return PathPropertyBits.MiterLimit;

    return Super::GetPropertyBit( PropertyName );
}

void
UArianeEditorPathProxy::ApplyPropertyBits( FArianeObject* Object )
{
    Super::ApplyPropertyBits( Object );

    if( Object->HasBaseClass( FArianePath::StaticClass() ) )
    {
        FArianePath* Path = static_cast<FArianePath*>(Object);

        if( WideningMode == EArianeEditorPathProxyWideningMode::Percent )
        {
            if( PathPropertyBits.PathWidthInPercent )
            {
                for( FArianeVertexID& VertexID : Path->GetVertices() )
                {
                    FArianeVertex* Vertex = VertexID.GetVertex();

                    Vertex->SetRadius( Vertex->GetRadius() * PathWidthInPercent * 0.01f );
                }
            }
        }

        if( WideningMode == EArianeEditorPathProxyWideningMode::Units )
        {
            if( PathPropertyBits.PathWidthInUnits )
            {
                for( FArianeVertexID& VertexID : Path->GetVertices() )
                {
                    FArianeVertex* Vertex = VertexID.GetVertex();

                    Vertex->SetRadius( PathWidthInUnits );
                }
            }
        }

        if( PathPropertyBits.Material )
            Path->SetMaterial( Material );

        if( PathPropertyBits.LineType )
            Path->SetLineType( LineType );

        //if( PathPropertyBits.JointType )
        //    Path->SetJointType( JointType, true );

        //if( PathPropertyBits.Brush )
        //    Path->SetBrush( Brush );

        //if( PathPropertyBits.MiterLimit )
        //    Path->SetMiterLimit( MiterLimit, true );
    }
}

void
UArianeEditorPathProxy::SetPropertyBit( const FName& PropertyName
                                      , const FName& MemberPropertyName
                                      , const FName& Category
                                      , bool State )
{
    Super::SetPropertyBit( PropertyName
                         , MemberPropertyName
                         , Category
                         , State );

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, PathWidthInPercent) )
        PathPropertyBits.PathWidthInPercent = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, PathWidthInUnits) )
        PathPropertyBits.PathWidthInUnits = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, Material) )
        PathPropertyBits.Material = State;

    if( PropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, LineType) )
        PathPropertyBits.LineType = State;

    //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, JointType) )
    //    PathPropertyBits.JointType = iState;

    //if( ( iPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, Brush) )
    // || ( iMemberPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, Brush) ) )
    //    PathPropertyBits.Brush = iState;

    //if( iPropertyName == GET_MEMBER_NAME_CHECKED(UArianeEditorPathProxy, MiterLimit) )
    //    PathPropertyBits.MiterLimit = iState;
}
