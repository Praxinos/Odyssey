// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeSegment.h"
#include "ArianePainting3DComponent.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawIndexBuffer.h"
#include "Materials/MaterialRenderProxy.h"

bool
FArianePathInvalidationFlags::HasBaseClass( uint32 BaseClass ) const
{
    if( StaticClass() == BaseClass )
    {
        return true;
    }

    return Super::HasBaseClass( BaseClass );
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::AND( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianePathInvalidationFlags::StaticClass() ) )
    {
        VertexGeometry  &= ((FArianePathInvalidationFlags&)RHS).VertexGeometry;
        SegmentGeometry &= ((FArianePathInvalidationFlags&)RHS).SegmentGeometry;
        VertexTopology  &= ((FArianePathInvalidationFlags&)RHS).VertexTopology;
        SegmentTopology &= ((FArianePathInvalidationFlags&)RHS).SegmentTopology;
    }

    Super::AND( RHS );

    return *this;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::OR( const FArianeObjectInvalidationFlags& RHS )
{
    if( RHS.HasBaseClass( FArianePathInvalidationFlags::StaticClass() ) )
    {
        VertexGeometry  |= ((FArianePathInvalidationFlags&)RHS).VertexGeometry;
        SegmentGeometry |= ((FArianePathInvalidationFlags&)RHS).SegmentGeometry;
        VertexTopology  |= ((FArianePathInvalidationFlags&)RHS).VertexTopology;
        SegmentTopology |= ((FArianePathInvalidationFlags&)RHS).SegmentTopology;
    }

    Super::OR( RHS );

    return *this;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::SetAll()
{
    VertexGeometry  =
    SegmentGeometry =
    VertexTopology  =
    SegmentTopology = 1;

    Super::SetAll();

    return *this;
}

void
FArianePathInvalidationFlags::ClearOwn( FArianePathInvalidationFlags& Flags )
{
    Flags.VertexGeometry  =
    Flags.SegmentGeometry =
    Flags.VertexTopology  =
    Flags.SegmentTopology = 0;
}

FArianePathInvalidationFlags&
FArianePathInvalidationFlags::Clear()
{
    FArianePathInvalidationFlags::ClearOwn( *this );

    Super::Clear();

    return *this;
}

bool
FArianePathInvalidationFlags::HasAny()
{
    return ( VertexGeometry
          || SegmentGeometry
          || VertexTopology
          || SegmentTopology ) ? true : Super::HasAny();
}

FArianePath::~FArianePath()
{
    //UE_LOG( LogTemp, Warning, TEXT("FArianePath::DTOR %llu"), (uint64*)this );
}

FArianePath::FArianePath()
    : FArianeObject()
    , Geometry3D ( this )
{
    InvalidationFlags = new FArianePathInvalidationFlags();

    //UE_LOG( LogTemp, Warning, TEXT("Default FArianePath::CTOR %llu"), (uint64*)this );
}

FArianePath::FArianePath( UArianePainting3DComponent* InPainting3DComponent )
    : FArianeObject ( InPainting3DComponent )
    , Geometry3D ( this )
{
    InvalidationFlags = new FArianePathInvalidationFlags();
}

/*
void
FArianePath::PostLoad()
{
    //if( Geometry3D == nullptr )
    {
        //Geometry3D = new FArianePathGeometry3D( this );

        InvalidationFlags = new FArianePathInvalidationFlags();

        Material = NewObject<UMaterial>();
    }
}
*/

FArianeVertex*
FArianePath::AllocVertex( const FVector& iPosition, const FVector& InNormal, double InRadius )
{
    InstancedVertices.Push( FInstancedStruct::Make<FArianeVertex>( this, iPosition, InNormal, InRadius ) );

    FArianeVertex* NewVertex = InstancedVertices.Last().GetMutablePtr<FArianeVertex>();

    return NewVertex;
}

void
FArianePath::RemoveVertex( FArianeVertex* iVertex, bool bRemoveFromInstancedVertices )
{
    Vertices.RemoveAll( [iVertex]( FArianeVertexID& VertexID ) -> bool
    {
        return ( iVertex == VertexID.GetVertex() ) ? true : false;
    } );

    Invalidate( FArianePathInvalidationFlags().SetVertexTopology() );

    if( bRemoveFromInstancedVertices )
    {
        InstancedVertices.RemoveAll( [iVertex]( FInstancedStruct& Struct ) -> bool
        {
            return ( iVertex == Struct.GetPtr<FArianeVertex>() ) ? true : false;
        } );
    }
}

FArianeVertex*
FArianePath::GetVertexByGuid( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedVertex : InstancedVertices )
    {
        FArianeVertex* Vertex = InstancedVertex.GetMutablePtr<FArianeVertex>();

        if( Vertex->Guid == InGuid )
        {
            return Vertex;
        }
    }

    return nullptr;
}

