// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeLayerFolder.h"
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
{
    ResetHierarchy();

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;

    RootFolder = NewObject<UArianeLayerFolder>( this );

    //LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
}

void
UArianePainting3DComponent::PrintPointers()
{
    for( FInstancedStruct& InstancedObject : InstancedObjects )
    {
        FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

        UE_LOG( LogTemp, Warning, TEXT("PTR:%llu"), (uint64) Object );
    }
}

void
UArianePainting3DComponent::PostLoad()
{
    Super::PostLoad();

    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostLoad();
    }

    // Second part, invalidate Path Segments to rebuild the VertexFactory
/*
    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        if( Object->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            FArianePathInvalidationFlags Flags = FArianePathInvalidationFlags();

            Path->Invalidate( Flags.SetAll() );
        }
    }
*/

    //RootObjectID.GetObject()->Update( true );
    Update();
}

void
UArianePainting3DComponent::PostEditUndo()
{
    Super::PostEditUndo();

    // RootObjectID won't have its cache reset after Undoing, we have to force it.
    RootObjectID.InvalidateCache();

    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostEditUndo();
    }

    // Second part, invalidate Path Segments to rebuild the VertexFactory
/*
    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        if( Object->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            FArianePathInvalidationFlags Flags = FArianePathInvalidationFlags();

            Path->Invalidate( Flags.SetAll() );
        }
    }
*/

    //RootObjectID.GetObject()->Update( true );
    Update();
}

FArianeObject*
UArianePainting3DComponent::GetRootObject()
{
    return RootObjectID.GetObject();
}

FArianeObject*
UArianePainting3DComponent::GetObject( const FGuid& InGuid )
{
    for( FInstancedStruct& InstancedObject : InstancedObjects )
    {
        FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

        if( Object->GetGuid() == InGuid )
        {
            return Object;
        }
    }

    return nullptr;
}

FPrimitiveSceneProxy*
UArianePainting3DComponent::CreateSceneProxy()
{
    return new FArianeGeometryProxy( GetScene()->GetFeatureLevel(), this );
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

    for( const FInstancedStruct& InstancedObject : InstancedObjects )
    {
        const FArianeObject* Object = InstancedObject.GetPtr<FArianeObject>();

        if( const_cast<FArianeObject*>(Object)->GetClass() == FArianePath::StaticClass() )
        {
            const FArianePath* Path = static_cast<const FArianePath*>(Object);

            RetBounds = RetBounds + const_cast<FArianePath*>(Path)->GetBounds();
        }
    }

    return RetBounds.TransformBy( LocalToWorld );
}

FArianeObject*
UArianePainting3DComponent::AllocObject()
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();



    return NewObject;
}

FArianePath*
UArianePainting3DComponent::AllocPath( EArianePathLineType InLineType )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianePath>( this, InLineType ) );
    InstancedObjectsAccessRW.Unlock();

    FArianePath* NewPath = InstancedObjects.Last().GetMutablePtr<FArianePath>();


    //RootObjectID.GetObject()->AppendChild( NewPath );

    return NewPath;
}

TArray<FInstancedStruct>&
UArianePainting3DComponent::GetInstancedObjects()
{
    return InstancedObjects;
}

void
UArianePainting3DComponent::Update()
{
    FArianeGeometryProxy* GeometryProxy = static_cast<FArianeGeometryProxy*>(GetSceneProxy());

    RootObjectID.GetObject()->Update( true );

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();

    UsedMaterials.Empty();
    //UsedMaterials.Add( Material );
}

void
UArianePainting3DComponent::PostEditChangeProperty( FPropertyChangedEvent& event )
{
    /*
    if( event.GetPropertyName() == GET_MEMBER_NAME_CHECKED( UArianePainting3DComponent, GeometryMode ) )
    {
        for( FInstancedStruct& InstancedObject : InstancedObjects )
        {
            FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

            if( Object->GetClass() == FArianePath::StaticClass() )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                Path->InvalidateAllSegments();
            }
        }

        RootObjectID.GetObject()->Update( true );
    }
    */

    Super::PostEditChangeProperty( event );
}

void
UArianePainting3DComponent::ResetHierarchy()
{
    InstancedObjects.Empty();
    // Nb: this object will be destroyed automatically when loading from the disc, as the TArray is replaced entirely.
    RootObjectID = FArianeObjectID( AllocObject() );
}


