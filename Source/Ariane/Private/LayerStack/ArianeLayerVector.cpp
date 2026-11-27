// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerVector.h"
#include "ArianeDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeVertex.h"
#include "ArianeGroup.h"
#include "ArianeCycle.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"
// Unreal headers
#include "UObject/ObjectSaveContext.h"
#include "UObject/UObjectIterator.h"
#include "Materials/Material.h"
#include "Materials/MaterialRenderProxy.h"

//--------------------------------------------------------------------------------------------------
// Rendering of the vector layer: the render items
//
// The proxy (FArianeGeometryProxy) never reads the Ariane objects: the render thread would read them while
// the game thread changes them. It draws a flat array of FArianeMeshRenderItem instead, one per path or cycle.
//
// Each thread owns its data, nothing is shared:
// - the drawing owns the master items (UArianeDrawing::RenderItems), game thread only;
// - the proxy owns a copy (FArianeGeometryProxy::MeshItems), render thread only. It's copied by CreateSceneProxy,
//   then changed only by render commands (EnqueueRenderItem(s)), executed in the order they were enqueued.
//
// An item holds a copy of what the proxy needs (world matrix, inherited visibility, color, material...) and a
// pointer to the mesh's GPU buffers (Geometry3D), only changed by render commands.
//
// Keeping the items up to date, on the game thread: each change writes the master item, then enqueues a copy.
// - FArianeMesh::UpdateRenderItem fills an item from its mesh. It's called at the end of FArianeObject::Update
//   (RefreshRenderItem, after UpdateBoundingBox, at the mesh's RenderIndex) and by RebuildRenderItems.
// - FArianeObject::UpdateTransform writes the world matrix of the meshes whose matrix it computes.
// - A change of a group that applies below it (visibility, HUD color) invalidates its tree (InvalidateTree):
//   every mesh below updates its own item.
// - A Hierarchy change (mesh added, removed, reordered) rebuilds the items at the end of UArianeDrawing::Update:
//   the RenderIndex are reassigned and the whole array is enqueued.
// - A mesh deleted or reset hides its item, then enqueues the deletion of its Geometry3D: the proxy skips it.
// - Only the displayed drawing knows its layer (SetDrawing detaches the previous one): a detached drawing
//   enqueues nothing and is rebuilt when attached again.
// - The layer settings (visibility, cast shadows) recreate the proxy.
// - r.Ariane.ShowNormals and the editor HUD color rebuild the items of the displayed drawings.
//--------------------------------------------------------------------------------------------------

// for debugging purpose
TAutoConsoleVariable<bool> CVarShowArianeNormals( TEXT("r.Ariane.ShowNormals")
                                                , 0
                                                , TEXT("Show Ariane mesh normals")
                                                , FConsoleVariableDelegate::CreateLambda( []( IConsoleVariable* )
    {
        UArianeLayerVector::RebuildAllRenderItems();
    } ) );

//--------------------------------------------------------------------------------------------------

FArianeMeshRenderItem::FArianeMeshRenderItem()
    : MeshClass ( 0 )
    , Geometry3D ( nullptr )
    , Material ( nullptr )
    , Color ( FLinearColor::Black )
#if WITH_EDITOR
    , HUDColor ( FLinearColor::Black )
#endif
    , WorldMatrix ( FMatrix::Identity )
    , BoundingBox ( ForceInit )
    , bVisible ( false )
{
}

//--------------------------------------------------------------------------------------------------

// Same check as FStaticMeshSceneProxy: a material that can't be used on a static mesh (e.g. Post Process domain)
// is replaced by the default surface material.
// This way Ariane displays what the static mesh export will display.
// static
UMaterialInterface*
FArianeGeometryProxy::GetMeshCompatibleMaterial( UMaterialInterface* MaterialInterface )
{
    if( MaterialInterface == nullptr )
    {
        return nullptr;
    }

    // Only Surface, Deferred Decal and Volume domains pass. In the editor, a missing usage flag
    // is set automatically (as the engine does when the static mesh is rendered)
    if( MaterialInterface->CheckMaterialUsage_Concurrent( MATUSAGE_StaticMesh ) == false )
    {
        return UMaterial::GetDefaultMaterial( MD_Surface );
    }

    return MaterialInterface;
}

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

