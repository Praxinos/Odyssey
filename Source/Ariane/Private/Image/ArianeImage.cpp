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
#include "ArianeImageKeyData.h"

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
UArianeImage::SetDrawingLayer( TWeakObjectPtr<UArianeLayerDrawing> InDrawingLayer )
{
    DrawingLayer = InDrawingLayer;
}

TWeakObjectPtr<UArianeLayerDrawing>
UArianeImage::GetDrawingLayer()
{
    return DrawingLayer;
}

// called when the world starts to exist (the drawing layer is registered). This is required by InitVertexFactory().
void
UArianeImage::OnRegisterLayer()
{
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
    UArianeLayerDrawing* DrawingLayerPtr = GetDrawingLayer().Get();

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

    // Not all Images have drawing layers, especially those stored in a Level Sequence
    if( DrawingLayerPtr )
    {
        UArianeLayerStack* LayerStack = DrawingLayerPtr->GetLayerStack();

        // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
        if( LayerStack )
        {
            // recompute the bounding volumes or else nothing will draw
            LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
        }
    }
}

void
UArianeImage::PostEditUndo()
{
    UArianeLayerDrawing* DrawingLayerPtr = GetDrawingLayer().Get();

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

    // Not all Images have drawing layers, especially those stored in a Level Sequence
    if( DrawingLayerPtr )
    {
        UArianeLayerStack* LayerStack = DrawingLayerPtr->GetLayerStack();

        // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
        if( LayerStack )
        {
            // recompute the bounding volumes or else nothing will draw
            LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
        }
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

void
UArianeImage::Animate( const FArianeImageKeyData* KeyData, const FArianeImageKeyData* NextKeyData, float T )
{
    FArianeObject::Traverse( GetRootGroup()
                           , [ KeyData
                             , NextKeyData
                             , T ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            const FArianeKeyedObject* KeyedObject = const_cast<FArianeImageKeyData*>(KeyData)->GetKeyedObject( Object->GetGuid() );
            const FArianeKeyedObject* NextKeyedObject = NextKeyData ? const_cast<FArianeImageKeyData*>(NextKeyData)->GetKeyedObject( Object->GetGuid() )
                                                                    : KeyedObject;

            if( KeyedObject )
            {
                Object->Animate( KeyedObject, NextKeyedObject, T );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
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
    DrawingLayer->MarkRenderStateDirty();


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
    DrawingLayer->MarkRenderStateDirty();


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
    DrawingLayer->MarkRenderStateDirty();


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
    DrawingLayer->MarkRenderStateDirty();


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
    DrawingLayer->MarkRenderStateDirty();


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
    DrawingLayer->MarkRenderStateDirty();


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
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // not: do not use findOrAdd, it will not initialize value to zero.
    if( value == nullptr )
    {
        UsedMaterials.Add( MaterialInterface, 1 );
    }

    // DrawingLayer can be null for orphan Images (stored in animation keys)
    if( DrawingLayer.IsValid() )
    {
        // we need to rebuild the proxy
        DrawingLayer->MarkRenderStateDirty();
    }
}

void
UArianeImage::DecrementMaterial( UMaterialInterface* MaterialInterface )
{
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // Note: We don't check the ptr because what is decremented must have been incremented first.
    // Or else there is a bug somewhere else.
    *value--;

    if( *value == 0 )
    {
        UsedMaterials.Remove( MaterialInterface );
    }

    // DrawingLayer can be null for orphan Images (stored in animation keys)
    if( DrawingLayer.IsValid() )
    {
        // we need to rebuild the proxy
        DrawingLayer->MarkRenderStateDirty();
    }
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
    // DrawingLayer can be null for orphan images (stored in animation keys)
    if( DrawingLayer.IsValid() )
    {
        DrawingLayer->Invalidate( FArianeLayerInvalidationFlags() );
    }
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