FArianeSegment*
FArianePath::GetSegmentByGuid( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        if( Segment->Guid == InGuid )
        {
            return Segment;
        }
    }

    return nullptr;
}

FArianeSegment*
FArianePath::AllocSegment( FArianeVertex* Vertex0, FArianeVertex* Vertex1 )
{
    InstancedSegments.Push( FInstancedStruct::Make<FArianeSegment>( this, Vertex0, Vertex1 ) );

    FArianeSegment* NewSegment = InstancedSegments.Last().GetMutablePtr<FArianeSegment>();

    return NewSegment;
}

void
FArianePath::AddVertex( FArianeVertex* Vertex )
{
    Vertices.Add( FArianeVertexID( Vertex ) );

    Invalidate( FArianePathInvalidationFlags().SetVertexTopology() );
}

void
FArianePath::AddSegment( FArianeSegment* Segment )
{
    Segment->Link();

    Segments.Add( FArianeSegmentID( Segment ) );
    InvalidatedSegments.Add( Segment );

    Invalidate( FArianePathInvalidationFlags().SetSegmentTopology() );
}

void
FArianePath::RemoveSegment( FArianeSegment* Segment, bool bRemoveFromInstancedSegments )
{
    Segments.RemoveAll( [Segment]( FArianeSegmentID& SegmentID ) -> bool
    {
        return ( Segment == SegmentID.GetSegment() ) ? true : false;
    } );

    InvalidatedSegments.Remove( Segment );

    Segment->Unlink();

    Invalidate( FArianePathInvalidationFlags().SetSegmentTopology() );

    if( bRemoveFromInstancedSegments )
    {
        InstancedSegments.RemoveAll( [Segment]( const FInstancedStruct& Struct ) -> bool
        {
            return ( Segment == Struct.GetPtr<FArianeSegment>() ) ? true : false;
        } );
    }
}

TArray<FArianeSegmentID>&
FArianePath::GetSegments()
{
    return Segments;
}

TArray<FArianeSegment*>&
FArianePath::GetInvalidatedSegments()
{
    return InvalidatedSegments;
}

TArray<FArianeVertexID>&
FArianePath::GetVertices()
{
    return Vertices;
}

void
FArianePath::UpdateBounds()
{
    Bounds = FBoxSphereBounds();

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        Bounds = Bounds + Segment->GetBounds();
    }
}

void
FArianePath::InvalidateSegment( FArianeSegment* Segment )
{
    InvalidatedSegments.Add( Segment );
}

void
FArianePath::PostEditUndo()
{
    InvalidatedSegments.Empty();

    for( FInstancedStruct& InstancedSegment : InstancedSegments )
    {
        FArianeSegment* Segment = InstancedSegment.GetMutablePtr<FArianeSegment>();

        InvalidateSegment( Segment );
    }

    Invalidate( FArianePathInvalidationFlags().SetAll() );

/** Unimplemented
    for( FInstancedStruct& InstancedVertex : InstancedVertices )
    {
        FArianeVertex* Vertex = InstancedVertex.GetMutablePtr<FArianeVertex>();

        InvalidateVertex( Vertex );
    }
*/
}

