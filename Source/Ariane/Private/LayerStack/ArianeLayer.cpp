// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"

#include "ArianeCycle.h"
#include "ArianeVertex.h"
#include "ArianePath.h"
#include "ArianeGroup.h"

// for debugging purpose
static TAutoConsoleVariable<bool> CVarShowArianeNormals( TEXT("r.Ariane.ShowNormals")
                                                       , 0
                                                       , TEXT("Show Ariane mesh normals") );

UArianeLayer::~UArianeLayer()
{
}

UArianeLayer::UArianeLayer()
    : //bVisible ( true )
      ParentFolder( nullptr )
    , bLocked ( false )
    , bSelected ( false )
    , bInvalidatedInParentFolder ( false )
    , InvalidationFlags ( nullptr )
{
    // for Transform operations
    SetMobility(EComponentMobility::Movable);

    bWantsOnUpdateTransform = true;
/*
    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;
*/
}

UArianeLayerFolder*
UArianeLayer::GetRootFolder()
{
    UArianeLayerFolder* Candidate = this->GetParentFolder();

    while ( Candidate )
    {
        if( Candidate->GetParentFolder() == nullptr )
        {
            return Candidate;
        }

        Candidate = Candidate->GetParentFolder();
    }

    return nullptr;
}

UArianeLayerStack*
UArianeLayer::GetLayerStack()
{
    AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(GetOwner());

    // Painting3DActor can be null if the layer is copied in the clipboard
    return Painting3DActor ? Painting3DActor->GetPainting3DComponent()->GetLayerStack() : nullptr;
    //return Cast<UArianeLayerStack>(GetOuter());
}

#if WITH_EDITOR
void
UArianeLayer::PreEditUndo()
{
    Super::PreEditUndo();
}

void
UArianeLayer::PostEditUndo()
{
    Super::PostEditUndo();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }
}
#endif

void
UArianeLayer::PostLoad()
{
    Super::PostLoad();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }
}

/*
void
UArianeLayer::SetVisible( bool bInVisible )
{
    bVisible = bInVisible;
}

bool
UArianeLayer::IsVisible( bool bHierarchical )
{
    UArianeLayerFolder* ParentLayer = GetParent();

    if( bHierarchical && ParentLayer )
    {
        return ParentLayer->IsVisible( bHierarchical ) && bVisible;
    }

    return bVisible;
}
*/

void
UArianeLayer::SetLocked( bool bInLocked )
{
    bLocked = bInLocked;
}

bool
UArianeLayer::IsLocked( bool bHierarchical )
{
    UArianeLayerFolder* ParentLayer = GetParentFolder();

    if( bHierarchical && ParentLayer )
    {
        return ParentLayer->IsLocked( bHierarchical ) || bLocked;
    }

    return bLocked;
}

#if WITH_EDITOR
void
UArianeLayer::SetSelected( bool bInSelected )
{
    bSelected = bInSelected;
}

bool
UArianeLayer::IsSelectedInEditor() const
{
    return bSelected;
}
#endif

void
UArianeLayer::SetParentFolder( UArianeLayerFolder* InParentFolder )
{
    ParentFolder = InParentFolder;
}

UArianeLayerFolder*
UArianeLayer::GetParentFolder()
{
    return ParentFolder;
}

void
UArianeLayer::Invalidate( const FArianeLayerInvalidationFlags& InInvalidationFlags )
{
    if( ParentFolder && ( bInvalidatedInParentFolder == false ) )
    {
        ParentFolder->InvalidateChildLayer( this );

        bInvalidatedInParentFolder = true;
    }

    InvalidationFlags->OR( InInvalidationFlags );

    //GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();
    MarkPackageDirty();
}

void UArianeLayer::SetInvalidatedInParentFolder( bool bInInvalidatedInParentFolder )
{
    bInvalidatedInParentFolder = bInInvalidatedInParentFolder;
}

FArianeLayerInvalidationFlags*
UArianeLayer::GetInvalidationFlags()
{
    return InvalidationFlags;
}

FBoxSphereBounds
UArianeLayer::CalcBounds( const FTransform& LocalToWorld ) const
{
    // ForceInit makes the box invalid and excludes it from the computation unitl it is valid
    FBoxSphereBounds NewBounds = FBoxSphereBounds(ForceInit);

    return NewBounds.TransformBy(LocalToWorld);
}

