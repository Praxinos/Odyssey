// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeEllipse.h"
#include "ArianeVertex.h"
#include "ArianeSegmentCubic.h"
#include "ArianeLayerDrawing.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

FArianeEllipse::~FArianeEllipse()
{
   // vertices ans segments freed in ArianePath::~destructor
}

FArianeEllipse::FArianeEllipse()
    : FArianeEllipse( nullptr
                    , FName ( "Ariane Ellipse" )
                    , 0.0f
                    , 0.0f
                    , 0.0f
                    , EArianeAllocationModel::InstancedStruct )
{
}

FArianeEllipse::FArianeEllipse( UArianeLayerDrawing* InDrawingLayer
                              , const FName& InName
                              , double InRadiusX
                              , double InRadiusY
                              , double InStrokeWidth
                              , EArianeAllocationModel InAllocationModel )
    : FArianePrimitive( InDrawingLayer
                      , InName
                      , InStrokeWidth
                      , InAllocationModel
                      , new FArianePrimitiveInvalidationFlags() )
    , RadiusX( InRadiusX )
    , RadiusY( InRadiusY )
{
    ResetGeometry();
    ReshapeGeometry();
}

bool
FArianeEllipse::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

void
FArianeEllipse::ResetGeometry()
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

        GeneratedVertices[i]->SetHandleAligned( true );

        AddSegment ( GeneratedSegments[i] );
    }
}

void
FArianeEllipse::ReshapeGeometry()
{
    double ctlDistX = RadiusX * MAGICRATIO;
    double ctlDistY = RadiusY * MAGICRATIO;

    GeneratedVertices[0]->SetRadius( StrokeWidth );
    GeneratedVertices[1]->SetRadius( StrokeWidth );
    GeneratedVertices[2]->SetRadius( StrokeWidth );
    GeneratedVertices[3]->SetRadius( StrokeWidth );

    GeneratedVertices[0]->SetPosition( FVector(  0.0f   ,  RadiusY, 0.0f ) );
    GeneratedVertices[1]->SetPosition( FVector(  RadiusX,  0.0f   , 0.0f ) );
    GeneratedVertices[2]->SetPosition( FVector(  0.0f   , -RadiusY, 0.0f ) );
    GeneratedVertices[3]->SetPosition( FVector( -RadiusX,  0.0f   , 0.0f ) );

    GeneratedSegments[0]->GetHandle((uint32)0)->SetPosition( FVector( ctlDistX,  RadiusY, 0.0f ) );
    GeneratedSegments[0]->GetHandle((uint32)1)->SetPosition( FVector( RadiusX,  ctlDistY, 0.0f ) );

    GeneratedSegments[1]->GetHandle((uint32)0)->SetPosition( FVector( RadiusX, -ctlDistY, 0.0f ) );
    GeneratedSegments[1]->GetHandle((uint32)1)->SetPosition( FVector( ctlDistX, -RadiusY, 0.0f ) );

    GeneratedSegments[2]->GetHandle((uint32)0)->SetPosition( FVector( -ctlDistX, -RadiusY, 0.0f ) );
    GeneratedSegments[2]->GetHandle((uint32)1)->SetPosition( FVector( -RadiusX, -ctlDistY, 0.0f ) );

    GeneratedSegments[3]->GetHandle((uint32)0)->SetPosition( FVector( -RadiusX,  ctlDistY, 0.0f ) );
    GeneratedSegments[3]->GetHandle((uint32)1)->SetPosition( FVector( -ctlDistX,  RadiusY, 0.0f ) );
}

FArianeObject*
FArianeEllipse::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeObject* EllipseCopy;

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::PrimitiveAsPath ) )
    {
        EllipseCopy = FArianePath::CopyShape( CopyArgs );
    }
    else
    {
         EllipseCopy = CopyArgs.DrawingLayer->AllocEllipse( MaterialInterface
                                                          , Name
                                                          , RadiusX
                                                          , RadiusY
                                                          , StrokeWidth
                                                          , CopyArgs.AllocationModel );
    }

    return EllipseCopy;
}

void
FArianeEllipse::SetRadius( double InRadius )
{
    SetRadius( InRadius, InRadius );
}

void
FArianeEllipse::SetRadius( double InRadiusX, double InRadiusY )
{
    RadiusX = InRadiusX;
    RadiusY = InRadiusY;

    ReshapeGeometry();
}

double
FArianeEllipse::GetRadiusX()
{
    return RadiusX;
}

double
FArianeEllipse::GetRadiusY()
{
    return RadiusY;
}