bool
FArianePath::Update( bool Recurse )
{
    FArianePathInvalidationFlags* PathInvalidationFlags = static_cast<FArianePathInvalidationFlags*>(InvalidationFlags);

    FArianeObject::Update( Recurse );

    if( PathInvalidationFlags->VertexGeometry
     || PathInvalidationFlags->VertexTopology
     || PathInvalidationFlags->SegmentGeometry
     || PathInvalidationFlags->SegmentTopology )
    {
        Geometry3D.Build();

        UpdateBounds();

        PathInvalidationFlags->VertexGeometry
      = PathInvalidationFlags->VertexTopology
      = PathInvalidationFlags->SegmentGeometry
      = PathInvalidationFlags->SegmentTopology = 0;
    }

    return true; // update succeeded
}


void
FArianePath::InvalidatePointerCache( TArray<FArianeVertexID>& VertexIDArray )
{
    for( FArianeVertexID& VertexID : VertexIDArray )
    {
        VertexID.InvalidatePointerCache();
    }
}

void
FArianePath::InvalidatePointerCache( TArray<FArianeSegmentID>& SegmentIDArray )
{
    for( FArianeSegmentID& SegmentID : SegmentIDArray )
    {
        SegmentID.InvalidatePointerCache();
    }
}

FArianePathGeometry3D&
FArianePath::GetGeometry3D()
{
    return Geometry3D;
}

FArianePathGeometry3D::~FArianePathGeometry3D()
{
    if( VertexFactory )
    {
        IndexBuffer.ReleaseResource();
        VertexBuffers.PositionVertexBuffer.ReleaseResource();
        VertexBuffers.StaticMeshVertexBuffer.ReleaseResource();

        VertexFactory->ReleaseResource();

        delete VertexFactory;
    }
}

FArianePathGeometry3D::FArianePathGeometry3D( FArianePath* InPath )
    : Path( InPath )
    , VertexFactory ( nullptr )
{
}

FVector
FArianePathGeometry3D::GetTangentVectorAt( FArianeSegment* Segment
                                         , FVector* OptionalPerpendicularVector
                                         , double T
                                         , bool bNormalize )
{
    FVector SegmentVector =  Segment->GetVertex(1)->GetPosition() - Segment->GetVertex(0)->GetPosition();
    FVector TangentVector = FVector::Zero();

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = Segment->GetVertex( static_cast<uint32>(T) );
        FVector AverageVector = FVector::Zero();

        if( Vertex->GetSegments().Num() == 2 )
        {
            for( FArianeSegment* ConnectedSegment : Vertex->GetSegments() )
            {
                FVector ConnectedSegmentVector = ConnectedSegment->GetVectorLeavingFromVertex( Vertex, true );

                AverageVector += ConnectedSegmentVector;
            }

            AverageVector /= Vertex->GetSegments().Num();

            if( AverageVector.IsNearlyZero() == false )
            {
                FVector Perpendicular = OptionalPerpendicularVector ? *OptionalPerpendicularVector
                                                                    : AverageVector.Cross( SegmentVector );

                TangentVector = Perpendicular.Cross( AverageVector );
            }
        }
    }

    if( TangentVector.IsNearlyZero() )
    {
        TangentVector = SegmentVector;
    }

    // Let's go in the same direction as the segment
    if( TangentVector.Dot( SegmentVector ) < 0.0f )
    {
        TangentVector = -TangentVector;
    }

    if( bNormalize )
    {
        TangentVector.Normalize();
    }

    return TangentVector;
}

