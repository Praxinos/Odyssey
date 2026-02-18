// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeSegment.h"
#include "ArianeVertex.h"

FArianeSegment::~FArianeSegment()
{
}

FArianeSegment::FArianeSegment( FArianeVertex* iVertex0, FArianeVertex* iVertex1 )
    : Vertices { iVertex0, iVertex1 }
    , Length ( 0.0f )
{
    FractionCache.Emplace( iVertex0, iVertex1 );

    FractionPointsT.Add( 0.0f );
    FractionPointsT.Add( 1.0f );

    FractionPoints.Add( iVertex0 );
    FractionPoints.Add( iVertex1 );
}

float
FArianeSegment::GetFractionPointT( uint32 FractionPointIndex )
{
    return FractionPointsT[FractionPointIndex];
}

void
FArianeSegment::SetOwner( FArianeObject* iOwner )
{
    Owner = iOwner;
}

void
FArianeSegment::Link()
{
    Vertices[0]->AddSegment( this );
    Vertices[1]->AddSegment( this );
}

void
FArianeSegment::Unlink()
{
    Vertices[0]->RemoveSegment( this );
    Vertices[1]->RemoveSegment( this );
}

FArianeVertex*
FArianeSegment::GetVertex( uint32 Index )
{
    return Vertices[Index];
}

FVector
FArianeSegment::GetTangentVectorAt( double T, bool bNormalize )
{
    FVector RetVector = ( Vertices[1]->GetPosition() - Vertices[0]->GetPosition() );

    if( bNormalize && ( RetVector.SquaredLength() != 0.0f ) )
    {
        RetVector.Normalize();
    }

    return RetVector;
}

void
FArianeSegment::AllocateCache( uint32 VertexCount, uint32 TriangleCount )
{
    ModelVertexCache.SetNum( VertexCount );
    IndexCache.SetNum( TriangleCount * 3 );
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

    if( Vertex == Vertices[0] )
    {
        RetVector = Vertices[1]->GetPosition() - Vertices[0]->GetPosition();
    }

    if( Vertex == Vertices[1] )
    {
        RetVector = Vertices[0]->GetPosition() - Vertices[1]->GetPosition();
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
    if( ( Vertices[0] != Vertex ) && ( Vertices[1] != Vertex )  )
    {
        return nullptr;
    }

    return ( Vertices[0] == Vertex ) ? Vertices[1] : Vertices[0];
}

FVector
FArianeSegment::GetAverageVectorAt( double T )
{
    FVector AverageVector = FVector( 0.0f, 0.0f, 0.0f );

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = T == 0.0f ? Vertices[0] : Vertices[1];

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
        for( FModelVertex ModelVertex : ModelVertexCache )
        {
            if( ModelVertex.Position.X < Min.X ) Min.X = ModelVertex.Position.X;
            if( ModelVertex.Position.Y < Min.Y ) Min.Y = ModelVertex.Position.Y;
            if( ModelVertex.Position.Z < Min.Z ) Min.Z = ModelVertex.Position.Z;
            if( ModelVertex.Position.X > Max.X ) Max.X = ModelVertex.Position.X;
            if( ModelVertex.Position.Y > Max.Y ) Max.Y = ModelVertex.Position.Y;
            if( ModelVertex.Position.Z > Max.Z ) Max.Z = ModelVertex.Position.Z;
        }

        Bounds.Origin = ( Min + Max ) * 0.5f;
        Bounds.BoxExtent = ( Max - Bounds.Origin );
        Bounds.SphereRadius = Bounds.BoxExtent.Length();
    }
}

void
FArianeSegment::Update()
{
    Length = ( GetVertex(1)->GetPosition() - GetVertex(0)->GetPosition() ).Length();

    UpdateBounds();
}
