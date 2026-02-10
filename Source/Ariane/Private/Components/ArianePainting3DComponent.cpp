// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawIndexBuffer.h"
#include "Materials/MaterialRenderProxy.h"

// testing
#include "Components/LineBatchComponent.h"


void
FTestVertex::AddSegment( uint32 ObjectIndex
                       , uint32 SegmentIndex
                       , uint32 SegmentClass
                       , uint32 IndexInSegment )
{

}

UArianePainting3DComponent::~UArianePainting3DComponent()
{
}

UArianePainting3DComponent::UArianePainting3DComponent()
{
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;

    //LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
}

FPrimitiveSceneProxy*
UArianePainting3DComponent::CreateSceneProxy()
{
    return new FArianeGeometryProxy(GetScene()->GetFeatureLevel(), this);
}

void
UArianePainting3DComponent::PostInitProperties ()
{
    Super::PostInitProperties();

/*
    TestStructs.SetNum(5);

    TestStructs[0].dummy = 0;
    TestStructs[1].dummy = 1;
    TestStructs[2].dummy = 2;
    TestStructs[3].dummy = 3;
    TestStructs[4].dummy = 4;

    TestStructs.RemoveAt( 2 );
*/
    //InitVertexFactoryData( mGeometryProxy->VertexFactory, &mMeshVertexBuffers );
}

void
UArianePainting3DComponent::BeginPlay()
{
    Super::BeginPlay();
}

void
UArianePainting3DComponent::TickComponent( float DeltaTime
                                         , ELevelTick TickType
                                         , FActorComponentTickFunction* ThisTickFunction )
{
    AActor* actor = GetOwner();
    const FTransform& actorWorldTransform = actor->GetRootComponent()->GetComponentTransform();
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TArray<FBatchedLine> lines;

    Super::TickComponent( DeltaTime, TickType, ThisTickFunction );
/*
    for ( int32 i = 0, n = 1; i < Vertices.Num() - 1; i++, n++ )
    {
        FVector vertexi = Vertices[i];
        FVector vertexn = Vertices[n];

        FBatchedLine line = FBatchedLine( actorWorldTransform.TransformPosition( vertexi ),
                                          actorWorldTransform.TransformPosition( vertexn ),
                                          FLinearColor( 0, 0, 0, 1.0f ),
                                          10000, // for long period draw
                                          2.0f,
                                          4 );
        lines.Add(line);
    }

    LineBatchComponent->DrawLines(lines);
*/
}

FBoxSphereBounds
UArianePainting3DComponent::CalcBounds(const FTransform& LocalToWorld) const
{
    FBoxSphereBounds Ret;

    Ret.Origin = FVector( 0.f, 0.0f, 0.0f );
    Ret.BoxExtent = FVector( 100, 100, 100 );
    Ret.SphereRadius = 100.0f;

    /* Gary
    FBoxSphereBounds Ret( LocalBounds.TransformBy( LocalToWorld ) );

    Ret.BoxExtent *= BoundsScale;
    Ret.SphereRadius *= BoundsScale;
*/
    return Ret.TransformBy( LocalToWorld );
}

void
UArianePainting3DComponent::AddPath( FArianePath* Path )
{
    PathMeshs.Add( new FArianePathGeometry3D( this, Path ) );
}

const
TArray<FArianePathGeometry3D*>& UArianePainting3DComponent::GetPathMeshs()
{
    return PathMeshs;
}

void
UArianePainting3DComponent::BuildPathMeshs()
{
    FArianeGeometryProxy* GeometryProxy = static_cast<FArianeGeometryProxy*>(GetSceneProxy());
    TArray<UMaterialInterface*> UsedMaterials;

    for( FArianePathGeometry3D* PathMesh : PathMeshs )
    {
        PathMesh->Build();

        UsedMaterials.Add( PathMesh->GetPath()->GetMaterial() );
    }

    GeometryProxy->SetUsedMaterialForVerification( UsedMaterials );
}

//--------------------------------------------------------------------------------------------------


