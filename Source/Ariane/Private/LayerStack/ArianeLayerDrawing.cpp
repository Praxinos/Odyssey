// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
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

UArianeLayerDrawing::~UArianeLayerDrawing()
{
}

void
UArianeLayerDrawing::BeginDestroy()
{
    UnbindDelegates();

    Super::BeginDestroy();
}

UArianeLayerDrawing::UArianeLayerDrawing()
    : DrawingOrigin ( EArianeLayerDrawingOrigin::Layer )
    , DrawingOrientation ( EArianeLayerDrawingOrientation::View )
{
    InvalidationFlags = new FArianeLayerInvalidationFlags();

    ResetHierarchy();

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
UArianeLayerDrawing::OnAssetLoaded(UObject* LoadedObject)
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
UArianeLayerDrawing::InvalidateCache()
{
    // RootObjectID won't have its cache reset after Undoing, we have to force it.
    RootGroupID.InvalidateCache();
}

FArianeGroup*
UArianeLayerDrawing::GetRootGroup()
{
    return static_cast<FArianeGroup*>(RootGroupID.GetObject());
}

void
UArianeLayerDrawing::PrintPointers()
{
    for( FInstancedStruct& InstancedObject : InstancedObjects )
    {
        FArianeObject* Object = InstancedObject.GetMutablePtr<FArianeObject>();

        UE_LOG( LogTemp, Warning, TEXT("PTR:%llu"), (uint64) Object );
    }
}

void
UArianeLayerDrawing::PostLoad()
{
    UArianeLayerStack* LayerStack = GetLayerStack();

    Super::PostLoad();

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
        GetLayerStack()->GetPainting3DComponent()->UpdateComponentToWorld();
    }
}

void
UArianeLayerDrawing::PostEditUndo()
{
    UArianeLayerStack* LayerStack = GetLayerStack();

    Super::PostEditUndo();

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
        GetLayerStack()->GetPainting3DComponent()->UpdateComponentToWorld();
    }
}

FArianeObject*
UArianeLayerDrawing::GetObject( const FGuid& InGuid )
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
UArianeLayerDrawing::AllocObject( const FName& InName, EArianeAllocationModel AllocationModel )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this, InName, EArianeAllocationModel::InstancedStruct ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();

    return NewObject;
}

FArianeGroup*
UArianeLayerDrawing::AllocGroup( const FName& InName
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
UArianeLayerDrawing::AllocCycle( UMaterialInterface* InMaterialInterface
                               , const FName& InName
                               , EArianeAllocationModel AllocationModel )
{
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
                                               : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return NewCycle;
}

FArianeEllipse*
UArianeLayerDrawing::AllocEllipse( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , double RadiusX
                                 , double RadiusY
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
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
                                              : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Ellipse;
}

FArianeLine*
UArianeLayerDrawing::AllocLine( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , const FVector& StartPoint
                              , const FVector& EndPoint
                              , double StrokeWidth
                              , EArianeAllocationModel AllocationModel )
{
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
                                           : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Line;
}

FArianePolygon*
UArianeLayerDrawing::AllocPolygon( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , uint32 CornerCount
                                 , double Radius
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
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
                                              : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Polygon;
}

FArianeRectangle*
UArianeLayerDrawing::AllocRectangle( UMaterialInterface* InMaterialInterface
                                   , const FName& InName
                                   , double Width
                                   , double Height
                                   , double StrokeWidth
                                   , EArianeAllocationModel AllocationModel )
{
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
                                                : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return Rectangle;
}

FArianePath*
UArianeLayerDrawing::AllocPath( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , EArianeAllocationModel AllocationModel )
{
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
                                              : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    MarkRenderStateDirty();


    return NewPath;
}

void
UArianeLayerDrawing::AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials ) const
{
    OutUsedMaterials.Reserve( OutUsedMaterials.Num() + UsedMaterials.Num() );

    for( auto Pair : UsedMaterials )
    {
        OutUsedMaterials.Add( Pair.Key );
    }
}