/*
*
// Scheme when valence equals 2
// Vertex        = V
// CloserVertex  = CV
// FurtherVertex = FV
//
// FV   V    FV
// °    °    °
//  \  / \  /
//   \/   \/
//   °     °
//   CV   CV

FVector
FArianePathGeometry3D::GetTangentVectorAt( FArianeSegment* Segment
                                         , const FVector& PerpendicularVector
                                         , double T
                                         , bool bNormalize )
{
    FVector SegmentVector =  Segment->GetVertex(1)->GetPosition() - Segment->GetVertex(0)->GetPosition();
    FVector TangentVector = FVector::Zero();

    if( ( T == 0.0f ) || ( T == 1.0f ) )
    {
        FArianeVertex* Vertex = Segment->GetVertex( static_cast<uint32>(T) );
        FVector AverageVector = FVector::Zero();

        if( Vertex->GetSegments().Num() == 2 )
        {
            for( FArianeSegment* ConnectedSegment : Vertex->GetSegments() )
            {
                FVector ConnectedSegmentVector = ConnectedSegment->GetVectorLeavingFromVertex( Vertex, true );

                AverageVector += ConnectedSegmentVector;
            }

            AverageVector /= Vertex->GetSegments().Num();

            TangentVector = AverageVector.Cross( PerpendicularVector );
        }
        else
        {
            TangentVector = SegmentVector;
        }
    }

    if( TangentVector.IsZero() )
    {
        TangentVector = SegmentVector;
    }

    // Let's go in the same direction as the segment
    if( TangentVector.Dot( SegmentVector ) < 0.0f )
    {
        TangentVector = -TangentVector;
    }

    if( bNormalize )
    {
        TangentVector.Normalize();
    }

    return TangentVector;
}
*/

/*
FVector
FArianePathGeometry3D::GetPerpendicularVector( FArianeVertex* Vertex
                                             , FVector& InOutPreviousPerpendicularVector
                                             , bool bNormalize )
{
    FVector PerpendicularVector = InOutPreviousPerpendicularVector; // default if none is found

    if( Vertex->GetSegments().Num() <= 2 )
    {
        FArianeVertex *NeighbourVertices[5] = { nullptr
                                              , nullptr
                                              , Vertex
                                              , nullptr
                                              , nullptr };
        FArianeVertex* CloserVertices[2] = { nullptr, nullptr };
        FArianeVertex* FurtherVertices[2] = { nullptr, nullptr };
        static uint32 TriangleIndices[3][3] = { { 0, 1, 2 }, {  2, 1, 3 }, { 3, 4, 2 } };
        FVector SumVector = FVector( 0.0f, 0.0f, 0.0f );
        uint32 CloserVertexCount = 0;
        uint32 CrossCount = 0;

        for( FArianeSegment* Segment : Vertex->GetSegments() )
        {
            FArianeVertex *CloserVertex = Segment->GetOtherVertex( Vertex );

            CloserVertices[CloserVertexCount] = CloserVertex;

            if( CloserVertex->GetSegments().Num() <= 2 )
            {
                for( FArianeSegment* OtherSegment : CloserVertex->GetSegments() )
                {
                    if( OtherSegment != Segment )
                    {
                        FArianeVertex *FurtherVertex = OtherSegment->GetOtherVertex( CloserVertex );

                        FurtherVertices[CloserVertexCount] = FurtherVertex;

                        break;
                    }
                }
            }

            CloserVertexCount++;
        }

        if( CloserVertices[0] )
        {
            NeighbourVertices[0] = FurtherVertices[0];
            NeighbourVertices[1] = CloserVertices[0];
        }

        if( CloserVertices[1] )
        {
            NeighbourVertices[3] = CloserVertices[1];
            NeighbourVertices[4] = FurtherVertices[1];
        }

        for( uint32 i = 0; i < 3; i++ )
        {
            uint32 TriVertexIndex0 = TriangleIndices[i][0];
            uint32 TriVertexIndex1 = TriangleIndices[i][1];
            uint32 TriVertexIndex2 = TriangleIndices[i][2];
            FArianeVertex* TriVertex0 = NeighbourVertices[TriVertexIndex0];
            FArianeVertex* TriVertex1 = NeighbourVertices[TriVertexIndex1];
            FArianeVertex* TriVertex2 = NeighbourVertices[TriVertexIndex2];

            if(  TriVertex0 && TriVertex1 && TriVertex2 )
            {
                FVector V0V1 = TriVertex1->GetPosition() - TriVertex0->GetPosition();
                FVector V0V2 = TriVertex2->GetPosition() - TriVertex0->GetPosition();
                FVector CrossVector = V0V1.Cross( V0V2 );

                if( CrossVector.Normalize() )
                {
                    if( ( InOutPreviousPerpendicularVector.IsZero() == false )
                     && ( InOutPreviousPerpendicularVector.Dot( CrossVector ) < 0.0f ) )
                    {
                        CrossVector = -CrossVector;
                    }

                    SumVector += CrossVector;

                    CrossCount++;

                    if( CrossVector.IsZero() == false )
                    {
                        InOutPreviousPerpendicularVector = CrossVector;
                    }
                }
            }
        }

        if( CrossCount )
        {
            PerpendicularVector = SumVector / CrossCount;

            if( bNormalize )
            {
                PerpendicularVector.Normalize();
            }
        }
    }

    return PerpendicularVector;
}
*/