FArianePathGeometry3D::~FArianePathGeometry3D()
{
    IndexBuffer.ReleaseResource();
    VertexFactory.ReleaseResource();
}

FArianePathGeometry3D::FArianePathGeometry3D( UArianePainting3DComponent* InPainting3DComponent
                                            , FArianePath* InPath )
    : Painting3DComponent ( InPainting3DComponent )
    , Path( InPath )
    , VertexFactory ( InPainting3DComponent->GetScene()->GetFeatureLevel(), "Path Vertex Factory" )
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

        FVector PerpendicularVector = TangentVector.Cross( Painting3DComponent->GetUpVector() );

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

FLocalVertexFactory&
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
FArianePathGeometry3D::InitVertexFactory()
{
    ENQUEUE_RENDER_COMMAND(StaticMeshVertexBuffersLegacyInit)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            //Initialize or update the RHI vertex buffers
            InitOrUpdateResource( RHICmdList, &VertexBuffers.PositionVertexBuffer );
            InitOrUpdateResource( RHICmdList, &VertexBuffers.StaticMeshVertexBuffer );

            //Use the RHI vertex buffers to create the needed Vertex stream components in an FDataType instance, and then set it as the data of the vertex factory
            FLocalVertexFactory::FDataType Data;

            VertexBuffers.PositionVertexBuffer.BindPositionVertexBuffer( &VertexFactory, Data );
            VertexBuffers.StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer( &VertexFactory, Data );
            VertexBuffers.StaticMeshVertexBuffer.BindTangentVertexBuffer( &VertexFactory, Data );

            VertexFactory.SetData( Data );

            //Initalize the vertex factory using the data that we just set, this will call the InitRHI() method that we implemented in out vertex factory
            InitOrUpdateResource( RHICmdList, &VertexFactory );
        } );

    ENQUEUE_RENDER_COMMAND(IndexBufferInit)(
        [this](FRHICommandListImmediate& RHICmdList)
        {
            InitOrUpdateResource( RHICmdList, &IndexBuffer );
        } );
}

// TODO: Call on invalidated Path only
void
FArianePathGeometry3D::Build()
{
    uint32 TotalModelVertexCount = 0;
    uint32 TotalIndexCount = 0;
    uint32 IndexCount = 0;
    TArray<uint32> Indices;
    TArray<FModelVertex> ModelVertices;
    FVector PreviousPerpendicularVector = Painting3DComponent->GetUpVector();//FVector::Zero();

    // for each Vertex, compute the tangent vector
    for( FArianeSegment* Segment : Path->GetSegments() )
    {
        FArianeVertex* segmentVertices[2] = { Segment->GetVertex(0)
                                            , Segment->GetVertex(1) };
        uint32 SegmentIndexCount = 0;
        FModelVertex storedVertex;

        //if( Segment->IsInvalidated() )
        //{
            BuildSegmentAsFlat( Segment, PreviousPerpendicularVector );
        //}

        TotalModelVertexCount += Segment->GetModelVertexCache().Num();
        TotalIndexCount += Segment->GetIndexCache().Num();
    }

    ModelVertices.SetNum( TotalModelVertexCount );
    Indices.SetNum( TotalIndexCount );

    TotalModelVertexCount = 0;
    TotalIndexCount = 0;

    for( FArianeSegment* Segment : Path->GetSegments() )
    {
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

    VertexBuffers.InitModelBuffers( ModelVertices );
    IndexBuffer.SetIndices( Indices, EIndexBufferStride::Type::Force32Bit );

    InitVertexFactory();
}

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianePainting3DComponent* iPainting3DComponent )
    : FPrimitiveSceneProxy ( iPainting3DComponent )
    , Painting3DComponent ( iPainting3DComponent )
{
}

