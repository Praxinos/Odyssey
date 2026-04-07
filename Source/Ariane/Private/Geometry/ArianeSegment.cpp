// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeObject.h"
#include "ArianePath.h"

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

void
FArianeSegment::PostLoad()
{
    Init();
}

void
FArianeSegment::PostEditUndo()
{
    Init();
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
FArianeSegment::GetFractions()
{
    return Fractions;
}

const TArray<FArianeSegment::FractionStep>&
FArianeSegment::GetFractionSteps()
{
    return FractionSteps;
}

uint32
FArianeSegment::GetFractionCount()
{
    return Fractions.Num();
}

const TArray<FDynamicMeshVertex>&
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

    Bounds = FBoxSphereBounds(ForceInit);

    if( Length )
    {
        for( int32 i = 0; i < FractionSteps.Num(); i++ )
        {
            FractionStep& Step = FractionSteps[i];
            FArianePoint* Point = Step.Point;
            float PointRadius = Step.Radius;
            const FVector PointPosition = Point->GetPosition();
            FVector PointMax = FVector( PointPosition.X + PointRadius
                                      , PointPosition.Y + PointRadius
                                      , PointPosition.Z + PointRadius );
            FVector PointMin = FVector( PointPosition.X - PointRadius
                                      , PointPosition.Y - PointRadius
                                      , PointPosition.Z - PointRadius );

            if( PointMin.X < Min.X ) Min.X = PointMin.X;
            if( PointMin.Y < Min.Y ) Min.Y = PointMin.Y;
            if( PointMin.Z < Min.Z ) Min.Z = PointMin.Z;
            if( PointMax.X > Max.X ) Max.X = PointMax.X;
            if( PointMax.Y > Max.Y ) Max.Y = PointMax.Y;
            if( PointMax.Z > Max.Z ) Max.Z = PointMax.Z;
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

void
FArianeSegment::Invalidate()
{
    if( OwnerID.GetObject() )
    {
        if( OwnerID.GetObject()->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>( OwnerID.GetObject() );

            Path->InvalidateSegment( this );
        }
    }
}

void
FArianeSegment::Init()
{
    Fractions.Empty();
    FractionSteps.Empty();

    FractionSteps.Emplace( Vertices[0].GetVertex(), 0.0f, Vertices[0].GetVertex()->GetRadius() );
    FractionSteps.Emplace( Vertices[1].GetVertex(), 1.0f, Vertices[1].GetVertex()->GetRadius() );

    Fractions.Emplace( &FractionSteps[0], &FractionSteps[1] );

    Invalidate();
}

const FGuid&
FArianeSegment::GetGuid()
{
    return Guid;
}

FVector
FArianeSegment::GetPointAt( double T )
{
    FVector V0Coords = GetVertex(0)->GetPosition();
    FVector V1Coords = GetVertex(1)->GetPosition();

    return V0Coords + ( V1Coords - V0Coords ) * T;
}

FVector
FArianeSegment::GetNormalAt( double T )
{
    FVector V0Coords = GetVertex(0)->GetNormal();
    FVector V1Coords = GetVertex(1)->GetNormal();

    return V0Coords + ( V1Coords - V0Coords ) * T;
}

FArianeSegment*
FArianeSegment::Extract( FArianeObject* NewSegmentOwner
                       , FArianeVertex* NewSegmentVertex0
                       , float T0
                       , FArianeVertex* NewSegmentVertex1
                       , float T1 )
{
    FVector DeltaPosition = GetVertex(1)->GetPosition() - GetVertex(0)->GetPosition();
    FVector DeltaNormal = GetVertex(1)->GetNormal() - GetVertex(0)->GetNormal();
    double DeltaRadius = GetVertex(1)->GetRadius() - GetVertex(0)->GetRadius();

    NewSegmentVertex0->SetPosition( GetVertex(0)->GetPosition() + DeltaPosition * T0 );
    NewSegmentVertex0->SetNormal( GetVertex(0)->GetNormal() + DeltaNormal * T0 );
    NewSegmentVertex0->SetRadius( GetVertex(0)->GetRadius() + DeltaRadius * T0 );

    NewSegmentVertex1->SetPosition( GetVertex(0)->GetPosition() + DeltaPosition * T1 );
    NewSegmentVertex1->SetNormal( GetVertex(0)->GetNormal() + DeltaNormal * T1 );
    NewSegmentVertex1->SetRadius( GetVertex(0)->GetRadius() + DeltaRadius * T1 );

    if( NewSegmentOwner->GetClass() == FArianePath::StaticClass() )
    {
        FArianePath* Path = static_cast<FArianePath*>( NewSegmentOwner );

        return Path->AllocSegment( NewSegmentVertex0, NewSegmentVertex1 );
    }

    return nullptr;
}
