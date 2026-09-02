// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeImage.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCycle.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"
#include "ArianeVertex.h"

// for debugging purpose
static TAutoConsoleVariable<bool> CVarShowArianeNormals( TEXT("r.Ariane.ShowNormals")
                                                       , 0
                                                       , TEXT("Show Ariane mesh normals") );


UArianeImage::~UArianeImage()
{
}

void
UArianeImage::BeginDestroy()
{
    UnbindDelegates();

    Super::BeginDestroy();
}

UArianeImage::UArianeImage()
{
    ResetHierarchy();

    bWantsOnUpdateTransform = true;


    FCoreUObjectDelegates::OnAssetLoaded.AddUObject( this, &UArianeImage::OnAssetLoaded );

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

UArianeLayerDrawing*
UArianeImage::GetDrawingLayer()
{
    return Cast<UArianeLayerDrawing>(GetOuter());
}

void
UArianeImage::OnAssetLoaded(UObject* LoadedObject)
{
    // Note: . Our FArianeObjects Transforms depend on the
    // Layer (to compute the world Transform). but, when PostLoad is called, the Transforms are not set yet
    // so we update the Transforms for our ArianeObjects after the Asset is loaded
    if ( GetOutermost() )
    {
        GetRootGroup()->UpdateTransform();
    }
}

void
UArianeImage::InvalidateCache()
{
    // RootObjectID won't have its cache reset after Undoing, we have to force it.
    RootGroupID.InvalidateCache();
}

FArianeGroup*
UArianeImage::GetRootGroup()
{
    return static_cast<FArianeGroup*>(RootGroupID.GetObject());
}

void
UArianeImage::PrintPointers()
{
    for( FInstancedStruct& InstancedObject : InstancedObjects )
    {
        FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

        UE_LOG( LogTemp, Warning, TEXT("PTR:%llu"), (uint64) Object );
    }
}

void
UArianeImage::PostLoad()
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();

    Super::PostLoad();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }

    // RootObjectID won't have its cache reset after Undoing, we have to force it.
    InvalidateCache();

    BindDelegates();

    // Empty the map before reloading. Each path will populate or increment the attached value.
    UsedMaterials.Empty();
    // Empty the selection
    SelectedObjects.Empty();

    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostLoad();
    }

    GetRootGroup()->UpdateTransform();

    Update( false );

    // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
    if( LayerStack )
    {
        // recompute the bounding volumes or else nothing will draw
        LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
    }
}

void
UArianeImage::PostEditUndo()
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();

    Super::PostEditUndo();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }

    // RootObjectID won't have its cache reset after Undoing, we have to force it.
    InvalidateCache();

    BindDelegates();

    // Empty the map before reloading. Each path will populate or increment the attached value.
    UsedMaterials.Empty();
    // Empty the selection
    SelectedObjects.Empty();

    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostEditUndo();
    }

    GetRootGroup()->UpdateTransform();

    Update( false );

    // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
    if( LayerStack )
    {
        // recompute the bounding volumes or else nothing will draw
        LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
    }
}

FArianeObject*
UArianeImage::GetObject( const FGuid& InGuid )
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

FArianeObject*
UArianeImage::AllocObject( const FName& InName, EArianeAllocationModel AllocationModel )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this, InName, EArianeAllocationModel::InstancedStruct ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();

    return NewObject;
}

FArianeGroup*
UArianeImage::AllocGroup( const FName& InName
                               , EArianeAllocationModel AllocationModel )
{
    FArianeGroup* NewGroup = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianeGroup>( this, InName, AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        NewGroup = InstancedObjects.Last().GetMutablePtr<FArianeGroup>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewGroup = new FArianeGroup( this, InName, AllocationModel );
    }

    return NewGroup;
}