const FBoxSphereBounds&
UArianeLayer::GetBounds()
{
    return Bounds;
}

void
UArianeLayer::Update( bool Interactive )
{
    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();
}

UArianeLayer::FOnTransformChanged&
UArianeLayer::GetOnTransformChangedDelegate()
{
    return OnTransformChanged;
}

void
UArianeLayer::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
    OnTransformChanged.Broadcast();
}

UArianeLayer::ETraversalReturnValue
UArianeLayer::TraverseBackwards_Private( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    ETraversalReturnValue Ret = Callback( this );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ParentFolder )
    {
        ETraversalReturnValue ParentRet = ParentFolder->TraverseBackwards_Private( Callback );

        if( ParentRet == ETraversalReturnValue::Stop )
        {
            return ParentRet;
        }
    }

    return Ret;
}

void
UArianeLayer::TraverseBackwards( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    TraverseBackwards_Private( Callback );
}

FPrimitiveSceneProxy*
UArianeLayer::CreateSceneProxy()
{
    return new FArianeGeometryProxy( GetScene()->GetFeatureLevel(), this );
}

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

// Note: The proxy is created via CreateSceneProxy, and will be recreated everytime MarkRenderStateDirty() is called
FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianeLayer* InLayer )
    : FPrimitiveSceneProxy ( InLayer )
    , Layer ( InLayer )
{
    EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform_Checked( InFeatureLevel );
    TArray<UMaterialInterface*> MaterialInterfaces;

    Layer->GetLayerStack()->GetPainting3DComponent()->GetUsedMaterials( MaterialInterfaces );

    // MaterialRelevance is used by GetViewRelevance and is necessary to render all kinds of materials
    for( UMaterialInterface* MaterialInterface : MaterialInterfaces )
    {
        MaterialRelevance |=  MaterialInterface->GetRelevance_Concurrent( ShaderPlatform );
    }

    SetUsedMaterialForVerification( MaterialInterfaces );
}

void
FArianeGeometryProxy::DrawStaticElements( FStaticPrimitiveDrawInterface * PDI )
{
// commented out. This is called only once at engine start, then only GetDynamicMeshElements gets called.
// however we keep it commented just in case we'd need it some day.
/*
    UMaterialInterface* MaterialInterface = GEngine->VertexColorMaterial;

    Painting3DComponent->GetSceneProxy()->SetUsedMaterialForVerification( Painting3DComponent->GetUsedMaterials() );
    Painting3DComponent->InstancedObjectsAccessRW.Lock();

    Painting3DComponent->GetRootObject()->Traverse( [ this
                                                    , MaterialInterface
                                                    , PDI ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
    {
        if( Object->GetClass() == FArianePath::StaticClass() )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            FArianePathGeometry3D& Mesh = Path->GetGeometry3D();

            if( MaterialInterface
             && MaterialInterface->GetRenderProxy()
             && Path->IsVisible( true )
             && Mesh.GetVertexCount()
             && Mesh.GetIndexBuffer().GetNumIndices()
             && Mesh.GetIndexBuffer().IsInitialized() )
            {
                FMeshBatch MeshBatch;
                FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                BatchElement.IndexBuffer = &Mesh.GetIndexBuffer();

                //Mesh.bWireframe = bWireframe;
                MeshBatch.VertexFactory = Mesh.GetVertexFactory();
                MeshBatch.MaterialRenderProxy = MaterialInterface->GetRenderProxy();

                BatchElement.PrimitiveUniformBuffer = GetUniformBuffer();

                //Additional data
                BatchElement.FirstIndex = 0;
                BatchElement.NumPrimitives = Mesh.GetIndexBuffer().GetNumIndices() / 3;
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = Mesh.GetVertexCount() - 1;

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
                MeshBatch.bUseForMaterial = 1;
                MeshBatch.bDitheredLODTransition = 0;
                MeshBatch.bRenderToVirtualTexture = 0;

                PDI->DrawMesh(MeshBatch, FLT_MAX);
            }
        }

        return FArianeObject::ETraversalReturnValue::Continue;
    } );

    Painting3DComponent->InstancedObjectsAccessRW.Unlock();
*/
}