// Note: The proxy is created via CreateSceneProxy, and will be recreated everytime MarkRenderStateDirty() is called
FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianeLayerVector* InVectorLayer
                                          , const TArray<FArianeMeshRenderItem>& InMeshItems )
    : FPrimitiveSceneProxy ( InVectorLayer )
    , MeshItems ( InMeshItems )
    , bLayerVisible ( InVectorLayer->IsVisible() )
    , bCastShadows ( InVectorLayer->GetLayerStack()->GetPainting3DComponent()->GetCastShadows() )
{
    EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform_Checked( InFeatureLevel );
    TArray<UMaterialInterface*> MaterialInterfaces;

    // On the game thread: the proxy is constructed by CreateSceneProxy
    InVectorLayer->GetDrawing()->GetUsedMaterials( MaterialInterfaces );

    // MaterialRelevance is used by GetViewRelevance and is necessary to render all kinds of materials
    for( int32 Index = 0; Index < MaterialInterfaces.Num(); Index++ )
    {
        // replaced in the array too, so that SetUsedMaterialForVerification knows the default material
        MaterialInterfaces[Index] = GetMeshCompatibleMaterial( MaterialInterfaces[Index] );

        if( MaterialInterfaces[Index] )
        {
            MaterialRelevance |=  MaterialInterfaces[Index]->GetRelevance_Concurrent( ShaderPlatform );
        }
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
FArianeGeometryProxy::SetMeshItem_RenderThread( int32 Index, const FArianeMeshRenderItem& Item )
{
    MeshItems[Index] = Item;
}

void
FArianeGeometryProxy::SetMeshItems_RenderThread( TArray<FArianeMeshRenderItem>&& InMeshItems )
{
    MeshItems = MoveTemp( InMeshItems );
}

void
FArianeGeometryProxy::DrawMeshItem( const FArianeMeshRenderItem& Item
                                  , const FSceneViewFamily& ViewFamily
                                  , int32 ViewIndex
                                  , FMeshElementCollector& Collector ) const
{
    const FSceneView* View = ViewFamily.Views[ViewIndex];
    FPrimitiveDrawInterface* PDI = Collector.GetPDI( ViewIndex );
    bool bWireframe = AllowDebugViewmodes() && ViewFamily.EngineShowFlags.Wireframe;
    UMaterialInterface* MaterialInterface = Item.Material;
    FLinearColor ItemColor = Item.Color;
    // Null until the mesh's first build in a world (UArianeDrawing::InitVertexFactories): then never changed
    FLocalVertexFactory* VertexFactory = Item.Geometry3D->GetVertexFactory();

    if( bWireframe )
    {
        MaterialInterface = GEngine->WireframeMaterial;

#if WITH_EDITOR
        // The HUD colors are editor settings: a packaged game keeps the object color
        ItemColor = Item.HUDColor;
#endif
    }

    if( MaterialInterface
     && VertexFactory
     && MaterialInterface->GetRenderProxy()
     && Item.Geometry3D->GetIndexBuffer().GetNumIndices()
     && Item.Geometry3D->GetIndexBuffer().IsInitialized() )
    {
        // Allocate a mesh batch and get a ref to the first element
        FMeshBatch& MeshBatch = Collector.AllocateMesh();
        FMeshBatchElement& BatchElement = MeshBatch.Elements[0];

        BatchElement.IndexBuffer = &Item.Geometry3D->GetIndexBuffer();

        // Temporary proxy to store the color of the object (lighter than a UMaterialInstanceDynamic).
        // Owned by the collector, released at the end of the frame
        FColoredMaterialRenderProxy& MaterialProxy = Collector.AllocateOneFrameResource<FColoredMaterialRenderProxy>(
            MaterialInterface->GetRenderProxy()
          , ItemColor );

        MeshBatch.bWireframe = bWireframe;
        MeshBatch.VertexFactory = VertexFactory;
        MeshBatch.MaterialRenderProxy = &MaterialProxy;

        //The LocalVertexFactory uses a uniform buffer to pass primitve data like the local to world transform for this frame and for the previous one
        //Most of this data can be fetched using the helper function below
        bool bHasPrecomputedVolumetricLightmap;
        FMatrix PreviousLocalToWorld;
        int32 SingleCaptureIndex;
        bool bOutputVelocity;

        FMatrix ItemWorldMatrix = Item.WorldMatrix;

        // against Z-fighting. at each frame we move the cylce away from the screen
        // always in the direction of the camera.
        if( Item.MeshClass == FArianeCycle::StaticClass() )
        {
            if( View->IsPerspectiveProjection() )
            {
                const FVector C = View->ViewMatrices.GetViewOrigin();
                const float DepthEpsilon = 1e-4;
                const float s = 1.f + DepthEpsilon;
                // note: the scaling is centered on the camera.
                ItemWorldMatrix.SetAxis( 0, ItemWorldMatrix.GetScaledAxis( EAxis::X ) * s );
                ItemWorldMatrix.SetAxis( 1, ItemWorldMatrix.GetScaledAxis( EAxis::Y ) * s );
                ItemWorldMatrix.SetAxis( 2, ItemWorldMatrix.GetScaledAxis( EAxis::Z ) * s );
                ItemWorldMatrix.SetOrigin( ItemWorldMatrix.GetOrigin() * s + C * ( 1.f - s ) );
            }
            else // orthographic
            {
                double OrthoOffset = 5.0f;
                // Note: no scaling required in orthographic projection
                ItemWorldMatrix.SetOrigin( ItemWorldMatrix.GetOrigin() + View->GetViewDirection() * OrthoOffset );
            }
        }

        GetScene().GetPrimitiveUniformShaderParameters_RenderThread( GetPrimitiveSceneInfo()
                                                                   , bHasPrecomputedVolumetricLightmap
                                                                   , PreviousLocalToWorld
                                                                   , SingleCaptureIndex
                                                                   , bOutputVelocity );

        //Alloate a temporary primitive uniform buffer, fill it with the data and set it in the batch element
        FDynamicPrimitiveUniformBuffer& DynamicPrimitiveUniformBuffer = Collector.AllocateOneFrameResource<FDynamicPrimitiveUniformBuffer>();

        DynamicPrimitiveUniformBuffer.Set( Collector.GetRHICommandList()
                                         , ItemWorldMatrix
                                         , PreviousLocalToWorld
                                         , FBoxSphereBounds ( Item.BoundingBox.TransformBy( ItemWorldMatrix ) )
                                         , FBoxSphereBounds ( Item.BoundingBox )
                                         , true
                                         , bHasPrecomputedVolumetricLightmap
                                         , bOutputVelocity );

        BatchElement.PrimitiveUniformBufferResource = &DynamicPrimitiveUniformBuffer.UniformBuffer;
        BatchElement.PrimitiveIdMode = PrimID_DynamicPrimitiveShaderData;

        //Additional data
        BatchElement.FirstIndex = 0;
        BatchElement.NumPrimitives = Item.Geometry3D->GetIndexBuffer().GetNumIndices() / 3;
        BatchElement.MinVertexIndex = 0;
        BatchElement.MaxVertexIndex = Item.Geometry3D->GetVertexCount() - 1;

        MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
        MeshBatch.Type = PT_TriangleList;
        MeshBatch.DepthPriorityGroup = SDPG_World;
        MeshBatch.bCanApplyViewModeOverrides = false;
        MeshBatch.bDisableBackfaceCulling = true; // draw both sides
        MeshBatch.CastShadow = bCastShadows;

        //Add the batch to the collector
        Collector.AddMesh( ViewIndex, MeshBatch );
    }

    // for debugging purpose (flag "r.Ariane.ShowNormals"): computed on the game thread
    for( const FRay3f& NormalRay : Item.NormalRays )
    {
        PDI->DrawLine( Item.WorldMatrix.TransformPosition( FVector( NormalRay.Origin ) )
                     , Item.WorldMatrix.TransformPosition( FVector( NormalRay.PointAt( 200.0f ) ) )
                     , FLinearColor::Green
                     , SDPG_World
                     , 1.0f );
    }
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector ) const
{
    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        if( bLayerVisible )
        {
            for( const FArianeMeshRenderItem& Item : MeshItems )
            {
                // Invisible: hidden, or a deleted mesh whose Geometry3D must not be read
                if( Item.bVisible )
                {
                    DrawMeshItem( Item, ViewFamily, ViewIndex, Collector );
                }
            }
        }

        // Render bounds manually because it's a bit complicated to render them when using custom proxies like this one.
        // The proxy's bounds: the layer is a game thread object
        if( ViewFamily.EngineShowFlags.Bounds )
        {
            RenderBounds( Collector.GetPDI( ViewIndex ), ViewFamily.EngineShowFlags, GetBounds(), true );
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

//--------------------------------------------------------------------------------------------------

UArianeLayerVector::~UArianeLayerVector()
{
}

UArianeLayerVector::UArianeLayerVector()
    : Drawing( nullptr )
    , bOverrideDrawingSettings( false )
    , DrawingDistanceToPlane ( 0.0f )
    , DrawingOrigin ( EArianeDrawingOrigin::Layer )
    , DrawingOrientation ( EArianeDrawingOrientation::View )
{
    bWantsOnUpdateTransform = true;

    //Drawing = CreateDefaultSubobject<UArianeDrawing>("Subobject Drawing");

    //Drawing->SetVectorLayer( this );

    //bWantsInitializeComponent = true;

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

void UArianeLayerVector::PreSave(FObjectPreSaveContext Context)
{
    Super::PreSave(Context);
}

void
UArianeLayerVector::Serialize(FArchive& Ar )
{
    // for some currently UNKOWN reason, the UPROPERTY Drawing is set to Transient when loading the asset as
    // a spawnable from the level sequence. We have to clear the TRANSIENT flag if we wan't the image to be
    // saved in case there is a modification via ArianeEditor. the question should be asked to Epic Games.
    // That took days to troubleshoot :(
    if ( Ar.IsSaving() && Drawing )
    {
        Drawing->ClearFlags(RF_Transient);
    }

    Super::Serialize(Ar);
}

void
UArianeLayerVector::PostLoad()
{
    Super::PostLoad();
}

void
UArianeLayerVector::OnRegister()
{
    UArianeDrawing* DefaultDrawing = Drawing;

    Super::OnRegister();

    // Force computation of the world Transform in FArianeObject::UpdateTransform
    WorldTransformVersion++;

    if ( DefaultDrawing == nullptr )
    {
        DefaultDrawing = NewObject<UArianeDrawing>(this, TEXT("DefaultDrawing"), RF_Transactional | RF_Public);
    }

    SetDrawing( DefaultDrawing, false );

    Drawing->OnRegisterLayer();
}

void
UArianeLayerVector::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials ) const
{
    Drawing->GetUsedMaterials( OutUsedMaterials, bGetDebugMaterials );
}

FPrimitiveSceneProxy*
UArianeLayerVector::CreateSceneProxy()
{
    // The world exists now: the meshes built before have no vertex factory
    Drawing->InitVertexFactories();

    return new FArianeGeometryProxy( GetScene()->GetFeatureLevel(), this, Drawing->GetRenderItems() );
}

void
UArianeLayerVector::EnqueueRenderItem( int32 Index, const FArianeMeshRenderItem& Item )
{
    FArianeGeometryProxy* Proxy = static_cast<FArianeGeometryProxy*>( SceneProxy );

    // The proxy is destroyed by a render command too: this one runs before
    if( Proxy )
    {
        ENQUEUE_RENDER_COMMAND( ArianeEnqueueRenderItem )(
            [ Proxy, Index, Item ]( FRHICommandListImmediate& RHICmdList )
            {
                Proxy->SetMeshItem_RenderThread( Index, Item );
            } );
    }
}

void
UArianeLayerVector::EnqueueRenderItems( const TArray<FArianeMeshRenderItem>& Items )
{
    FArianeGeometryProxy* Proxy = static_cast<FArianeGeometryProxy*>( SceneProxy );

    if( Proxy )
    {
        ENQUEUE_RENDER_COMMAND( ArianeEnqueueRenderItems )(
            [ Proxy, ItemsCopy = CopyTemp( Items ) ]( FRHICommandListImmediate& RHICmdList ) mutable
            {
                Proxy->SetMeshItems_RenderThread( MoveTemp( ItemsCopy ) );
            } );
    }
}

// static
void
UArianeLayerVector::RebuildAllRenderItems()
{
    // The detached drawings are rebuilt when attached (SetDrawing)
    for( TObjectIterator<UArianeLayerVector> It; It; ++It )
    {
        UArianeDrawing* LayerDrawing = It->GetDrawing();

        // Null for the CDO and the layers never registered
        if( LayerDrawing )
        {
            LayerDrawing->RebuildRenderItems();
        }
    }
}

FBoxSphereBounds
UArianeLayerVector::CalcBounds( const FTransform& LocalToWorld ) const
{
    FBoxSphereBounds RetBounds = FBoxSphereBounds(ForceInit);

    if( Drawing )
    {
        RetBounds = Drawing->GetRootGroup()->GetBoundingBox();
    }

    return FBoxSphereBounds( RetBounds.TransformBy( LocalToWorld ) );
}

void
UArianeLayerVector::OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType )
{
    Super::OnUpdateTransform( UpdateTransformFlags, TeleportType );

    Drawing->GetRootGroup()->UpdateTransform();
}

void
UArianeLayerVector::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    Drawing->Update( bInteractive );

    Super::Update( bInteractive );

    // The root group may stay invalidated on purpose: the layer must then remain in its parent's list
    bInvalidated = Drawing->GetRootGroup()->IsInvalidated();
}

void
UArianeLayerVector::OnRootObjectInvalidated()
{
    Invalidate();
}

/*
FBoxSphereBounds
UArianeLayerVector::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( Drawing->GetRootGroup()->GetBoundingBox().TransformBy( LocalToWorld ) );
}
*/

bool UArianeLayerVector::GetOverrideDrawingSettings() const
{
    return bOverrideDrawingSettings;
}

void UArianeLayerVector::SetOverrideDrawingSettings(bool bInOverrideDrawingSettings)
{
    bOverrideDrawingSettings = bInOverrideDrawingSettings;
}

void
UArianeLayerVector::SetDrawingDistanceToPlane( float InDrawingDistanceToPlane )
{
    DrawingDistanceToPlane = InDrawingDistanceToPlane;
}

float
UArianeLayerVector::GetDrawingDistanceToPlane()
{
    // should not de reached anyways
    return DrawingDistanceToPlane;
}

EArianeDrawingOrigin
UArianeLayerVector::GetDrawingOrigin()
{
    return DrawingOrigin;
}

void
UArianeLayerVector::SetDrawingOrigin( EArianeDrawingOrigin InDrawingOrigin )
{
    DrawingOrigin = InDrawingOrigin;
}

EArianeDrawingOrientation
UArianeLayerVector::GetDrawingOrientation()
{
    return DrawingOrientation;
}

void
UArianeLayerVector::SetDrawingOrientation( EArianeDrawingOrientation InDrawingOrientation )
{
    DrawingOrientation = InDrawingOrientation;
}

UArianeDrawing*
UArianeLayerVector::GetDrawing()
{
    return Drawing;
}

void
UArianeLayerVector::SetDrawing( UArianeDrawing* InDrawing, bool bTriggerEvent )
{
    if( bTriggerEvent )
        OnPreDrawingChanged.Broadcast();

    // Only the displayed drawing knows its layer. Null on the first call (OnRegister creates the default drawing)
    if( Drawing )
    {
        Drawing->SetVectorLayer( nullptr );
    }

    Drawing = InDrawing;
    Drawing->SetVectorLayer( this );
    // Changed while detached (Hierarchy, r.Ariane.ShowNormals, editor HUD color)
    Drawing->RebuildRenderItems();

    MarkRenderStateDirty();

    if( bTriggerEvent )
        OnPostDrawingChanged.Broadcast();
}

UArianeLayerVector::FOnDrawingChanged&
UArianeLayerVector::OnPreDrawingChangedDelegate()
{
    return OnPreDrawingChanged;
}

UArianeLayerVector::FOnDrawingChanged&
UArianeLayerVector::OnPostDrawingChangedDelegate()
{
    return OnPostDrawingChanged;
}

void
UArianeLayerVector::PropertyChanged( const FName& PropertyName
                                    , const FName& MemberPropertyName
                                    , const FName& Category )
{
    Super::PropertyChanged( PropertyName, MemberPropertyName, Category );

    /*if( MemberPropertyName == GET_MEMBER_NAME_CHECKED(USceneComponent, RelativeLocation) )
    {

    }*/
}