void
UArianePainting3DComponent::DeleteInstancedObject( FArianeObject* Object )
{
    InstancedObjectsAccessRW.Lock();

    InstancedObjects.RemoveAll( [Object]( FInstancedStruct& Struct ) -> bool
    {
        return ( Object == Struct.GetPtr<FArianeObject>() ) ? true : false;
    } );

    InstancedObjectsAccessRW.Unlock();
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

    Painting3DComponent->GetSceneProxy()->SetUsedMaterialForVerification( Painting3DComponent->UsedMaterials );
    Painting3DComponent->InstancedObjectsAccessRW.Lock();

    Painting3DComponent->GetRootObject()->Traverse( [ this
                                                    , &MeshBatch ]( FArianeObject* Object ) -> FArianeObject::TraversalReturnValue
    {
        if( Object->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            FArianePathGeometry3D& Mesh = Path->GetGeometry3D();

            if( Mesh.GetIndexBuffer().GetNumIndices() )
            {
                //UMaterialInterface* MaterialInterface = Painting3DComponent->Material;
                UMaterialInterface* MaterialInterface = UMaterial::GetDefaultMaterial( MD_Surface );

                FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                BatchElement.IndexBuffer = &Mesh.GetIndexBuffer();

                //Mesh.bWireframe = bWireframe;
                MeshBatch.VertexFactory = Mesh.GetVertexFactory();
                MeshBatch.MaterialRenderProxy = MaterialInterface->GetRenderProxy();

                //Additional data
                BatchElement.FirstIndex = 0;
                BatchElement.NumPrimitives = Mesh.GetIndexBuffer().GetNumIndices() / 3;
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = Mesh.GetVertexBuffers().PositionVertexBuffer.GetNumVertices() - 1;

                MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
                MeshBatch.Type = PT_TriangleList;
                MeshBatch.DepthPriorityGroup = SDPG_World;
                MeshBatch.bCanApplyViewModeOverrides = false;
                MeshBatch.bDisableBackfaceCulling = true; // draw both sides
                MeshBatch.CastShadow = false;

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
            }
        }

        return FArianeObject::TraversalReturnValue::Continue;
    } );

    PDI->DrawMesh(MeshBatch, FLT_MAX);

    Painting3DComponent->InstancedObjectsAccessRW.Unlock();
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector) const
{
    Painting3DComponent->GetSceneProxy()->SetUsedMaterialForVerification( Painting3DComponent->UsedMaterials );
    Painting3DComponent->InstancedObjectsAccessRW.Lock();

    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];

        Painting3DComponent->GetRootObject()->Traverse( [ this
                                                        , ViewIndex
                                                        , &Collector ]( FArianeObject* Object ) -> FArianeObject::TraversalReturnValue
        {
            if( ( Object->GetClass() == FArianePath::StaticClass() )  )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);
                FArianePathGeometry3D& Mesh = Path->GetGeometry3D();

                if( Mesh.GetIndexBuffer().GetNumIndices() && Mesh.GetIndexBuffer().IsInitialized() )
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
                    //UMaterialInterface* MaterialInterface = Painting3DComponent->Material;
                    UMaterialInterface* MaterialInterface = UMaterial::GetDefaultMaterial( MD_Surface );

                    // Allocate a mesh batch and get a ref to the first element
                    FMeshBatch& MeshBatch = Collector.AllocateMesh();
                    FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                    BatchElement.IndexBuffer = &Mesh.GetIndexBuffer();

                    //Mesh.bWireframe = bWireframe;
                    MeshBatch.VertexFactory = Mesh.GetVertexFactory();
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
                    BatchElement.NumPrimitives = Mesh.GetIndexBuffer().GetNumIndices() / 3;
                    BatchElement.MinVertexIndex = 0;
                    BatchElement.MaxVertexIndex = Mesh.GetVertexBuffers().PositionVertexBuffer.GetNumVertices() - 1;
                    MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
                    MeshBatch.Type = PT_TriangleList;
                    MeshBatch.DepthPriorityGroup = SDPG_World;
                    MeshBatch.bCanApplyViewModeOverrides = false;
                    MeshBatch.bDisableBackfaceCulling = true; // draw both sides
                    MeshBatch.CastShadow = false;

                    //Add the batch to the collector
                    Collector.AddMesh( ViewIndex, MeshBatch );
                }
            }

            return FArianeObject::TraversalReturnValue::Continue;
        } );
    }

    Painting3DComponent->InstancedObjectsAccessRW.Unlock();
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