void
FArianePathGeometry3D::BuildSegmentAsFlat( FArianeSegment* Segment
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    double Radius0 = SegmentVertices[0]->GetRadius();
    double Radius1 = SegmentVertices[1]->GetRadius();
    double DeltaRadius = Radius1 - Radius0;
    FVector Normal0 = SegmentVertices[0]->GetNormal();
    FVector Normal1 = SegmentVertices[1]->GetNormal();
    FVector DeltaNormal = Normal1 - Normal0;
    FVector SegmentVector = SegmentVertices[1]->GetPosition() - SegmentVertices[0]->GetPosition();

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * 2
                          , ( Segment->GetFractionCount() * 2 ) );

    TArray<FModelVertex>& ModelVertexCache = const_cast<TArray<FModelVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    TArray<FArianeSegment::Fraction>& FractionCache = const_cast<TArray<FArianeSegment::Fraction>&>(Segment->GetFractionCache());
    TArray<FArianePoint*>& FractionPoints = const_cast<TArray<FArianePoint*>&>(Segment->GetFractionPoints());

    for( int32 FractionPointIndex = 0; FractionPointIndex < FractionPoints.Num(); FractionPointIndex++ )
    {
        FArianePoint* Point = FractionPoints[FractionPointIndex];
        float PointT = Segment->GetFractionPointT( FractionPointIndex );
        uint32 ModelVertexOffset = FractionPointIndex * 2;
        double PointRadius = Radius0 + ( DeltaRadius * PointT );
        FVector PerpendicularVector = Normal0 + ( DeltaNormal * PointT );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , nullptr
                                                  , PointT
                                                  , false );

        if( PerpendicularVector.Normalize() )
        {
            FVector UpVector = PerpendicularVector.Cross( TangentVector );
            FModelVertex* ModelVertex0 = &ModelVertexCache[ModelVertexOffset+0];
            FModelVertex* ModelVertex1 = &ModelVertexCache[ModelVertexOffset+1];

            if( UpVector.Normalize() )
            {
                FVector NewPosition0 = Point->GetPosition() + ( UpVector * PointRadius );
                FVector NewPosition1 = Point->GetPosition() - ( UpVector * PointRadius );

                ModelVertex0->Position.X = NewPosition0.X;
                ModelVertex0->Position.Y = NewPosition0.Y;
                ModelVertex0->Position.Z = NewPosition0.Z;

                ModelVertex1->Position.X = NewPosition1.X;
                ModelVertex1->Position.Y = NewPosition1.Y;
                ModelVertex1->Position.Z = NewPosition1.Z;
            }
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < FractionCache.Num(); FractionIndex++ )
    {
        FArianeSegment::Fraction& SegmentFraction = FractionCache[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex * 2;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * 2;
        FVector3f SampleVec0 = ModelVertexCache[ModelVertexOffset0 + 1].Position
                             - ModelVertexCache[ModelVertexOffset0 + 0].Position;
        FVector3f SampleVec1 = ModelVertexCache[ModelVertexOffset1 + 1].Position
                             - ModelVertexCache[ModelVertexOffset1 + 0].Position;

        bool Twisted = ( SampleVec0.Dot( SampleVec1 ) < 0.0f ) ? true : false;

        uint32 Triangle0Index = ( FractionIndex * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
        uint32 Triangle1Index = Triangle0Index + 3;

        if( Twisted == false )
        {
            // first triangle
            IndexCache[Triangle0Index+0] = ModelVertexOffset1 + 1;
            IndexCache[Triangle0Index+1] = ModelVertexOffset0 + 1;
            IndexCache[Triangle0Index+2] = ModelVertexOffset0;

            // second triangle
            IndexCache[Triangle1Index+0] = ModelVertexOffset0;
            IndexCache[Triangle1Index+1] = ModelVertexOffset1;
            IndexCache[Triangle1Index+2] = ModelVertexOffset1 + 1;
        }
        else
        {
            // first triangle
            IndexCache[Triangle0Index+0] = ModelVertexOffset1;
            IndexCache[Triangle0Index+1] = ModelVertexOffset0 + 1;
            IndexCache[Triangle0Index+2] = ModelVertexOffset0;

            // second triangle
            IndexCache[Triangle1Index+0] = ModelVertexOffset0;
            IndexCache[Triangle1Index+1] = ModelVertexOffset1 + 1;
            IndexCache[Triangle1Index+2] = ModelVertexOffset1;
        }
    }
}

void
FArianePathGeometry3D::BuildSegmentAsTube( FArianeSegment* Segment
                                         , FVector& InOutPreviousPerpendicularVector )
{
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    double Radius0 = SegmentVertices[0]->GetRadius();
    double Radius1 = SegmentVertices[1]->GetRadius();
    double DeltaRadius = Radius1 - Radius0;
    uint32 Divisions = 12;
    //FVector PerpendicularVector0 = GetPerpendicularVector( SegmentVertices[0], InOutPreviousPerpendicularVector, false );
    //FVector PerpendicularVector1 = GetPerpendicularVector( SegmentVertices[1], InOutPreviousPerpendicularVector, false );
    //FVector DiffPerpendicularVector = PerpendicularVector1 - PerpendicularVector0;
    //FVector UpVector0 = GetUpVector( SegmentVertices[0], false );
    //FVector UpVector1 = GetUpVector( SegmentVertices[1], false );
    //FVector DiffUpVector = UpVector1 - UpVector0;

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * Divisions
                          , ( Segment->GetFractionCount() * 2 ) * Divisions );

    TArray<FModelVertex>& ModelVertexCache = const_cast<TArray<FModelVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    TArray<FArianeSegment::Fraction>& FractionCache = const_cast<TArray<FArianeSegment::Fraction>&>(Segment->GetFractionCache());
    TArray<FArianePoint*>& FractionPoints = const_cast<TArray<FArianePoint*>&>(Segment->GetFractionPoints());

    for( int32 FractionPointIndex = 0; FractionPointIndex < FractionPoints.Num(); FractionPointIndex++ )
    {
        FArianePoint* Point = FractionPoints[FractionPointIndex];
        float PointT = Segment->GetFractionPointT( FractionPointIndex );
        uint32 ModelVertexOffset = FractionPointIndex * Divisions;
        //FVector PerpendicularVector = PerpendicularVector0 + ( DiffPerpendicularVector * PointT );
        //FVector UpVector = UpVector0 + ( DiffUpVector * PointT );
        //FVector TangentVector = PerpendicularVector.Cross( UpVector );
        //FVector PerpendicularVector = SegmentVector.Cross( Painting3DComponent->GetUpVector() );
        double PointRadius = Radius0 + ( DeltaRadius * PointT );
        FVector TangentVector = GetTangentVectorAt( Segment
                                                  , nullptr
                                                  , PointT
                                                  , false );

        FVector PerpendicularVector = TangentVector.Cross( Path->Painting3DComponent->GetUpVector() );

        PerpendicularVector.Normalize();

        if( PerpendicularVector.IsZero() == false )
        {
            float AngleInDegrees = 0.0f;
            float StepAngle = ( float ) 360 / Divisions;

            for( uint32 j = 0; j < Divisions; j++ )
            {
                FRotator Rotator = UKismetMathLibrary::RotatorFromAxisAndAngle( TangentVector, AngleInDegrees );
                FModelVertex* ModelVertex = &ModelVertexCache[ModelVertexOffset+j];
                FVector RotatedPosition = Point->GetPosition() + ( Rotator.RotateVector( PerpendicularVector ) * PointRadius );

                ModelVertex->Position.X = RotatedPosition.X;
                ModelVertex->Position.Y = RotatedPosition.Y;
                ModelVertex->Position.Z = RotatedPosition.Z;

                AngleInDegrees += StepAngle;
            }
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < FractionCache.Num(); FractionIndex++ )
    {
        FArianeSegment::Fraction& SegmentFraction = FractionCache[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex       * Divisions;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * Divisions;
        FVector3f SampleVec0 = ModelVertexCache[ModelVertexOffset0 + 1].Position
                             - ModelVertexCache[ModelVertexOffset0 + 0].Position;
        FVector3f SampleVec1 = ModelVertexCache[ModelVertexOffset1 + 1].Position
                             - ModelVertexCache[ModelVertexOffset1 + 0].Position;

        bool Twisted = ( SampleVec0.Dot( SampleVec1 ) < 0.0f ) ? true : false;

        for( uint32 i = 0, j = ( Divisions * 2 ) - 1; i < Divisions; i++, j-- )
        {
            uint32 Triangle0Index = ( FractionIndex * Divisions * 2 * 3 ) + ( i * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
            uint32 Triangle1Index = Triangle0Index + 3;

            if( Twisted == false )
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) % Divisions );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( i + 1 ) % Divisions );
            }
            else
            {
                // first triangle
                IndexCache[Triangle0Index+0] = ModelVertexOffset1 + ( ( j - 1 ) % Divisions );
                IndexCache[Triangle0Index+1] = ModelVertexOffset0 + ( ( i + 1 ) % Divisions );
                IndexCache[Triangle0Index+2] = ModelVertexOffset0 + ( ( i     ) % Divisions );

                // second triangle
                IndexCache[Triangle1Index+0] = ModelVertexOffset0 + ( ( i     ) % Divisions );
                IndexCache[Triangle1Index+1] = ModelVertexOffset1 + ( ( j     ) % Divisions );
                IndexCache[Triangle1Index+2] = ModelVertexOffset1 + ( ( j - 1 ) % Divisions );
            }
        }
    }
}