void
FArianeGeometryProxy::DrawStaticElements( FStaticPrimitiveDrawInterface * PDI )
{
    FMeshBatch MeshBatch;

    // for now, update all paths mesh here
    Painting3DComponent->BuildPathMeshs();

    for ( FArianePathGeometry3D* Mesh : Painting3DComponent->GetPathMeshs() )
    {
        if( Mesh->GetIndexBuffer().GetNumIndices() )
        {
            FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

            BatchElement.IndexBuffer = &Mesh->GetIndexBuffer();

            //Mesh.bWireframe = bWireframe;
            MeshBatch.VertexFactory = &Mesh->GetVertexFactory();
            //Mesh.MaterialRenderProxy = MaterialProxy;

            //Additional data
            BatchElement.FirstIndex = 0;
            BatchElement.NumPrimitives = Mesh->GetIndexBuffer().GetNumIndices() / 3;
            BatchElement.MinVertexIndex = 0;
            BatchElement.MaxVertexIndex = Mesh->GetVertexBuffers().PositionVertexBuffer.GetNumVertices();

            MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
            MeshBatch.Type = PT_TriangleList;
            MeshBatch.DepthPriorityGroup = SDPG_World;
            MeshBatch.bCanApplyViewModeOverrides = false;


            // Else the virtual texture check fails in RuntimeVirtualTextureRender.cpp:338
            // and the static mesh isn't rendered at all
            MeshBatch.LODIndex = 0;

            // Runtime virtual texture mesh elements.
            MeshBatch.CastShadow = 0;
            MeshBatch.bUseAsOccluder = 0;
            MeshBatch.bUseForDepthPass = 0;
            MeshBatch.bUseForMaterial = 0;
            MeshBatch.bDitheredLODTransition = 0;
            MeshBatch.bRenderToVirtualTexture = 1;

            PDI->DrawMesh(MeshBatch, FLT_MAX);
        }
    }
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector) const
{
    // for now, update all paths mesh here
    Painting3DComponent->BuildPathMeshs();

    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];

        for ( FArianePathGeometry3D* Mesh : Painting3DComponent->GetPathMeshs() )
        {
            if( Mesh->GetIndexBuffer().GetNumIndices() && Mesh->GetIndexBuffer().IsInitialized() )
            {


/*
                FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();
                DynamicPrimitiveUniformBuffer.Set( FMatrix::Identity
                                                 , FMatrix::Identity
                                                 , GetBounds()
                                                 , GetLocalBounds()
                                                 , false, false
                                                 //, DrawsVelocity()
                                                 , false );
*/




/*
                auto* MaterialProxy = new FColoredMaterialRenderProxy( GEngine->Materi ->GetRenderProxy() );
                Collector.RegisterOneFrameMaterialProxy( MaterialProxy );
*/
                //UMaterialInterface* MaterialInterface = Mesh->GetPath()->GetMaterial();
                UMaterialInterface* MaterialInterface = UMaterial::GetDefaultMaterial(MD_Surface);

                // Allocate a mesh batch and get a ref to the first element
                FMeshBatch& MeshBatch = Collector.AllocateMesh();
                FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                BatchElement.IndexBuffer = &Mesh->GetIndexBuffer();

                //Mesh.bWireframe = bWireframe;
                MeshBatch.VertexFactory = &Mesh->GetVertexFactory();
                MeshBatch.MaterialRenderProxy = MaterialInterface->GetRenderProxy();;

                //The LocalVertexFactory uses a uniform buffer to pass primitve data like the local to world transform for this frame and for the previous one
                //Most of this data can be fetched using the helper function below
                bool bHasPrecomputedVolumetricLightmap;
                FMatrix PreviousLocalToWorld;
                int32 SingleCaptureIndex;
                bool bOutputVelocity;

                GetScene().GetPrimitiveUniformShaderParameters_RenderThread( GetPrimitiveSceneInfo()
                                                                           , bHasPrecomputedVolumetricLightmap
                                                                           , PreviousLocalToWorld
                                                                           , SingleCaptureIndex
                                                                           , bOutputVelocity );

                //Alloate a temporary primitive uniform buffer, fill it with the data and set it in the batch element
                FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

                DynamicPrimitiveUniformBuffer.Set( Collector.GetRHICommandList()
                                                 , GetLocalToWorld()
                                                 , PreviousLocalToWorld
                                                 , GetBounds()
                                                 , GetLocalBounds()
                                                 , true
                                                 , bHasPrecomputedVolumetricLightmap
                                              // , DrawsVelocity()
                                                 , bOutputVelocity );

                BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
                BatchElement.PrimitiveIdMode = PrimID_DynamicPrimitiveShaderData;

                //Additional data
                BatchElement.FirstIndex = 0;
                BatchElement.NumPrimitives = Mesh->GetIndexBuffer().GetNumIndices() / 3;
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = Mesh->GetVertexBuffers().PositionVertexBuffer.GetNumVertices();
                MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
                MeshBatch.Type = PT_TriangleList;
                MeshBatch.DepthPriorityGroup = SDPG_World;
                MeshBatch.bCanApplyViewModeOverrides = false;

                //Add the batch to the collector
                Collector.AddMesh( ViewIndex, MeshBatch );
            }
        }
    }

