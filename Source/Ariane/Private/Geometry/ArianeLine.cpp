// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeLine.h"
#include "ArianeVertex.h"
#include "ArianeSegmentCubic.h"
#include "ArianeImage.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

FArianeLine::~FArianeLine()
{
   // vertices ans segments freed in ArianePath::~destructor
}

FArianeLine::FArianeLine()
    : FArianeLine( nullptr
                 , FName ( "Ariane Line" )
                 , FVector()
                 , FVector()
                 , 0.0f
                 , EArianeAllocationModel::InstancedStruct )
{
}

FArianeLine::FArianeLine( UArianeImage* InImage
                        , const FName& InName
                        , const FVector& InStartPoint
                        , const FVector& InEndPoint
                        , double InStrokeWidth
                        , EArianeAllocationModel InAllocationModel )
    : FArianePrimitive( InImage
                      , InName
                      , InStrokeWidth
                      , InAllocationModel
                      , new FArianePrimitiveInvalidationFlags() )
    , StartPoint( InStartPoint )
    , EndPoint( InEndPoint )
{
    ResetGeometry();
    ReshapeGeometry();
}

bool
FArianeLine::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClassID );
}

void
FArianeLine::ResetGeometry()
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

    // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
    // as instanced structs by the reflection system
    GeneratedVertices.Add( AllocVertex( FVector(), FVector::ZAxisVector, StrokeWidth, EArianeAllocationModel::OperatingSystem ) );
    GeneratedVertices.Add( AllocVertex( FVector(), FVector::ZAxisVector, StrokeWidth, EArianeAllocationModel::OperatingSystem ) );
    AddVertex ( GeneratedVertices[0] );
    AddVertex ( GeneratedVertices[1] );

    // EArianeAllocationModel::OperatingSystem means we use "new" to alloc the vertices. they won't be saved
    // as instanced structs by the reflection system
    GeneratedSegments.Add( AllocCubicSegment( GeneratedVertices[0]
                                            , GeneratedVertices[0]->GetPosition()
                                            , GeneratedVertices[1]->GetPosition()
                                            , GeneratedVertices[1]
                                            , EArianeAllocationModel::OperatingSystem ) );
    AddSegment ( GeneratedSegments[0] );

    GeneratedVertices[0]->SetHandleAligned( false );
    GeneratedVertices[1]->SetHandleAligned( false );
}


void
FArianeLine::ReshapeGeometry()
{
    GeneratedVertices[0]->SetPosition( StartPoint );
    GeneratedVertices[1]->SetPosition( EndPoint );

    GeneratedSegments[0]->GetHandle((uint32)0)->SetPosition(  StartPoint );
    GeneratedSegments[0]->GetHandle((uint32)1)->SetPosition(  EndPoint );

    GeneratedVertices[0]->SetRadius( StrokeWidth );
    GeneratedVertices[1]->SetRadius( StrokeWidth );
}

FArianeObject*
FArianeLine::CopyShape( const FCopyArgs& CopyArgs )
{
    FArianeObject* LineCopy;

    if( EnumHasAllFlags( CopyArgs.Flags, ECopyFlags::PrimitiveAsPath ) )
    {
        LineCopy = FArianePath::CopyShape( CopyArgs );
    }
    else
    {
         LineCopy = CopyArgs.Image->AllocLine( MaterialInterface
                                             , Name
                                             , StartPoint
                                             , EndPoint
                                             , StrokeWidth
                                             , CopyArgs.AllocationModel );
    }

    return LineCopy;
}

void
FArianeLine::SetPoints( const FVector& InStartPoint, const FVector& InEndPoint )
{
    StartPoint = InStartPoint;
    EndPoint = InEndPoint;

    ReshapeGeometry();
}

void
FArianeLine::SetStartPoint( const FVector& InStartPoint  )
{
    StartPoint = InStartPoint;

    ReshapeGeometry();
}

void
FArianeLine::SetEndPoint( const FVector& InEndPoint  )
{
    EndPoint = InEndPoint;

    ReshapeGeometry();
}

FVector
FArianeLine::GetStartPoint()
{
    return StartPoint;
}

FVector
FArianeLine::GetEndPoint()
{
    return EndPoint;
}

double
FArianeLine::GetStrokeWidth()
{
    return StrokeWidth;
}
