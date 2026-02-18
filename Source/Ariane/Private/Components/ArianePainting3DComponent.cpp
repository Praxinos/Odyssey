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


UArianePainting3DComponent::~UArianePainting3DComponent()
{
}

UArianePainting3DComponent::UArianePainting3DComponent()
    : GeometryMode ( EArianePainting3DGeometryMode::Tube )
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
    FBoxSphereBounds RetBounds = Super::CalcBounds( FTransform::Identity );

    for( FArianePath* Path : Paths )
    {
        RetBounds = RetBounds + Path->GetBounds();
    }

    return RetBounds.TransformBy( LocalToWorld );
}

void
UArianePainting3DComponent::AddPath( FArianePath* Path )
{
    Paths.Add( Path );

    RootObject.AppendChild( Path );
}

const
TArray<FArianePath*>& UArianePainting3DComponent::GetPaths()
{
    return Paths;
}

void
UArianePainting3DComponent::Update()
{
    FArianeGeometryProxy* GeometryProxy = static_cast<FArianeGeometryProxy*>(GetSceneProxy());
    TArray<UMaterialInterface*> UsedMaterials;

    RootObject.Update( true );

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();

    for( FArianePath* Path: Paths )
    {
        UsedMaterials.Add( Path->GetMaterial() );
    }

    GeometryProxy->SetUsedMaterialForVerification( UsedMaterials );
}

void
UArianePainting3DComponent::PostEditChangeProperty( FPropertyChangedEvent& event )
{
    if( event.GetPropertyName() == GET_MEMBER_NAME_CHECKED( UArianePainting3DComponent, GeometryMode ) )
    {
        for( FArianePath* Path : Paths )
        {
            Path->Invalidate( FArianePathInvalidationFlags()
                              .SetVertexGeometry()
                              .SetSegmentGeometry() );
        }

        RootObject.Update( true );
    }

    Super::PostEditChangeProperty( event );
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

    for ( FArianePath* Path : Painting3DComponent->GetPaths() )
    {
        FArianePathGeometry3D* Mesh = Path->GetGeometry3D();

        if( Mesh->GetIndexBuffer().GetNumIndices() )
        {
            UMaterialInterface* MaterialInterface = Mesh->GetPath()->GetMaterial();
            //UMaterialInterface* MaterialInterface = UMaterial::GetDefaultMaterial( MD_Surface );

            FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

            BatchElement.IndexBuffer = &Mesh->GetIndexBuffer();

            //Mesh.bWireframe = bWireframe;
            MeshBatch.VertexFactory = &Mesh->GetVertexFactory();
            MeshBatch.MaterialRenderProxy = MaterialInterface->GetRenderProxy();

            //Additional data
            BatchElement.FirstIndex = 0;
            BatchElement.NumPrimitives = Mesh->GetIndexBuffer().GetNumIndices() / 3;
            BatchElement.MinVertexIndex = 0;
            BatchElement.MaxVertexIndex = Mesh->GetVertexBuffers().PositionVertexBuffer.GetNumVertices() - 1;

            MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
            MeshBatch.Type = PT_TriangleList;
            MeshBatch.DepthPriorityGroup = SDPG_World;
            MeshBatch.bCanApplyViewModeOverrides = false;
            MeshBatch.bDisableBackfaceCulling = true; // draw both sides

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
    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];

        for ( FArianePath* Path : Painting3DComponent->GetPaths() )
        {
            FArianePathGeometry3D* Mesh = Path->GetGeometry3D();

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
                UMaterialInterface* MaterialInterface = Mesh->GetPath()->GetMaterial();
                //UMaterialInterface* MaterialInterface = UMaterial::GetDefaultMaterial( MD_Surface );

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
                BatchElement.MaxVertexIndex = Mesh->GetVertexBuffers().PositionVertexBuffer.GetNumVertices() - 1;
                MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
                MeshBatch.Type = PT_TriangleList;
                MeshBatch.DepthPriorityGroup = SDPG_World;
                MeshBatch.bCanApplyViewModeOverrides = false;
                MeshBatch.bDisableBackfaceCulling = true; // draw both sides

                //Add the batch to the collector
                Collector.AddMesh( ViewIndex, MeshBatch );
            }
        }
    }
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
