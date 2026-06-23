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
    , Vertices { FArianeVertexID(), FArianeVertexID() }
    , Length ( 0.0f )
    , bInvalidated ( false )
    , bAutoFractioned ( true )
    , AllocationModel( EArianeAllocationModel::InstancedStruct )
{
}

FArianeSegment::FArianeSegment( FArianeObject* Owner
                              , FArianeVertex* InVertex0
                              , FArianeVertex* InVertex1
                              , EArianeAllocationModel InAllocationModel )
    : Guid( FGuid::NewGuid() )
    , OwnerID( Owner )
    , Vertices { InVertex0, InVertex1 }
    , Length ( 0.0f )
    , bInvalidated ( false )
    , bAutoFractioned ( true )
    , AllocationModel( InAllocationModel )
{
    Init();
}

EArianeAllocationModel
FArianeSegment::GetAllocationModel()
{
    return AllocationModel;
}

bool
FArianeSegment::HasBaseClass( uint32 BaseClassID )
{
    if( StaticClass() == BaseClassID )
    {
        return true;
    }

    return false;
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

void
FArianeSegment::AllocateCache( uint32 VertexCount, uint32 TriangleCount )
{
    ModelVertexCache.SetNumZeroed( VertexCount );
    IndexCache.SetNumZeroed( TriangleCount * 3 );
}

const TArray<FArianeSegment::FFraction>&
FArianeSegment::GetFractions()
{
    return Fractions;
}

const TArray<FArianeSegment::FFractionStep>&
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
            FFractionStep& Step = FractionSteps[i];
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
    if( bAutoFractioned )
    {
        Length = ( GetVertex(1)->GetPosition() - GetVertex(0)->GetPosition() ).Length();
    }

    UpdateBounds();

    bInvalidated = false;
}

void
FArianeSegment::SetAutoFractioned( bool bInAutoFractioned )
{
    bAutoFractioned = bInAutoFractioned;

    Invalidate();
}

bool
FArianeSegment::IsAutoFractioned()
{
    return bAutoFractioned;
}

void
FArianeSegment::SetFractions( const TArray<FArianePoint>& InFractionPoints
                            , const TArray<float>& Radii )
{
    FractionPoints.Empty();
    FractionSteps.Empty();
    Fractions.Empty();
    Length = 0.0f;

    if( InFractionPoints.Num() >= 2 )
    {
        uint32 FractionStepCount = InFractionPoints.Num();

        FractionPoints = InFractionPoints;
        FractionSteps.Reserve( FractionStepCount );
        Fractions.Reserve( FractionStepCount - 1 );

        FFractionStep* Step0 = &FractionSteps.Emplace_GetRef( Vertices[0].GetVertex()
                                                            , 0
                                                            , Vertices[0].GetVertex()->GetRadius() );

        Vertices[0].GetVertex()->SetPosition( FractionPoints[0].GetPosition() );

        for( int32 i = 1, n = 2; i < FractionPoints.Num() - 1; i++, n++ )
        {
            FFractionStep* Step1 = &FractionSteps.Emplace_GetRef ( &FractionPoints[i], ( float ) n / FractionStepCount, Radii[i] );

            Length += Fractions.Emplace_GetRef( Step0, Step1 ).Length;

            Step0 = Step1;
        }

        FFractionStep* Step1 = &FractionSteps.Emplace_GetRef( Vertices[1].GetVertex()
                                                            , 1
                                                            , Vertices[1].GetVertex()->GetRadius() );

        Vertices[1].GetVertex()->SetPosition( FractionPoints.Last().GetPosition() );

        Length += Fractions.Emplace_GetRef( Step0, Step1 ).Length;
    }

    Invalidate();
}

bool
FArianeSegment::IsInvalidated()
{
    return bInvalidated;
}

void
FArianeSegment::Invalidate()
{
    if( OwnerID.GetObject() && ( bInvalidated == false ) )
    {
        if( OwnerID.GetObject()->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>( OwnerID.GetObject() );

            Path->InvalidateSegment( this );

            bInvalidated = true;
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

double
FArianeSegment::GetLength()
{
    return Length;
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