FArianeCycle*
UArianeImage::AllocCycle( UMaterialInterface* InMaterialInterface
                               , const FName& InName
                               , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianeCycle* NewCycle = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianeCycle>( this, InName, AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        NewCycle = InstancedObjects.Last().GetMutablePtr<FArianeCycle>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewCycle = new FArianeCycle( this, InName, AllocationModel );
    }

    NewCycle->SetMaterial( InMaterialInterface ? InMaterialInterface
                                               : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return NewCycle;
}

FArianeEllipse*
UArianeImage::AllocEllipse( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , double RadiusX
                                 , double RadiusY
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianeEllipse* Ellipse = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianeEllipse>( this
                                                                    , InName
                                                                    , RadiusX
                                                                    , RadiusY
                                                                    , StrokeWidth
                                                                    , AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        Ellipse = InstancedObjects.Last().GetMutablePtr<FArianeEllipse>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        Ellipse = new FArianeEllipse( this
                                   , InName
                                   , RadiusX
                                   , RadiusY
                                   , StrokeWidth
                                   , AllocationModel );
    }

    Ellipse->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Ellipse;
}

FArianeLine*
UArianeImage::AllocLine( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , const FVector& StartPoint
                              , const FVector& EndPoint
                              , double StrokeWidth
                              , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianeLine* Line = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianeLine>( this
                                                                 , InName
                                                                 , StartPoint
                                                                 , EndPoint
                                                                 , StrokeWidth
                                                                 , AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        Line = InstancedObjects.Last().GetMutablePtr<FArianeLine>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        Line = new FArianeLine( this
                              , InName
                              , StartPoint
                              , EndPoint
                              , StrokeWidth
                              , AllocationModel );
    }

    Line->SetMaterial( InMaterialInterface ? InMaterialInterface
                                           : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Line;
}

FArianePolygon*
UArianeImage::AllocPolygon( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , uint32 CornerCount
                                 , double Radius
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianePolygon* Polygon = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianePolygon>( this
                                                                    , InName
                                                                    , CornerCount
                                                                    , Radius
                                                                    , StrokeWidth
                                                                    , AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        Polygon = InstancedObjects.Last().GetMutablePtr<FArianePolygon>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        Polygon = new FArianePolygon( this
                                    , InName
                                    , CornerCount
                                    , Radius
                                    , StrokeWidth
                                    , AllocationModel );
    }

    Polygon->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Polygon;
}

FArianeRectangle*
UArianeImage::AllocRectangle( UMaterialInterface* InMaterialInterface
                                   , const FName& InName
                                   , double Width
                                   , double Height
                                   , double StrokeWidth
                                   , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianeRectangle* Rectangle = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianeRectangle>( this
                                                                      , InName
                                                                      , Width
                                                                      , Height
                                                                      , StrokeWidth
                                                                      , AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        Rectangle = InstancedObjects.Last().GetMutablePtr<FArianeRectangle>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        Rectangle = new FArianeRectangle( this
                                        , InName
                                        , Width
                                        , Height
                                        , StrokeWidth
                                        , AllocationModel );
    }

    Rectangle->SetMaterial( InMaterialInterface ? InMaterialInterface
                                                : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Rectangle;
}

FArianePath*
UArianeImage::AllocPath( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    FArianePath* NewPath = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjectsAccessRW.Lock();
        InstancedObjects.Add( FInstancedStruct::Make<FArianePath>( this, InName, AllocationModel ) );
        InstancedObjectsAccessRW.Unlock();

        NewPath = InstancedObjects.Last().GetMutablePtr<FArianePath>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewPath = new FArianePath( this, InName, AllocationModel );
    }

    NewPath->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return NewPath;
}

void
UArianeImage::AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials ) const
{
    OutUsedMaterials.Reserve( OutUsedMaterials.Num() + UsedMaterials.Num() );

    for( auto Pair : UsedMaterials )
    {
        OutUsedMaterials.Add( Pair.Key );
    }
}

void
UArianeImage::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials ) const
{
    OutUsedMaterials.Empty();

    AppendUsedMaterials( OutUsedMaterials );
}

void
UArianeImage::IncrementMaterial( UMaterialInterface* MaterialInterface )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // not: do not use findOrAdd, it will not initialize value to zero.
    if( value == nullptr )
    {
        UsedMaterials.Add( MaterialInterface, 1 );
    }

    // we need to rebuild the proxy
    MarkRenderStateDirty();
}

void
UArianeImage::DecrementMaterial( UMaterialInterface* MaterialInterface )
{
    UArianeLayerStack* LayerStack = GetDrawingLayer()->GetLayerStack();
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // Note: We don't check the ptr because what is decremented must have been incremented first.
    // Or else there is a bug somewhere else.
    *value--;

    if( *value == 0 )
    {
        UsedMaterials.Remove( MaterialInterface );
    }

    // we need to rebuild the proxy
    MarkRenderStateDirty();
}

const
TArray<FInstancedStruct>&
UArianeImage::GetInstancedObjects() const
{
    return InstancedObjects;
}

TArray<FInstancedStruct>&
UArianeImage::GetInstancedObjects()
{
    return InstancedObjects;
}

void
UArianeImage::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    RootGroupID.GetObject()->Update( ObjectUpdateFlags, true );
}

