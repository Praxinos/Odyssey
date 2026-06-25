// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeSegment.h"
#include "ArianeVertex.h"
#include "ArianeGroup.h"
#include "ArianeLayerStack.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
// Unreal headers
#include "Engine/EngineBaseTypes.h"
#include "StaticMeshResources.h"
#include "RenderResource.h"
#include "MeshBatch.h"
#include "Kismet/KismetMathLibrary.h"
#include "RawIndexBuffer.h"
#include "Materials/MaterialRenderProxy.h"
#include "HAL/IConsoleManager.h"

// testing
#include "Components/LineBatchComponent.h"

// for debugging purpose
static TAutoConsoleVariable<bool> CVarShowArianeNormals( TEXT("r.Ariane.ShowNormals")
                                                       , 0
                                                       , TEXT("Show Ariane mesh normals") );

UArianePainting3DComponent::~UArianePainting3DComponent()
{
}

UArianePainting3DComponent::UArianePainting3DComponent()
    : LayerStack ( nullptr )
    , CurrentPaletteColorEntry ( nullptr )
    , EditorInterface ( nullptr )
{
    LayerStack = CreateDefaultSubobject<UArianeLayerStack>(TEXT("LayerStack"));

    LayerStack->SetupAttachment( this );

    PrimaryComponentTick.bCanEverTick = true;
    PrimaryComponentTick.bStartWithTickEnabled = true;
    PrimaryComponentTick.SetTickFunctionEnable(true);

    bAutoRegister = true;
    bWantsInitializeComponent = true;
    bAutoActivate = true;
    bTickInEditor = true;

/*
    SelectionOverrideDelegate.BindLambda([](const UPrimitiveComponent*) {
        return false; // don't draw the outline
    });
*/

    //LineBatchComponent = CreateDefaultSubobject<ULineBatchComponent>(TEXT("LineBatcher"));
}

#if WITH_EDITOR
void
UArianePainting3DComponent::SetEditorInterface( IArianePainting3DComponentEditorInterface* InEditorInterface )
{
    EditorInterface = InEditorInterface;
}
#endif

void
UArianePainting3DComponent::OnRegister()
{
    Super::OnRegister();

    //UsedMaterials.Add( GEngine->VertexColorMaterial );

    //SetMaterial( 0, GEngine->VertexColorMaterial );
}