void
FArianeGeometryProxy::GetDrawingLayerDynamicMeshElements( UArianeLayerDrawing* DrawingLayer
                                                        , FMeshElementCollector& Collector
                                                        , int32 ViewIndex ) const
{
    FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
    //UMaterialInterface* MaterialInterface = GEngine->VertexColorMaterial;
    FArianeGroup* RootGroup = DrawingLayer->GetRootGroup();

    DrawingLayer->InstancedObjectsAccessRW.Lock();

    FArianeObject::Traverse( RootGroup
                           , [ this
                             , PDI
                             , DrawingLayer
                             , ViewIndex
                             , &Collector ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
    {

        FArianeObjectGeometry3D* Geometry3D = nullptr;
        UMaterialInterface* MaterialInterface = nullptr;

        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);

            Geometry3D = &Path->GetGeometry3D();
            MaterialInterface = Path->GetMaterial();
        }

        if( Object->HasBaseClass( FArianeCycle::StaticClass() ) )
        {
            FArianeCycle* Cycle = static_cast<FArianeCycle*>(Object);

            Geometry3D = &Cycle->GetGeometry3D();
            MaterialInterface = Cycle->GetMaterial();
        }

        if( Geometry3D && MaterialInterface )
        {
            if( MaterialInterface->GetRenderProxy()
             && Object->IsVisible( true )
             //&& Path->GetSegments().Num()
             && Geometry3D->GetIndexBuffer().GetNumIndices()
             && Geometry3D->GetIndexBuffer().IsInitialized() )
            {
                // Allocate a mesh batch and get a ref to the first element
                FMeshBatch& MeshBatch = Collector.AllocateMesh();
                FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

                BatchElement.IndexBuffer = &Geometry3D->GetIndexBuffer();

                //Mesh.bWireframe = bWireframe;
                MeshBatch.VertexFactory = Geometry3D->GetVertexFactory();
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
                                                 , Object->GetTransform().ToMatrixWithScale()
                                                 //, DrawingLayer->GetComponentToWorld().ToMatrixWithScale() //GetLocalToWorld()
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
                BatchElement.NumPrimitives = Geometry3D->GetIndexBuffer().GetNumIndices() / 3;
                BatchElement.MinVertexIndex = 0;
                BatchElement.MaxVertexIndex = Geometry3D->GetVertexCount() - 1;

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

        if( Object->HasBaseClass( FArianePath::StaticClass() ) )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);

            // for debugging purpose (flag "r.Ariane.ShowNormals")
            if ( CVarShowArianeNormals.GetValueOnRenderThread() )
            {
                const FTransform& PathTransform = Path->GetTransform();

                for ( FArianeVertexID& VertexID : Path->GetVertices() )
                {
                    FArianeVertex* Vertex = VertexID.GetVertex();
                    FVector VertexWorldPosition = PathTransform.TransformPosition( Vertex->GetPosition() );
                    FVector VertexWorldNormal = PathTransform.TransformVector( Vertex->GetNormal() );

                    PDI->DrawLine( VertexWorldPosition
                                 , VertexWorldPosition + ( VertexWorldNormal * 200.0f )
                                 , FLinearColor::Green
                                 , SDPG_World
                                 , 1.0f );
                }
            }
        }

        return FArianeObject::ETraversalReturnValue::Continue;
    } );

    DrawingLayer->InstancedObjectsAccessRW.Unlock();
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector ) const
{
    UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];

        if( DrawingLayer )
        {
            if( DrawingLayer->IsVisible() )
            {
                GetDrawingLayerDynamicMeshElements( DrawingLayer
                                                  , Collector
                                                  , ViewIndex );
            }

            // Render bounds manually because it's a bit complicated to render them when using custom proxies like this one.
            if ( ViewFamily.EngineShowFlags.Bounds )
            {
                RenderBounds(
                    Collector.GetPDI(ViewIndex),
                    ViewFamily.EngineShowFlags,
                    DrawingLayer->GetBounds(),
                    true
                );
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
    Result.bOpaque = true;

    Result.bRenderInMainPass = ShouldRenderInMainPass();
    Result.bUsesLightingChannels = GetLightingChannelMask() != GetDefaultLightingChannelMask();
    Result.bRenderCustomDepth = ShouldRenderCustomDepth();
    Result.bTranslucentSelfShadow = bCastVolumetricTranslucentShadow;

    MaterialRelevance.SetPrimitiveViewRelevance( Result );

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