#ifdef unused
    for( FArianePath* Path : Painting3DComponent->GetPaths() )
    {
        // Draw the mesh.
        FMeshBatch Mesh;
        FMeshBatchElement& BatchElement = Mesh.Elements[0];

        BatchElement.IndexBuffer = &IndexBuffer;
        Mesh.bWireframe = bWireframe;
        Mesh.VertexFactory = &VertexFactory;
        Mesh.MaterialRenderProxy = MaterialProxy;
#if ENGINE_MAJOR_VERSION >= 4 && ENGINE_MINOR_VERSION >= 5
        BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true, UseEditorDepthTest());
#else
        BatchElement.PrimitiveUniformBuffer = CreatePrimitiveUniformBufferImmediate(GetLocalToWorld(), GetBounds(), GetLocalBounds(), true);
#endif
        BatchElement.FirstIndex = 0;
        BatchElement.NumPrimitives = IndexBuffer.Indices.Num() / 3;
        BatchElement.MinVertexIndex = 0;
        BatchElement.MaxVertexIndex = VertexBuffer.Vertices.Num() - 1;
        Mesh.ReverseCulling = IsLocalToWorldDeterminantNegative();
        Mesh.Type = PT_TriangleList;
        Mesh.DepthPriorityGroup = SDPG_World;
        PDI->DrawMesh(Mesh);
    }

    for( const FSceneView* sceneView : Views )
    {
        FMeshBatch& Mesh = Collector.AllocateMesh();
        FMeshBatchElement& batchElement = Mesh.Elements[0];
/*
        batchElement.( FVector( iHUDCoordsP0, 0.f)
                                           , FVector( iHUDCoordsP1, 0.f)
                                           , iColor
                                           , iParams.mCanvas->GetHitProxyId()
                                           , iThickness
                                           , 0.f
                                           , true );
*/

    }
#endif
}

FPrimitiveViewRelevance
FArianeGeometryProxy::GetViewRelevance( const FSceneView* View ) const
{
    FPrimitiveViewRelevance Result;
    Result.bDrawRelevance = IsShown( View );
    Result.bShadowRelevance = IsShadowCast( View );

    Result.bDynamicRelevance = true;
    Result.bStaticRelevance = false;

    Result.bRenderInMainPass = ShouldRenderInMainPass();
    Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
    Result.bRenderCustomDepth = ShouldRenderCustomDepth();
    Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;

    //MaterialRelevance.SetPrimitiveViewRelevance(Result);

    Result.bVelocityRelevance = IsMovable() && Result.bOpaque && Result.bRenderInMainPass;

    return Result;
}

SIZE_T
FArianeGeometryProxy::GetTypeHash() const
{
    static size_t UniquePointer;
    return reinterpret_cast<size_t>(&UniquePointer);
}

uint32
FArianeGeometryProxy::GetMemoryFootprint( void ) const
{
    return sizeof( *this );
}
