// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeObject.h"

FArianeSegment::~FArianeSegment()
{
}

FArianeSegment::FArianeSegment( )
    : Guid( FGuid::NewGuid() )
    , OwnerID()
    , Length ( 0.0f )
{
}

FArianeSegment::FArianeSegment( FArianeObject* Owner, FArianeVertex* iVertex0, FArianeVertex* iVertex1 )
    : Guid( FGuid::NewGuid() )
    , OwnerID( Owner )
    , Vertices { iVertex0, iVertex1 }
    , Length ( 0.0f )
{
    Init();
}

float
FArianeSegment::GetFractionPointT( uint32 FractionPointIndex )
{
    return FractionPointsT[FractionPointIndex];
}

FArianeObject*
FArianeSegment::GetOwner()
{
    return OwnerID.GetObject();
}

void
FArianeSegment::Link()
{
    Vertices[0].GetVertex()->AddSegment( this );
    Vertices[1].GetVertex()->AddSegment( this );
}

void
FArianeSegment::Unlink()
{
    Vertices[0].GetVertex()->RemoveSegment( this );
    Vertices[1].GetVertex()->RemoveSegment( this );
}

FArianeVertex*
FArianeSegment::GetVertex( uint32 Index )
{
    return Vertices[Index].GetVertex();
}

FVector
FArianeSegment::GetTangentVectorAt( double T, bool bNormalize )
{
    FVector RetVector = ( Vertices[1].GetVertex()->GetPosition() - Vertices[0].GetVertex()->GetPosition() );

    if( bNormalize && ( RetVector.SquaredLength() != 0.0f ) )
    {
        RetVector.Normalize();
    }

    return RetVector;
}

void
FArianeSegment::AllocateCache( uint32 VertexCount, uint32 TriangleCount )
{
    ModelVertexCache.SetNumZeroed( VertexCount );
    IndexCache.SetNumZeroed( TriangleCount * 3 );
}

const TArray<FArianeSegment::Fraction>&
FArianeSegment::GetFractionCache()
{
    return FractionCache;
}

const TArray<FArianePoint*>&
FArianeSegment::GetFractionPoints()
{
    return FractionPoints;
}

uint32
FArianeSegment::GetFractionCount()
{
    return GetFractionCache().Num();
}

const TArray<FModelVertex>&
FArianeSegment::GetModelVertexCache()
{
    return ModelVertexCache;
}

const TArray<uint32>&
FArianeSegment::GetIndexCache()
{
    return IndexCache;
}

FVector
FArianeSegment::GetVectorLeavingFromVertex( FArianeVertex* Vertex, bool bNormalize )
{
    FVector RetVector;

    if( Vertex == Vertices[0].GetVertex() )
    {
        RetVector = Vertices[1].GetVertex()->GetPosition() - Vertices[0].GetVertex()->GetPosition();
    }

    if( Vertex == Vertices[1].GetVertex() )
    {
        RetVector = Vertices[0].GetVertex()->GetPosition() - Vertices[1].GetVertex()->GetPosition();
    }

    if( bNormalize && ( RetVector.SquaredLength() != 0.0f ) )
    {
        RetVector.Normalize();
    }

    return RetVector;
}

FArianeVertex*
FArianeSegment::GetOtherVertex( FArianeVertex* Vertex )
{
    if( ( Vertices[0].GetVertex() != Vertex ) && ( Vertices[1].GetVertex() != Vertex )  )
    {
        return nullptr;
    }

    return ( Vertices[0].GetVertex() == Vertex ) ? Vertices[1].GetVertex() : Vertices[0].GetVertex();
}

FVector
FArianeSegment::GetAverageVectorAt( double T )
{
    FVector AverageVector = FVector( 0.0f, 0.0f, 0.0f );

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = T == 0.0f ? Vertices[0].GetVertex() : Vertices[1].GetVertex();

        if( Vertex->GetSegments().Num() )
        {
            for( FArianeSegment* Segment : Vertex->GetSegments() )
            {
                AverageVector += Segment->GetVectorLeavingFromVertex( Vertex, true );
            }

            AverageVector /= Vertex->GetSegments().Num();
        }

        if( AverageVector.SquaredLength() != 0.0f )
        {
            AverageVector.Normalize( 1.0f );
        }
    }

    return AverageVector;
}

const FBoxSphereBounds&
FArianeSegment::GetBounds()
{
    return Bounds;
}

void
FArianeSegment::UpdateBounds()
{
    FVector Min = FVector (  DBL_MAX,  DBL_MAX,  DBL_MAX );
    FVector Max = FVector ( -DBL_MAX, -DBL_MAX, -DBL_MAX );

    Bounds = FBoxSphereBounds();

    if( Length )
    {
        for( int32 i = 0; i < FractionPoints.Num(); i++ )
        {
            FArianePoint* FractionPoint = FractionPoints[i];
            float FractionPointRadius = FractionPointsRadius[i];
            const FVector FractionPointPosition = FractionPoint->GetPosition();
            FVector FractionPointMax = FVector( FractionPointPosition.X + FractionPointRadius
                                              , FractionPointPosition.Y + FractionPointRadius
                                              , FractionPointPosition.Z + FractionPointRadius );
            FVector FractionPointMin = FVector( FractionPointPosition.X - FractionPointRadius
                                              , FractionPointPosition.Y - FractionPointRadius
                                              , FractionPointPosition.Z - FractionPointRadius );

            if( FractionPointMin.X < Min.X ) Min.X = FractionPointMin.X;
            if( FractionPointMin.Y < Min.Y ) Min.Y = FractionPointMin.Y;
            if( FractionPointMin.Z < Min.Z ) Min.Z = FractionPointMin.Z;
            if( FractionPointMax.X > Max.X ) Max.X = FractionPointMax.X;
            if( FractionPointMax.Y > Max.Y ) Max.Y = FractionPointMax.Y;
            if( FractionPointMax.Z > Max.Z ) Max.Z = FractionPointMax.Z;
        }

        Bounds.Origin = ( Min + Max ) * 0.5f;
        Bounds.BoxExtent = ( Max - Bounds.Origin );
        Bounds.SphereRadius = Bounds.BoxExtent.Length();
    }
}

void
FArianeSegment::Update()
{
    Init();

    Length = ( GetVertex(1)->GetPosition() - GetVertex(0)->GetPosition() ).Length();

    UpdateBounds();
}

void
FArianeSegment::Init()
{
    FractionCache.Empty();
    FractionPoints.Empty();
    FractionPointsT.Empty();

    FractionCache.Emplace( Vertices[0].GetVertex(), Vertices[1].GetVertex() );

    FractionPointsT.Add( 0.0f );
    FractionPointsT.Add( 1.0f );

    FractionPointsRadius.Add( Vertices[0].GetVertex()->GetRadius() );
    FractionPointsRadius.Add( Vertices[1].GetVertex()->GetRadius() );

    FractionPoints.Add( Vertices[0].GetVertex() );
    FractionPoints.Add( Vertices[1].GetVertex() );
}