const FStaticMeshVertexBuffers&
FArianePathGeometry3D::GetVertexBuffers() const
{
    return VertexBuffers;
}

const FRawStaticIndexBuffer&
FArianePathGeometry3D::GetIndexBuffer() const
{
    return IndexBuffer;
}

FArianePath*
FArianePathGeometry3D::GetPath()
{
    return Path;
}

FLocalVertexFactory*
FArianePathGeometry3D::GetVertexFactory()
{
    return VertexFactory;
}

static inline void InitOrUpdateResource( FRHICommandListImmediate& RHICmdList
                                       , FRenderResource* Resource )
{
    if (!Resource->IsInitialized())
    {
        Resource->InitResource( RHICmdList );
    }
    else
    {
        Resource->UpdateRHI( RHICmdList );
    }
}

void
FArianePathGeometry3D::InitVertexFactory( TArray<FModelVertex>& ModelVertices
                                        , TArray<uint32>& Indices )
{
    if( VertexFactory == nullptr )
    {
        VertexFactory = new FLocalVertexFactory( Path->Painting3DComponent->GetWorld()->GetFeatureLevel(), "Path Vertex Factory" );
    }

    ENQUEUE_RENDER_COMMAND(StaticMeshVertexBuffersLegacyInit)(
        [ this
        , ModelVerticesAsync = MoveTemp(ModelVertices) ] ( FRHICommandListImmediate& RHICmdList )
        {
            VertexBuffers.InitModelBuffers( const_cast<TArray<FModelVertex>&>(ModelVerticesAsync) );

            //Initialize or update the RHI vertex buffers
            InitOrUpdateResource( RHICmdList, &VertexBuffers.PositionVertexBuffer );
            InitOrUpdateResource( RHICmdList, &VertexBuffers.StaticMeshVertexBuffer );

            //Use the RHI vertex buffers to create the needed Vertex stream components in an FDataType instance, and then set it as the data of the vertex factory
            FLocalVertexFactory::FDataType Data;

            VertexBuffers.PositionVertexBuffer.BindPositionVertexBuffer( VertexFactory, Data );
            VertexBuffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer( VertexFactory, Data );
            VertexBuffers.StaticMeshVertexBuffer.BindTangentVertexBuffer( VertexFactory, Data );

            VertexFactory->SetData( Data );

            //Initalize the vertex factory using the data that we just set, this will call the InitRHI() method that we implemented in out vertex factory
            InitOrUpdateResource( RHICmdList, VertexFactory );
        } );

    ENQUEUE_RENDER_COMMAND(IndexBufferInit)(
        [ this
        , IndicesAsync = MoveTemp(Indices) ] ( FRHICommandListImmediate& RHICmdList )
        {
            IndexBuffer.SetIndices( IndicesAsync, EIndexBufferStride::Type::Force32Bit );

            InitOrUpdateResource( RHICmdList, &IndexBuffer );
        } );
}