void
UArianePainting3DComponent::GetUsedMaterials( TArray<UMaterialInterface*>& OutMaterials, bool bGetDebugMaterials ) const
{
    //OutMaterials.Append( UsedMaterials );

    LayerStack->GetRootFolder()->Traverse( [ &OutMaterials ] ( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
        {
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

            if( DrawingLayer )
            {
                TArray<UMaterialInterface*> DrawingLayerUsedMaterials;

                DrawingLayer->GetUsedMaterials( DrawingLayerUsedMaterials );

                OutMaterials.Append( DrawingLayerUsedMaterials );
            }

            return UArianeLayerFolder::ETraversalReturnValue::Continue;
        } );
}

int32
UArianePainting3DComponent::GetNumMaterials() const
{
    return UsedMaterials.Num();
}

UMaterialInterface*
UArianePainting3DComponent::GetMaterial(int32 ElementIndex) const
{
    if ( UsedMaterials.IsValidIndex( ElementIndex ) )
    {
        return UsedMaterials[ElementIndex];
    }
    return nullptr;
}

void
UArianePainting3DComponent::SetMaterial( int32 ElementIndex, UMaterialInterface* Material )
{
    if (ElementIndex >= 0 )
    {
        if ( ElementIndex >= UsedMaterials.Num() )
        {
            UsedMaterials.SetNum( ElementIndex + 1 );
        }

        UsedMaterials[ElementIndex] = Material;

        // This call will destroy the Proxy and create a new one with the new data (via CreateSceneProxy)
        MarkRenderStateDirty();
    }
}

void
UArianePainting3DComponent::Init()
{
    //LayerStack->Init();

    Update( false );
}

void
UArianePainting3DComponent::PostLoad()
{
    Super::PostLoad();

    Init();
}

void
UArianePainting3DComponent::PostEditUndo()
{
    Super::PostEditUndo();

    Update( false );
}

UArianeLayerStack*
UArianePainting3DComponent::GetLayerStack()
{
    return LayerStack;
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
UArianePainting3DComponent::OnComponentDestroyed( bool bDestroyingHierarchy )
{
    Super::OnComponentDestroyed( bDestroyingHierarchy );

    LayerStack->OnComponentDestroyed( bDestroyingHierarchy );
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
    //FBoxSphereBounds RetBounds = Super::CalcBounds( FTransform::Identity );
    FBoxSphereBounds RetBounds = FBoxSphereBounds(ForceInit);

    RetBounds = /*RetBounds +*/ LayerStack->GetRootFolder()->GetBounds();

    return RetBounds.TransformBy( LocalToWorld );
}

UArianePainting3DComponent::FOnUpdateDelegate&
UArianePainting3DComponent::OnPreUpdateDelegate()
{
    return OnPreUpdate;
}

UArianePainting3DComponent::FOnUpdateDelegate&
UArianePainting3DComponent::OnPostUpdateDelegate()
{
    return OnPostUpdate;
}

void
UArianePainting3DComponent::Update( bool bInteractive )
{
    OnPreUpdate.Broadcast( bInteractive );

    LayerStack->GetRootFolder()->Update( bInteractive );

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();

    OnPostUpdate.Broadcast( bInteractive );

//UpdateBounds();
//MarkRenderTransformDirty();
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

const TArray<UOdysseyPaletteSet*>&
UArianePainting3DComponent::GetPaletteSets() const
{
    return PaletteSets;
}

UOdysseyPaletteEntryColor*
UArianePainting3DComponent::GetCurrentPaletteColorEntry() const
{
    return CurrentPaletteColorEntry;
}

void
UArianePainting3DComponent::SetCurrentPaletteSet( const FGuid& InCurrentPaletteSet )
{
    CurrentPaletteSet = InCurrentPaletteSet;
}

FGuid
UArianePainting3DComponent::GetCurrentPaletteSet() const
{
    return CurrentPaletteSet;
}

void
UArianePainting3DComponent::AddPaletteSet( UOdysseyPalette* iPalette )
{
}

void
UArianePainting3DComponent::RemovePaletteSet( UOdysseyPaletteSet* PaletteSet )
{
}

void
UArianePainting3DComponent::SetPaletteSet( FGuid Index, UOdysseyPaletteSet* PaletteSet )
{
}

void
UArianePainting3DComponent::SetCurrentPaletteColorEntry( UOdysseyPaletteEntryColor* Entry, FGuid Set )
{
}

FColor
UArianePainting3DComponent::GetHUDForegroundColor()
{
    return EditorInterface ? EditorInterface->GetHUDForegroundColor() : FColor::Black;
}

//--------------------------------------------------------------------------------------------------

FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

// Note: The proxy is created via CreateSceneProxy, and will be recreated everytime MarkRenderStateDirty() is called
FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianePainting3DComponent* iPainting3DComponent )
    : FPrimitiveSceneProxy ( iPainting3DComponent )
    , Painting3DComponent ( iPainting3DComponent )
{
    EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform_Checked( InFeatureLevel );
    TArray<UMaterialInterface*> MaterialInterfaces;

    Painting3DComponent->GetUsedMaterials( MaterialInterfaces );

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
    //UMaterialInterface* MaterialInterface = GEngine->VertexColorMaterial;

    DrawingLayer->InstancedObjectsAccessRW.Lock();

    DrawingLayer->GetRootGroup()->Traverse( [ this
                                            , DrawingLayer
                                            //, MaterialInterface
                                            , ViewIndex
                                            , &Collector ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
    {
        if( ( Object->GetClass() == FArianePath::StaticClass() )  )
        {
            FArianePath* Path = static_cast<FArianePath*>(Object);
            FArianePathGeometry3D& Mesh = Path->GetGeometry3D();
            UMaterialInterface* MaterialInterface = Path->GetMaterial();
            FPrimitiveDrawInterface* PDI = Collector.GetPDI(ViewIndex);

            if( MaterialInterface
             && MaterialInterface->GetRenderProxy()
             && Path->IsVisible( true )
             && Path->GetSegments().Num()
             && Mesh.GetIndexBuffer().GetNumIndices()
             && Mesh.GetIndexBuffer().IsInitialized() )
            {
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
                                                 , DrawingLayer->GetComponentToWorld().ToMatrixWithScale() //GetLocalToWorld()
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
                BatchElement.MaxVertexIndex = Mesh.GetVertexCount() - 1;

                MeshBatch.ReverseCulling = IsLocalToWorldDeterminantNegative();
                MeshBatch.Type = PT_TriangleList;
                MeshBatch.DepthPriorityGroup = SDPG_World;
                MeshBatch.bCanApplyViewModeOverrides = false;
                MeshBatch.bDisableBackfaceCulling = true; // draw both sides
                MeshBatch.CastShadow = false;

                //Add the batch to the collector
                Collector.AddMesh( ViewIndex, MeshBatch );

                // for debugging purpose (flag "r.Ariane.ShowNormals")
                if ( CVarShowArianeNormals.GetValueOnRenderThread() )
                {
                    const FTransform& DrawingLayerTransform = DrawingLayer->GetComponentTransform();

                    for ( FArianeVertexID& VertexID : Path->GetVertices() )
                    {
                        FArianeVertex* Vertex = VertexID.GetVertex();
                        FVector VertexWorldPosition = DrawingLayerTransform.TransformPosition( Vertex->GetPosition() );
                        FVector VertexWorldNormal = DrawingLayerTransform.TransformVector( Vertex->GetNormal() );

                        PDI->DrawLine( VertexWorldPosition
                                     , VertexWorldPosition + ( VertexWorldNormal * 200.0f )
                                     , FLinearColor::Green
                                     , SDPG_World
                                     , 1.0f );
                    }
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
    UArianeLayerStack* LayerStack = Painting3DComponent->GetLayerStack();

    for( int32 ViewIndex = 0; ViewIndex < Views.Num(); ViewIndex++ )
    {
        const FSceneView* View = Views[ViewIndex];


        LayerStack->GetRootFolder()->Traverse( [ this
                                                    , &Collector
                                                    , ViewIndex ]( UArianeLayer* Layer ) -> UArianeLayerFolder::ETraversalReturnValue
        {
            UArianeLayerDrawing* DrawingLayer = Cast<UArianeLayerDrawing>(Layer);

            if( DrawingLayer && DrawingLayer->IsVisible() )
            {
                GetDrawingLayerDynamicMeshElements( DrawingLayer
                                                  , Collector
                                                  , ViewIndex );
            }

            return UArianeLayerFolder::ETraversalReturnValue::Continue;
        } );

        // Render bounds manually because it's a bit complicated to render them when using custom proxies like this one.
        if ( ViewFamily.EngineShowFlags.Bounds )
        {
            RenderBounds(
                Collector.GetPDI(ViewIndex),
                ViewFamily.EngineShowFlags,
                GetBounds(),
                true
            );
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