void
UArianeImage::OnRootObjectInvalidated()
{
    GetDrawingLayer()->Invalidate( FArianeLayerInvalidationFlags() );
}

void
UArianeImage::BindDelegates()
{
    if( RootGroupID.GetObject() )
    {
        RootGroupID.GetObject()->GetOnPostInvalidatedDelegate().AddUObject( this, &UArianeImage::OnRootObjectInvalidated );
    }
}

void
UArianeImage::UnbindDelegates()
{
    if( RootGroupID.GetObject() )
    {
        RootGroupID.GetObject()->GetOnPostInvalidatedDelegate().RemoveAll( this );
    }
}

void
UArianeImage::ResetHierarchy()
{
    UnbindDelegates( );

    InstancedObjects.Empty();
    UsedMaterials.Empty();
    SelectedObjects.Empty();

    RootGroupID = FArianeObjectID( AllocGroup( "Root Group", EArianeAllocationModel::InstancedStruct ) );

    RootGroupID.GetObject()->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );

    RootGroupID.GetObject()->UpdateTransform();

    BindDelegates();
}

void
UArianeImage::DeleteInstancedObject( FArianeObject* Object )
{
    InstancedObjectsAccessRW.Lock();

    InstancedObjects.RemoveAll( [Object]( FInstancedStruct& Struct ) -> bool
    {
        return ( Object == Struct.GetPtr<FArianeObject>() ) ? true : false;
    } );

    InstancedObjectsAccessRW.Unlock();
}

FBoxSphereBounds
UArianeImage::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( RootGroupID.GetObject()->GetBoundingBox().TransformBy( LocalToWorld ) );
}

void
UArianeImage::OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType )
{
    GetRootGroup()->UpdateTransform();
}

void
UArianeImage::ClearObjectSelection()
{
    SelectedObjects.RemoveAll(
        []( FArianeObject* SelectedObject )
        {
            SelectedObject->SetSelected( false );

            return true;
        } );
}

void
UArianeImage::UnselectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Remove( ObjectToSelect );

    ObjectToSelect->SetSelected( false );
}

void
UArianeImage::SelectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Add( ObjectToSelect );

    ObjectToSelect->SetSelected( true );
}

const TArray<FArianeObject*>&
UArianeImage::GetSelectedObjects() const
{
    return SelectedObjects;
}

TArray<FArianeObject*>&
UArianeImage::GetSelectedObjects()
{
    return SelectedObjects;
}

void
UArianeImage::AppendSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
{
    SelectedTrees.Reserve( SelectedTrees.Num() + SelectedObjects.Num() );

    for( FArianeObject* SelectedObject : SelectedObjects )
    {
        bool bHasSelectedAncestor = false;

        // Group only objects that have no selected ancestors
        FArianeObject::TraverseBackwards ( SelectedObject
                                        , [ SelectedObject
                                          , &bHasSelectedAncestor
                                          , &SelectedTrees ]( FArianeObject* TraversedObject ) -> FArianeObject::ETraversalReturnValue
        {
            if( TraversedObject != SelectedObject )
            {
                if( TraversedObject->IsSelected() )
                {
                    bHasSelectedAncestor = true;

                    return FArianeObject::ETraversalReturnValue::Stop;
                }
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );

        if( bHasSelectedAncestor == false )
        {
            SelectedTrees.Add( SelectedObject );
        }
    }
}

void
UArianeImage::GetSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
{
    SelectedTrees.Empty();

    AppendSelectedTrees( SelectedTrees );
}

FPrimitiveSceneProxy*
UArianeImage::CreateSceneProxy()
{
    return new FArianeGeometryProxy( GetScene()->GetFeatureLevel(), this );
}

//--------------------------------------------------------------------------------------------------


FArianeGeometryProxy::~FArianeGeometryProxy()
{
}

// Note: The proxy is created via CreateSceneProxy, and will be recreated everytime MarkRenderStateDirty() is called
FArianeGeometryProxy::FArianeGeometryProxy( ERHIFeatureLevel::Type InFeatureLevel
                                          , UArianeImage* InImage )
    : FPrimitiveSceneProxy ( InImage )
    , Image ( InImage )
{
    EShaderPlatform ShaderPlatform = GetFeatureLevelShaderPlatform_Checked( InFeatureLevel );
    TArray<UMaterialInterface*> MaterialInterfaces;

    Image->GetUsedMaterials( MaterialInterfaces );

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
    UArianeLayerDrawing* DrawingLayer = Image->GetDrawingLayer();

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
