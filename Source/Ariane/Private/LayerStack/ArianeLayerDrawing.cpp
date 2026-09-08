// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerDrawing.h"
#include "ArianeImage.h"
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

// for debugging purpose
static TAutoConsoleVariable<bool> CVarShowArianeNormals( TEXT("r.Ariane.ShowNormals")
                                                       , 0
                                                       , TEXT("Show Ariane mesh normals") );

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

// Note: The proxy is created via CreateSceneProxy, and will be recreated everytime MarkRenderStateDirty() is called
FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianeLayerDrawing* InDrawingLayer )
    : FPrimitiveSceneProxy ( InDrawingLayer )
    , DrawingLayer ( InDrawingLayer )
{
    EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform_Checked( InFeatureLevel );
    TArray<UMaterialInterface*> MaterialInterfaces;

    DrawingLayer->GetImage()->GetUsedMaterials( MaterialInterfaces );

    FArianeObject::Traverse( DrawingLayer->GetImage()->GetRootGroup()
                           , []( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>( Object );

                Path->GetGeometry3D().InitVertexFactory();
            }

            if( Object->HasBaseClass( FArianeCycle::StaticClass() ) )
            {
                FArianeCycle* Cycle = static_cast<FArianeCycle*>( Object );

                Cycle->GetGeometry3D().InitVertexFactory();
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );

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

// static
void
FArianeGeometryProxy::GetImageDynamicMeshElements( FMeshElementCollector& Collector
                                                 , int32 ViewIndex ) const
{
    FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);
    //UMaterialInterface* MaterialInterface = GEngine->VertexColorMaterial;
    UArianeImage* Image = DrawingLayer->GetImage();
    FArianeGroup* RootGroup = Image->GetRootGroup();

    Image->InstancedObjectsAccessRW.Lock();

    FArianeObject::Traverse( RootGroup
                           , [ this
                             , PDI
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

    Image->InstancedObjectsAccessRW.Unlock();
}

void
FArianeGeometryProxy::GetDynamicMeshElements( const TArray<const FSceneView*>& Views
                                            , const FSceneViewFamily& ViewFamily
                                            , uint32 VisibilityMap
                                            , FMeshElementCollector& Collector ) const
{
    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];

        if( DrawingLayer )
        {
            if( DrawingLayer->IsVisible() )
            {
                GetImageDynamicMeshElements( Collector
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

//--------------------------------------------------------------------------------------------------

UArianeLayerDrawing::~UArianeLayerDrawing()
{
    delete InvalidationFlags;
}

UArianeLayerDrawing::UArianeLayerDrawing()
    : Image( nullptr )
    , DrawingOrigin ( EArianeLayerDrawingOrigin::Layer )
    , DrawingOrientation ( EArianeLayerDrawingOrientation::View )
{
    InvalidationFlags = new FArianeLayerInvalidationFlags();

    Image = CreateDefaultSubobject<UArianeImage>( "Default Image" );

    SetImage( Image );

    bWantsOnUpdateTransform = true;

    FCoreUObjectDelegates::OnAssetLoaded.AddUObject( this, &UArianeLayerDrawing::OnAssetLoaded );

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

void
UArianeLayerDrawing::BeginDestroy()
{
    FCoreUObjectDelegates::OnAssetLoaded.RemoveAll( this );

    Super::BeginDestroy();
}

void
UArianeLayerDrawing::OnAssetLoaded(UObject* LoadedObject)
{
    // Note: . Our FArianeObjects Transforms depend on the
    // Layer (to compute the world Transform). but, when PostLoad is called, the Transforms are not set yet
    // so we update the Transforms for our ArianeObjects after the Asset is loaded
    if ( GetOutermost() )
    {
        Image->GetRootGroup()->UpdateTransform();
    }
}

void
UArianeLayerDrawing::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials ) const
{
    Image->GetUsedMaterials( OutUsedMaterials, bGetDebugMaterials );
}

FPrimitiveSceneProxy*
UArianeLayerDrawing::CreateSceneProxy()
{
    return new FArianeGeometryProxy( GetScene()->GetFeatureLevel(), this );
}

FBoxSphereBounds
UArianeLayerDrawing::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( Image->GetRootGroup()->GetBoundingBox().TransformBy( LocalToWorld ) );
}

void
UArianeLayerDrawing::OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType )
{
    Super::OnUpdateTransform( UpdateTransformFlags, TeleportType );

    Image->GetRootGroup()->UpdateTransform();
}

void
UArianeLayerDrawing::OnRegister()
{
    Super::OnRegister();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }

    WorldTransformVersion++;
}

void
UArianeLayerDrawing::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    Image->Update( bInteractive );

    Super::Update( bInteractive );
}

void
UArianeLayerDrawing::OnRootObjectInvalidated()
{
    Invalidate( FArianeLayerInvalidationFlags() );
}

/*
FBoxSphereBounds
UArianeLayerDrawing::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( Image->GetRootGroup()->GetBoundingBox().TransformBy( LocalToWorld ) );
}
*/

EArianeLayerDrawingOrigin
UArianeLayerDrawing::GetDrawingOrigin()
{
    return DrawingOrigin;
}

void
UArianeLayerDrawing::SetDrawingOrigin( EArianeLayerDrawingOrigin InDrawingOrigin )
{
    DrawingOrigin = InDrawingOrigin;
}

EArianeLayerDrawingOrientation
UArianeLayerDrawing::GetDrawingOrientation()
{
    return DrawingOrientation;
}

void
UArianeLayerDrawing::SetDrawingOrientation( EArianeLayerDrawingOrientation InDrawingOrientation )
{
    DrawingOrientation = InDrawingOrientation;
}

UArianeImage*
UArianeLayerDrawing::GetImage()
{
    return Image;
}

void
UArianeLayerDrawing::SetImage( UArianeImage* InImage, bool bTriggerEvent )
{
    if( bTriggerEvent )
        OnPreImageChanged.Broadcast();

    Image = InImage;
    Image->SetDrawingLayer( this );

    MarkRenderStateDirty();

    if( bTriggerEvent )
        OnPostImageChanged.Broadcast();
}

UArianeLayerDrawing::FOnImageChanged&
UArianeLayerDrawing::OnPreImageChangedDelegate()
{
    return OnPreImageChanged;
}

UArianeLayerDrawing::FOnImageChanged&
UArianeLayerDrawing::OnPostImageChangedDelegate()
{
    return OnPostImageChanged;
}
