// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianePolygon.h"
#include "ArianeVertex.h"
#include "ArianeSegmentCubic.h"
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

FArianePolygon::~FArianePolygon()
{
   // vertices ans segments freed in ArianePath::~destructor
}

FArianePolygon::FArianePolygon()
    : FArianePolygon( nullptr
                    , FName ( "Ariane Polygon" )
                    , 3
                    , 0.0f
                    , 0.0f
                    , EArianeAllocationModel::InstancedStruct )
{
}

FArianePolygon::FArianePolygon( UArianeImage* InImage
                              , const FName& InName
                              , uint32 InCornerCount
                              , double InRadius
                              , double InStrokeWidth
                              , EArianeAllocationModel InAllocationModel )
    : FArianePrimitive( InImage
                      , InName
                      , InStrokeWidth
                      , InAllocationModel
                      , new FArianePrimitiveInvalidationFlags() )
    , CornerCount( InCornerCount < 3 ? 3 : InCornerCount )
    , Radius ( InRadius )
{
    ResetGeometry();
    ReshapeGeometry();
}

bool
FArianePolygon::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

void
FArianePolygon::ResetGeometry()
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

    for( uint32 i = 0; i < CornerCount; i++ )
    {
        // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
        // as instanced structs by the reflection system
        GeneratedVertices.Add( AllocVertex( FVector()
                                          , FVector::ZAxisVector
                                          , StrokeWidth
                                          , EArianeAllocationModel::OperatingSystem ) );

        AddVertex ( GeneratedVertices[i] );

        //GeneratedVertices[i]->SetHandleAligned( false );
    }

    for( uint32 i = 0; i < CornerCount; i++ )
    {
        uint32 n = ( i + 1 ) % CornerCount;

        // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
        // as instanced structs by the reflection system
        GeneratedSegments.Add( AllocCubicSegment( GeneratedVertices[i]
                                                , GeneratedVertices[i]->GetPosition()
                                                , GeneratedVertices[n]->GetPosition()
                                                , GeneratedVertices[n]
                                                , EArianeAllocationModel::OperatingSystem ) );

        AddSegment ( GeneratedSegments[i] );
    }
}

void
FArianePolygon::ReshapeGeometry()
{
    double Angle = 0.0f;
    double StepAngle = ( PI * 2 ) / CornerCount;

    for( uint32 i = 0; i < CornerCount; i++ )
    {
        FVector VertexPosition = FVector( FMath::Cos( Angle ) * Radius
                                        , FMath::Sin( Angle ) * Radius
                                        , 0.0f );

        GeneratedVertices[i]->SetPosition( VertexPosition );
        GeneratedVertices[i]->SetRadius( StrokeWidth );



        Angle += StepAngle;
    }

    for( uint32 i = 0; i < CornerCount; i++ )
    {
        GeneratedSegments[i]->GetHandle((uint32)0)->SetPosition( GeneratedSegments[i]->GetVertex(0)->GetPosition() );
        GeneratedSegments[i]->GetHandle((uint32)1)->SetPosition( GeneratedSegments[i]->GetVertex(1)->GetPosition() );
    }
}

FArianeObject*
FArianePolygon::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeObject* PolygonCopy;

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::PrimitiveAsPath ) )
    {
        PolygonCopy = FArianePath::CopyShape( CopyArgs );
    }
    else
    {
         PolygonCopy = CopyArgs.Image->AllocPolygon( MaterialInterface
                                                   , Name
                                                   , CornerCount
                                                   , Radius
                                                   , StrokeWidth
                                                   , CopyArgs.AllocationModel );
    }

    return PolygonCopy;
}

void
FArianePolygon::SetCornerCount( uint32 InCornerCount )
{
    CornerCount = InCornerCount;

    ResetGeometry();
    ReshapeGeometry();
}

uint32
FArianePolygon::GetCornerCount()
{
    return CornerCount;
}

void
FArianePolygon::SetRadius( double InRadius )
{
    Radius = InRadius;

    ReshapeGeometry();
}

double
FArianePolygon::GetRadius()
{
    return Radius;
}