void
FArianePathGeometry3D::Build()
{
    uint32 TotalModelVertexCount = 0;
    uint32 TotalIndexCount = 0;
    TArray<uint32> Indices;
    TArray<FModelVertex> ModelVertices;
    FVector PreviousPerpendicularVector = Path->Painting3DComponent->GetUpVector();//FVector::Zero();

    // TODO : update  invalidated segments only
    for( FArianeSegment* Segment : Path->GetInvalidatedSegments() )
    {
        FArianeVertex* segmentVertices[2] = { Segment->GetVertex(0)
                                            , Segment->GetVertex(1) };
        uint32 SegmentIndexCount = 0;
        FModelVertex storedVertex;

        Segment->Update();

        switch( Path->Painting3DComponent->GeometryMode )
        {
            case EArianePainting3DGeometryMode::Flat :
                BuildSegmentAsFlat( Segment, PreviousPerpendicularVector );
            break;

            case EArianePainting3DGeometryMode::Tube :
                BuildSegmentAsTube( Segment, PreviousPerpendicularVector );
            break;

            default:
            break;
        }
    }

    Path->GetInvalidatedSegments().Empty();

    for( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        TotalModelVertexCount += Segment->GetModelVertexCache().Num();
        TotalIndexCount += Segment->GetIndexCache().Num();
    }

    ModelVertices.SetNum( TotalModelVertexCount );
    Indices.SetNum( TotalIndexCount );

    TotalModelVertexCount = 0;
    TotalIndexCount = 0;

    for( FArianeSegmentID& SegmentID : Path->GetSegments() )
    {
        FArianeSegment* Segment = SegmentID.GetSegment();

        if( Segment->GetIndexCache().Num() )
        {
            const TArray<FModelVertex>& SegmentModelVertexCache = Segment->GetModelVertexCache();
            const TArray<uint32>& SegmentIndices = Segment->GetIndexCache();

            memcpy( &ModelVertices[TotalModelVertexCount]
                  , &SegmentModelVertexCache[0]
                  ,  SegmentModelVertexCache.Num() * sizeof( FModelVertex ) );

            memcpy( &Indices[TotalIndexCount]
                  , &SegmentIndices[0]
                  ,  SegmentIndices.Num() * sizeof( uint32 ) );

            // renumber indices
            for( int32 i = TotalIndexCount, j = 0; j < SegmentIndices.Num(); i++, j++ )
            {
                Indices[i] += TotalModelVertexCount;
            }

            TotalModelVertexCount += SegmentModelVertexCache.Num();
            TotalIndexCount += SegmentIndices.Num();
        }
    }

    if( ModelVertices.Num() )
    {
        //VertexBuffers.InitModelBuffers( ModelVertices );
        //IndexBuffer.SetIndices( Indices, EIndexBufferStride::Type::Force32Bit );

        InitVertexFactory( ModelVertices, Indices );
    }
}
