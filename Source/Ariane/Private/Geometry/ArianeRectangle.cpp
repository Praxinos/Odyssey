// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeRectangle.h"
#include "ArianeVertex.h"
#include "ArianeSegmentCubic.h"
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

FArianeRectangle::~FArianeRectangle()
{
   // vertices ans segments freed in ArianePath::~destructor
}

FArianeRectangle::FArianeRectangle()
    : FArianeRectangle( nullptr
                      , FName ( "Ariane Rectangle" )
                      , 0.0f
                      , 0.0f
                      , 0.0f
                      , EArianeAllocationModel::InstancedStruct )
{
}

FArianeRectangle::FArianeRectangle( UArianeImage* InImage
                                  , const FName& InName
                                  , double InWidth
                                  , double InHeight
                                  , double InStrokeWidth
                                  , EArianeAllocationModel InAllocationModel )
    : FArianePrimitive( InImage
                      , InName
                      , InStrokeWidth
                      , InAllocationModel
                      , new FArianePrimitiveInvalidationFlags() )
    , Width( InWidth )
    , Height( InHeight )
{
    ResetGeometry();
    ReshapeGeometry();
}

bool
FArianeRectangle::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

void
FArianeRectangle::ResetGeometry()
{
    for( FArianeSegment* Segment : GeneratedSegments )
    {
        RemoveSegment( Segment ); // will auto unallocate
    }

    for( FArianeVertex* Vertex : GeneratedVertices )
    {
        RemoveVertex( Vertex ); // will auto unallocate
    }

    GeneratedVertices.Empty();
    GeneratedSegments.Empty();

    InvalidatedVertices.Empty();
    InvalidatedSegments.Empty();

    for( uint32 i = 0; i < 4; i++ )
    {
        // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
        // as instanced structs by the reflection system
        GeneratedVertices.Add( AllocVertex( FVector()
                                          , FVector::ZAxisVector
                                          , StrokeWidth
                                          , EArianeAllocationModel::OperatingSystem ) );

        AddVertex ( GeneratedVertices[i] );
    }

    for( uint32 i = 0; i < 4; i++ )
    {
        uint32 n = ( i + 1 ) % 4;

        // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
        // as instanced structs by the reflection system
        GeneratedSegments.Add( AllocCubicSegment( GeneratedVertices[i]
                                                , GeneratedVertices[i]->GetPosition()
                                                , GeneratedVertices[n]->GetPosition()
                                                , GeneratedVertices[n]
                                                , EArianeAllocationModel::OperatingSystem ) );

        AddSegment ( GeneratedSegments[i] );

        GeneratedVertices[i]->SetHandleAligned( false );
    }
}


void
FArianeRectangle::ReshapeGeometry()
{
    GeneratedVertices[0]->SetPosition( FVector( 0.0f , 0.0f  , 0.0f ) );
    GeneratedVertices[1]->SetPosition( FVector( Width, 0.0f  , 0.0f ) );
    GeneratedVertices[2]->SetPosition( FVector( Width, Height, 0.0f ) );
    GeneratedVertices[3]->SetPosition( FVector( 0.0f , Height, 0.0f ) );

    GeneratedSegments[0]->GetHandle((uint32)0)->SetPosition(  FVector( Width  * 0.25f, 0.0f          , 0.0f ) );
    GeneratedSegments[0]->GetHandle((uint32)1)->SetPosition(  FVector( Width  * 0.75f, 0.0f          , 0.0f ) );

    GeneratedSegments[1]->GetHandle((uint32)0)->SetPosition(  FVector( Width         , Height * 0.25f, 0.0f ) );
    GeneratedSegments[1]->GetHandle((uint32)1)->SetPosition(  FVector( Width         , Height * 0.75f, 0.0f ) );

    GeneratedSegments[2]->GetHandle((uint32)0)->SetPosition(  FVector( Width  * 0.75f, Height        , 0.0f ) );
    GeneratedSegments[2]->GetHandle((uint32)1)->SetPosition(  FVector( Width  * 0.25f, Height        , 0.0f ) );

    GeneratedSegments[3]->GetHandle((uint32)0)->SetPosition(  FVector( 0.0f          , Height * 0.75f, 0.0f ) );
    GeneratedSegments[3]->GetHandle((uint32)1)->SetPosition(  FVector( 0.0f          , Height * 0.25f, 0.0f ) );

    GeneratedVertices[0]->SetRadius( StrokeWidth );
    GeneratedVertices[1]->SetRadius( StrokeWidth );
    GeneratedVertices[2]->SetRadius( StrokeWidth );
    GeneratedVertices[3]->SetRadius( StrokeWidth );
}

FArianeObject*
FArianeRectangle::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeObject* RectangleCopy;

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::PrimitiveAsPath ) )
    {
        RectangleCopy = FArianePath::CopyShape( CopyArgs );
    }
    else
    {
         RectangleCopy = CopyArgs.Image->AllocRectangle( MaterialInterface
                                                       , Name
                                                       , Width
                                                       , Height
                                                       , StrokeWidth
                                                       , CopyArgs.AllocationModel );
    }

    return RectangleCopy;
}

void
FArianeRectangle::SetSize( double InWidth, double InHeight )
{
    Width = InWidth;
    Height = InHeight;

    ReshapeGeometry();
}

double
FArianeRectangle::GetWidth()
{
    return Width;
}

double
FArianeRectangle::GetHeight()
{
    return Height;
}
