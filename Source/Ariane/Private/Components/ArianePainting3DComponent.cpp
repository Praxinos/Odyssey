// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponent.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"

// testing
#include "Components/LineBatchComponent.h"
/*
static inline void
InitOrUpdateResource( FRenderResource* Resource )
{
    if (!Resource->IsInitialized())
    {
        Resource->InitResource();
    }
    else
    {
        Resource->UpdateRHI();
    }
}

static void
InitVertexFactoryData( FVertexFactory* iVertexFactory
                     , FStaticMeshVertexBuffers* iVertexBuffers )
{
    ENQUEUE_RENDER_COMMAND( StaticMeshVertexBuffersLegacyInit ) (
        [ iVertexFactory
        , iVertexBuffers ]( FRHICommandListImmediate& RHICmdList )
        {
            //Initialize or update the RHI vertex buffers
            InitOrUpdateResource( &iVertexBuffers->PositionVertexBuffer );
            InitOrUpdateResource( &iVertexBuffers->StaticMeshVertexBuffer );

            //Use the RHI vertex buffers to create the needed Vertex stream components in an FDataType instance, and then set it as the data of the vertex factory
            FLocalVertexFactory::FDataType Data;

            iVertexBuffers->PositionVertexBuffer.BindPositionVertexBuffer( iVertexFactory, Data );
            iVertexBuffers->StaticMeshVertexBuffer.BindPackedTexCoordVertexBuffer( iVertexFactory, Data );
            iVertexFactory->SetData( Data );

            //Initalize the vertex factory using the data that we just set, this will call the InitRHI() method that we implemented in out vertex factory
            InitOrUpdateResource( iVertexFactory );
    });
}
*/

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

    mLineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
}

FPrimitiveSceneProxy*
UArianePainting3DComponent::CreateSceneProxy()
{
    return new FArianeGeometryProxy(this);
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
    Super::TickComponent(DeltaTime, TickType, ThisTickFunction);
    TArray<FBatchedLine> lines;

    for ( int32 i = 0, n = 1; i < mVertices.Num() - 1; i++, n++ )
    {
        FVector vertexi = mVertices[i];
        FVector vertexn = mVertices[n];

        FBatchedLine line = FBatchedLine( vertexi,
                                          vertexn,
                                          FLinearColor( 1, 1, 1, 0.5 ),
                                          10000, // for long period draw
                                          0.3,
                                          4 );
        lines.Add(line);
    }

    mLineBatchComponent->DrawLines(lines);
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

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

FArianeGeometryProxy::FArianeGeometryProxy( UArianePainting3DComponent* iPainting3DComponent )
    : FPrimitiveSceneProxy ( iPainting3DComponent )
{
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector) const
{
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
}

FPrimitiveViewRelevance
FArianeGeometryProxy::GetViewRelevance( const FSceneView* View ) const
{
    FPrimitiveViewRelevance Result;
    Result.bDrawRelevance = IsShown( View );
    Result.bShadowRelevance = IsShadowCast( View );
    Result.bDynamicRelevance = true;
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
