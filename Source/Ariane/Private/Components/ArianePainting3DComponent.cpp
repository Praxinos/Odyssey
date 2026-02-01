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

// testing
#include "Components/LineBatchComponent.h"

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
UArianePainting3DComponent::AddPath( FArianePath* iPath )
{
    PathMeshs.Add( new FArianePathGeometry3D( this, iPath ) );
}

const
TArray<FArianePathGeometry3D*>& UArianePainting3DComponent::GetPathMeshs()
{
    return PathMeshs;
}

void
UArianePainting3DComponent::BuildPathMeshs()
{
    for( FArianePathGeometry3D* PathMesh : PathMeshs )
    {
        PathMesh->Build();
    }
}

//--------------------------------------------------------------------------------------------------


FArianePathGeometry3D::~FArianePathGeometry3D()
{
}

FArianePathGeometry3D::FArianePathGeometry3D( UArianePainting3DComponent* InPainting3DComponent
                                            , FArianePath* InPath )
    : Painting3DComponent ( InPainting3DComponent )
    , Path( InPath )
{
}

void
FArianePathGeometry3D::BuildSegment( FArianeSegment* Segment )
{
    FArianeVertex* SegmentVertices[2] = { Segment->GetVertex(0)
                                        , Segment->GetVertex(1) };
    uint32 Divisions = 12;

    Segment->AllocateCache( ( Segment->GetFractionCount() + 1 ) * Divisions
                          , ( Segment->GetFractionCount() * 2 ) * Divisions );

    TArray<FModelVertex>& ModelVertexCache = const_cast<TArray<FModelVertex>&>(Segment->GetModelVertexCache());
    TArray<uint32>& IndexCache = const_cast<TArray<uint32>&>(Segment->GetIndexCache());
    TArray<FArianeSegment::Fraction>& FractionCache = const_cast<TArray<FArianeSegment::Fraction>&>(Segment->GetFractionCache());
    TArray<FArianePoint*>& FractionPoints = const_cast<TArray<FArianePoint*>&>(Segment->GetFractionPoints());

    for( int32 FractionPointIndex = 0; FractionPointIndex < FractionPoints.Num(); FractionPointIndex++ )
    {
        FArianePoint* Point = FractionPoints[FractionPointIndex];
        uint32 PointT = Segment->GetFractionPointT( FractionPointIndex );
        uint32 ModelVertexOffset = FractionPointIndex * Divisions;
        FVector SegmentVector = Segment->GetVectorAt( PointT, false );
        FVector AverageVector = Segment->GetAverageVectorAt( PointT );

        if( AverageVector.SquaredLength() == 0.0f )
        {
            AverageVector = Painting3DComponent->GetUpVector();
        }

        FVector Perpendicular = AverageVector.Cross( SegmentVector );

        // We need this vector to be normalized for the call to RotatorFromAxisAndAngle()
        Perpendicular.Normalize();

        FVector Tangent = Perpendicular.Cross( AverageVector );
        float AngleInDegrees = 0.0f;
        float StepAngle = ( float ) 360 / Divisions;

        // test the Tangent goes in the same direction
        if( Tangent.Dot( SegmentVector ) < 0.0f )
        {
            Tangent = -Tangent;
        }

        for( uint32 j = 0; j < Divisions; j++ )
        {
            FRotator Rotator = UKismetMathLibrary::RotatorFromAxisAndAngle( Tangent, AngleInDegrees );
            FModelVertex* ModelVertex = &ModelVertexCache[ModelVertexOffset+j];
            FVector RotatedPosition = Point->GetPosition() + Rotator.RotateVector( Perpendicular );

            ModelVertex->Position.X = RotatedPosition.X;
            ModelVertex->Position.Y = RotatedPosition.Y;
            ModelVertex->Position.Z = RotatedPosition.Z;

            AngleInDegrees += StepAngle;
        }
    }

    for( int32 FractionIndex = 0; FractionIndex < FractionCache.Num(); FractionIndex++ )
    {
        FArianeSegment::Fraction& SegmentFraction = FractionCache[FractionIndex];
        uint32 ModelVertexOffset0 =   FractionIndex       * Divisions;
        uint32 ModelVertexOffset1 = ( FractionIndex + 1 ) * Divisions;

        for( uint32 i = 0; i < Divisions; i++ )
        {
            uint32 Triangle0Index = ( FractionIndex * Divisions * 2 * 3 ) + ( i * 2 * 3 ); // 2 triangles per quad, 3 indexes per tirangle
            uint32 Triangle1Index = Triangle0Index + 3;

            // first triangles
            IndexCache[Triangle0Index+0] = ModelVertexOffset0 + i;
            IndexCache[Triangle0Index+1] = ModelVertexOffset0 + i + 1;
            IndexCache[Triangle0Index+2] = ModelVertexOffset1 + i + 1;

            // second triangles
            IndexCache[Triangle1Index+0] = ModelVertexOffset1 + i + 1;
            IndexCache[Triangle1Index+1] = ModelVertexOffset1 + i;
            IndexCache[Triangle1Index+2] = ModelVertexOffset0 + i;
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

// TODO: Call on invalidated Path only
void
FArianePathGeometry3D::Build()
{
    uint32 TotalModelVertexCount = 0;
    uint32 TotalIndexCount = 0;
    uint32 IndexCount = 0;
    TArray<uint32> Indices;
    TArray<FModelVertex> ModelVertices;

    // for each Vertex, compute the tangent vector
    for( FArianeSegment* Segment : Path->GetSegments() )
    {
        FArianeVertex* segmentVertices[2] = { Segment->GetVertex(0)
                                            , Segment->GetVertex(1) };
        uint32 SegmentIndexCount = 0;
        FModelVertex storedVertex;

        //if( Segment->IsInvalidated() )
        //{
            BuildSegment( Segment );
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
            for( uint32& Index : Indices )
            {
                Index += TotalModelVertexCount;
            }

            TotalModelVertexCount += SegmentModelVertexCache.Num();
            TotalIndexCount += SegmentIndices.Num();
        }
    }

    VertexBuffers.InitModelBuffers( ModelVertices );
    IndexBuffer.SetIndices( Indices, EIndexBufferStride::Type::Force32Bit );
}

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianePainting3DComponent* iPainting3DComponent )
    : FPrimitiveSceneProxy ( iPainting3DComponent )
    , Painting3DComponent ( iPainting3DComponent )
    , VertexFactory( InFeatureLevel )
{
}

void
FArianeGeometryProxy::DrawStaticElements( FStaticPrimitiveDrawInterface * PDI )
{
    FMeshBatch MeshBatch;

    // for now, update all paths mesh here
    Painting3DComponent->BuildPathMeshs();

    for ( const FArianePathGeometry3D* Mesh : Painting3DComponent->GetPathMeshs() )
    {
        if( Mesh->GetIndexBuffer().GetNumIndices() )
        {
            FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

            BatchElement.IndexBuffer = &Mesh->GetIndexBuffer();

            //Mesh.bWireframe = bWireframe;
            MeshBatch.VertexFactory = &VertexFactory;
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
            if( Mesh->GetIndexBuffer().GetNumIndices() )
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
                FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

                DynamicPrimitiveUniformBuffer.Set( Collector.GetRHICommandList()
                                                 , GetLocalToWorld()
                                                 , GetLocalToWorld()
                                                 , GetBounds()
                                                 , GetLocalBounds()
                                                 , true
                                                 , false
                                                 , AlwaysHasVelocity());


/*
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

                DynamicPrimitiveUniformBuffer.Set( GetLocalToWorld()
                                                 , PreviousLocalToWorld
                                                 , GetBounds()
                                                 , GetLocalBounds()
                                                 , true
                                                 , bHasPrecomputedVolumetricLightmap
                                              // , DrawsVelocity()
                                                 , bOutputVelocity );

                BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
                BatchElement.PrimitiveIdMode = PrimID_DynamicPrimitiveShaderData;
*/

                // Allocate a mesh batch and get a ref to the first element
                FMeshBatch& MeshBatch = Collector.AllocateMesh();
                FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                BatchElement.IndexBuffer = &Mesh->GetIndexBuffer();

                //Mesh.bWireframe = bWireframe;
                MeshBatch.VertexFactory = &VertexFactory;
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

                BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;

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