void
UArianeLayerDrawing::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials ) const
{
    OutUsedMaterials.Empty();

    AppendUsedMaterials( OutUsedMaterials );
}

void
UArianeLayerDrawing::IncrementMaterial( UMaterialInterface* MaterialInterface )
{
    UArianeLayerStack* LayerStack = GetLayerStack();
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // not: do not use findOrAdd, it will not initialize value to zero.
    if( value == nullptr )
    {
        UsedMaterials.Add( MaterialInterface, 1 );
    }

    // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
    if( LayerStack )
    {
    // we need to rebuild the proxy
        LayerStack->GetPainting3DComponent()->MarkRenderStateDirty();
    }
}

void
UArianeLayerDrawing::DecrementMaterial( UMaterialInterface* MaterialInterface )
{
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // Note: We don't check the ptr because what is decremented must have been incremented first.
    // Or else there is a bug somewhere else.
    *value--;

    if( *value == 0 )
    {
        UsedMaterials.Remove( MaterialInterface );
    }

    // we need to rebuild the proxy
    GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();
}

const
TArray<FInstancedStruct>&
UArianeLayerDrawing::GetInstancedObjects() const
{
    return InstancedObjects;
}

TArray<FInstancedStruct>&
UArianeLayerDrawing::GetInstancedObjects()
{
    return InstancedObjects;
}

void
UArianeLayerDrawing::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    RootGroupID.GetObject()->Update( ObjectUpdateFlags, true );


    Super::Update( bInteractive );
}

void
UArianeLayerDrawing::OnRootObjectInvalidated()
{
    Invalidate( FArianeLayerInvalidationFlags() );
}

void
UArianeLayerDrawing::BindDelegates()
{
    if( RootGroupID.GetObject() )
    {
        RootGroupID.GetObject()->GetOnPostInvalidatedDelegate().AddUObject( this, &UArianeLayerDrawing::OnRootObjectInvalidated );
    }
}

void
UArianeLayerDrawing::UnbindDelegates()
{
    if( RootGroupID.GetObject() )
    {
        RootGroupID.GetObject()->GetOnPostInvalidatedDelegate().RemoveAll( this );
    }
}

void
UArianeLayerDrawing::ResetHierarchy()
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
UArianeLayerDrawing::DeleteInstancedObject( FArianeObject* Object )
{
    InstancedObjectsAccessRW.Lock();

    InstancedObjects.RemoveAll( [Object]( FInstancedStruct& Struct ) -> bool
    {
        return ( Object == Struct.GetPtr<FArianeObject>() ) ? true : false;
    } );

    InstancedObjectsAccessRW.Unlock();
}

FBoxSphereBounds
UArianeLayerDrawing::CalcBounds( const FTransform& LocalToWorld ) const
{
    return FBoxSphereBounds( RootGroupID.GetObject()->GetBoundingBox().TransformBy( LocalToWorld ) );
}

void
UArianeLayerDrawing::OnUpdateTransform( EUpdateTransformFlags UpdateTransformFlags, ETeleportType TeleportType )
{
    GetRootGroup()->UpdateTransform();
}

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

void
UArianeLayerDrawing::ClearObjectSelection()
{
    SelectedObjects.RemoveAll(
        []( FArianeObject* SelectedObject )
        {
            SelectedObject->SetSelected( false );

            return true;
        } );
}

void
UArianeLayerDrawing::UnselectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Remove( ObjectToSelect );

    ObjectToSelect->SetSelected( false );
}

void
UArianeLayerDrawing::SelectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Add( ObjectToSelect );

    ObjectToSelect->SetSelected( true );
}

const TArray<FArianeObject*>&
UArianeLayerDrawing::GetSelectedObjects() const
{
    return SelectedObjects;
}

TArray<FArianeObject*>&
UArianeLayerDrawing::GetSelectedObjects()
{
    return SelectedObjects;
}

void
UArianeLayerDrawing::AppendSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
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
UArianeLayerDrawing::GetSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
{
    SelectedTrees.Empty();

    AppendSelectedTrees( SelectedTrees );
}
