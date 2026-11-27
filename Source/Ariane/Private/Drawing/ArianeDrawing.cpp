// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeDrawing.h"
#include "ArianeLayerVector.h"
#include "ArianeLayerStack.h"
#include "ArianeMesh.h"
#include "ArianePainting3DActor.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"
#include "ArianeCycle.h"
#include "ArianeEllipse.h"
#include "ArianeRectangle.h"
#include "ArianeLine.h"
#include "ArianePolygon.h"
#include "ArianeVertex.h"
#include "ArianeDrawingKeyData.h"

#define LOCTEXT_NAMESPACE "Ariane"

UArianeDrawing::~UArianeDrawing()
{
}

void
UArianeDrawing::BeginDestroy()
{
    //UnbindDelegates();

    Super::BeginDestroy();
}

UArianeDrawing::UArianeDrawing(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
    , bRenderItemsNeedRebuild ( true )
    , bUpdateUsedMaterials ( true )
    , bResetOnRegister ( false )
{
    ResetHierarchy();
}

UArianeDrawing::UArianeDrawing()
    : bRenderItemsNeedRebuild ( true )
    , bUpdateUsedMaterials ( true )
    , bResetOnRegister ( false )
{
    ResetHierarchy();
}

FGuid
UArianeDrawing::GetAllocatorGuid()
{
    return AllocatorGuid;
}

void
UArianeDrawing::RebindObjects()
{
    AllocatorGuid = FGuid::NewGuid();
}

void UArianeDrawing::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);
}

void
UArianeDrawing::Reset()
{
    AllocatorGuid = FGuid::NewGuid();

    // Empty the map before reloading. Each path will populate or increment the attached value.
    UsedMaterials.Empty();
    // Empty the selection
    SelectedObjects.Empty();

    for (FInstancedStruct& InstancedStruct : InstancedObjects)
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->ResetDrawing( this );
    }

    for (FInstancedStruct& InstancedStruct : InstancedObjects)
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostReset();
    }

    // Every child is attached now, in allocation order
    for (FInstancedStruct& InstancedStruct : InstancedObjects)
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->SortChildren();
    }

    //if( bCallUpdate )
    {
        RootGroupID.GetObject( this )->UpdateTransform();
        RootGroupID.GetObject( this )->Update( FArianeObject::EUpdateFlags::None, true );
    }
}

void
UArianeDrawing::OnRegisterLayer()
{
    if( bResetOnRegister )
    {
        // VectorLayer is now the right one: the invalidations reach its folder, so the next Update rebuilds the bounds
        bResetOnRegister = false;
        Reset();
    }
    else
    {
        RootGroupID.GetObject( this )->UpdateTransform();
    }
}

void
UArianeDrawing::SetVectorLayer( TWeakObjectPtr<UArianeLayerVector> InVectorLayer )
{
    VectorLayer = InVectorLayer;
}

TWeakObjectPtr<UArianeLayerVector>
UArianeDrawing::GetVectorLayer()
{
    return VectorLayer;
}

FArianeGroup*
UArianeDrawing::GetRootGroup()
{
    return static_cast<FArianeGroup*>(RootGroupID.GetObject( this ));
}

void
UArianeDrawing::PrintPointers()
{
    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        UE_LOG( LogTemp, Warning, TEXT("UArianeDrawing::PrintPointers - Object.Drawing:%p"), Object->GetDrawing() );
    }
}

void
UArianeDrawing::PostInitProperties()
{
    Super::PostInitProperties();

    // Copied from a template (spawnable, blueprint): the objects still point to the template drawing and have no
    // children yet. Reset rebuilds both, but only once the layer is connected (OnRegisterLayer).
    // Note: GetArchetype() can't tell, the template given to SpawnActor is not the archetype.
    if( RootGroupID.GetObject( this )->GetDrawing() != this )
    {
        bResetOnRegister = true;
    }
}

#if WITH_EDITOR
void
UArianeDrawing::PostEditImport()
{
    UArianeLayerVector* VectorLayerPtr = GetVectorLayer().Get();

    Super::PostEditImport();

    Reset();

    //BindDelegates();

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

    // Not all Drawings have drawing layers, especially those stored in a Level Sequence
    if( VectorLayerPtr )
    {
        UArianeLayerStack* LayerStack = VectorLayerPtr->GetLayerStack();

        // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
        if( LayerStack )
        {
            // recompute the bounding volumes or else nothing will draw
            LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
        }
    }
}
#endif

void
UArianeDrawing::PostLoad()
{
    UArianeLayerVector* VectorLayerPtr = GetVectorLayer().Get();

    Super::PostLoad();

    Reset();

    //BindDelegates();

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

    // Not all Drawings have drawing layers, especially those stored in a Level Sequence
    if( VectorLayerPtr )
    {
        UArianeLayerStack* LayerStack = VectorLayerPtr->GetLayerStack();

        // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
        if( LayerStack )
        {
            // recompute the bounding volumes or else nothing will draw
            LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
        }
    }
}

#if WITH_EDITOR
void
UArianeDrawing::PostEditUndo()
{
    UArianeLayerVector* VectorLayerPtr = GetVectorLayer().Get();

    Super::PostEditUndo();

    Reset();

    //BindDelegates();

    for( FInstancedStruct& InstancedStruct : InstancedObjects )
    {
        FArianeObject* Object = InstancedStruct.GetMutablePtr<FArianeObject>();

        Object->PostEditUndo();
    }

    GetRootGroup()->UpdateTransform();

    Update( false );

    // Not all Drawings have drawing layers, especially those stored in a Level Sequence
    if( VectorLayerPtr )
    {
        UArianeLayerStack* LayerStack = VectorLayerPtr->GetLayerStack();

        // layer stack can be null in case the layer is copied in the clipboard (orphan layer)
        if( LayerStack )
        {
            // recompute the bounding volumes or else nothing will draw
            LayerStack->GetPainting3DComponent()->UpdateComponentToWorld();
        }
    }
}
#endif

FArianeObject*
UArianeDrawing::GetObject( const FGuid& InGuid )
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
UArianeDrawing::Animate( const FArianeDrawingKeyData* KeyData, const FArianeDrawingKeyData* NextKeyData, float T )
{
    FArianeObject::Traverse( GetRootGroup()
                           , FArianeObject::ETraversalPolicy::AllChildren
                           , [ KeyData
                             , NextKeyData
                             , T ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            const FArianeKeyedObject* KeyedObject = const_cast<FArianeDrawingKeyData*>(KeyData)->GetKeyedObject( Object->GetGuid() );
            const FArianeKeyedObject* NextKeyedObject = NextKeyData ? const_cast<FArianeDrawingKeyData*>(NextKeyData)->GetKeyedObject( Object->GetGuid() )
                                                                    : KeyedObject;

            if( KeyedObject )
            {
                Object->Animate( KeyedObject, NextKeyedObject, T );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

FArianeObject*
UArianeDrawing::AllocObject( const FName& InName, EArianeAllocationModel AllocationModel )
{
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this, InName, EArianeAllocationModel::InstancedStruct ) );

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();

    NewObject->PostLoad();

    return NewObject;
}

FArianeGroup*
UArianeDrawing::AllocGroup( const FName& InName
                               , EArianeAllocationModel AllocationModel )
{
    FArianeGroup* NewGroup = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianeGroup>( this, InName, AllocationModel ) );

        NewGroup = InstancedObjects.Last().GetMutablePtr<FArianeGroup>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewGroup = new FArianeGroup( this, InName, AllocationModel );
    }

    NewGroup->PostLoad();


    return NewGroup;
}

FArianeCycle*
UArianeDrawing::AllocCycle( UMaterialInterface* InMaterialInterface
                        , const FName& InName
                        , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianeCycle* NewCycle = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianeCycle>( this, InName, AllocationModel ) );

        NewCycle = InstancedObjects.Last().GetMutablePtr<FArianeCycle>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewCycle = new FArianeCycle( this, InName, AllocationModel );
    }

    NewCycle->PostLoad();
    NewCycle->SetMaterial( InMaterialInterface ? InMaterialInterface
                                               : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return NewCycle;
}

FArianeEllipse*
UArianeDrawing::AllocEllipse( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , double RadiusX
                                 , double RadiusY
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianeEllipse* Ellipse = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianeEllipse>( this
                                                                    , InName
                                                                    , RadiusX
                                                                    , RadiusY
                                                                    , StrokeWidth
                                                                    , AllocationModel ) );

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

    Ellipse->PostLoad();
    Ellipse->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return Ellipse;
}

FArianeLine*
UArianeDrawing::AllocLine( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , const FVector& StartPoint
                              , const FVector& EndPoint
                              , double StrokeWidth
                              , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianeLine* Line = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianeLine>( this
                                                                 , InName
                                                                 , StartPoint
                                                                 , EndPoint
                                                                 , StrokeWidth
                                                                 , AllocationModel ) );

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

    Line->PostLoad();
    Line->SetMaterial( InMaterialInterface ? InMaterialInterface
                                           : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return Line;
}

FArianePolygon*
UArianeDrawing::AllocPolygon( UMaterialInterface* InMaterialInterface
                                 , const FName& InName
                                 , uint32 CornerCount
                                 , double Radius
                                 , double StrokeWidth
                                 , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianePolygon* Polygon = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianePolygon>( this
                                                                    , InName
                                                                    , CornerCount
                                                                    , Radius
                                                                    , StrokeWidth
                                                                    , AllocationModel ) );

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

    Polygon->PostLoad();
    Polygon->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return Polygon;
}

FArianeRectangle*
UArianeDrawing::AllocRectangle( UMaterialInterface* InMaterialInterface
                                   , const FName& InName
                                   , double Width
                                   , double Height
                                   , double StrokeWidth
                                   , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianeRectangle* Rectangle = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianeRectangle>( this
                                                                      , InName
                                                                      , Width
                                                                      , Height
                                                                      , StrokeWidth
                                                                      , AllocationModel ) );

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

    Rectangle->PostLoad();
    Rectangle->SetMaterial( InMaterialInterface ? InMaterialInterface
                                                : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return Rectangle;
}

FArianePath*
UArianeDrawing::AllocPath( UMaterialInterface* InMaterialInterface
                              , const FName& InName
                              , EArianeAllocationModel AllocationModel )
{
    UArianeLayerStack* LayerStack = GetVectorLayer()->GetLayerStack();
    FArianePath* NewPath = nullptr;

    if( AllocationModel == EArianeAllocationModel::InstancedStruct )
    {
        InstancedObjects.Add( FInstancedStruct::Make<FArianePath>( this, InName, AllocationModel ) );

        NewPath = InstancedObjects.Last().GetMutablePtr<FArianePath>();
    }

    if( AllocationModel == EArianeAllocationModel::OperatingSystem )
    {
        NewPath = new FArianePath( this, InName, AllocationModel );
    }

    NewPath->PostLoad();
    NewPath->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : LayerStack->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    VectorLayer->MarkRenderStateDirty();


    return NewPath;
}

void
UArianeDrawing::AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials ) const
{
    OutUsedMaterials.Reserve( OutUsedMaterials.Num() + UsedMaterials.Num() );

    for( UMaterialInterface* UsedMaterial : UsedMaterials )
    {
        OutUsedMaterials.Add( UsedMaterial );
    }
}

void
UArianeDrawing::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials, bool bGetDebugMaterials ) const
{
    OutUsedMaterials.Empty();

    AppendUsedMaterials( OutUsedMaterials );
}

const
TArray<FInstancedStruct>&
UArianeDrawing::GetInstancedObjects() const
{
    return InstancedObjects;
}

TArray<FInstancedStruct>&
UArianeDrawing::GetInstancedObjects()
{
    return InstancedObjects;
}

void
UArianeDrawing::Update( bool bInteractive )
{
    FArianeObject::EUpdateFlags ObjectUpdateFlags = FArianeObject::EUpdateFlags::None;

    if( bInteractive )
    {
        ObjectUpdateFlags = FArianeObject::EUpdateFlags::Interactive;
    }

    if( bUpdateUsedMaterials )
    {
        UpdateUsedMaterials();

        if( VectorLayer.IsValid() )
        {
            // we need to rebuild the proxy
            VectorLayer->MarkRenderStateDirty();
        }

        bUpdateUsedMaterials = false;
    }

    RootGroupID.GetObject( this )->Update( ObjectUpdateFlags, true );

    // Detached: rebuilt when attached again (UArianeLayerVector::SetDrawing)
    if( bRenderItemsNeedRebuild && VectorLayer.IsValid() )
    {
        RebuildRenderItems();
    }
}

const TArray<FArianeMeshRenderItem>&
UArianeDrawing::GetRenderItems() const
{
    return RenderItems;
}

FArianeMeshRenderItem&
UArianeDrawing::GetRenderItem( int32 Index )
{
    return RenderItems[Index];
}

void
UArianeDrawing::EnqueueRenderItem( int32 Index )
{
    // Detached drawing: nothing displays it
    if( VectorLayer.IsValid() )
    {
        VectorLayer->EnqueueRenderItem( Index, RenderItems[Index] );
    }
}

void
UArianeDrawing::RebuildRenderItems()
{
    // Game thread only: filled in place
    RenderItems.Reset();

    FArianeObject::Traverse( RootGroupID.GetObject( this )
                           , FArianeObject::ETraversalPolicy::AllChildren
                           , [ this ]( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->HasBaseClass( FArianeMesh::StaticClass() ) )
            {
                FArianeMesh* Mesh = static_cast<FArianeMesh*>(Object);

                Mesh->SetRenderIndex( RenderItems.Num() );
                Mesh->UpdateRenderItem( RenderItems.AddDefaulted_GetRef() );
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );

    if( VectorLayer.IsValid() )
    {
        VectorLayer->EnqueueRenderItems( RenderItems );
    }

    bRenderItemsNeedRebuild = false;
}

void
UArianeDrawing::InitVertexFactories()
{
    FArianeObject::Traverse( RootGroupID.GetObject( this )
                           , FArianeObject::ETraversalPolicy::AllChildren
                           , []( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->HasBaseClass( FArianeMesh::StaticClass() ) )
            {
                FArianeMeshGeometry3D& Geometry3D = static_cast<FArianeMesh*>(Object)->GetGeometry3D();

                if( Geometry3D.GetVertexFactory() == nullptr )
                {
                    Geometry3D.InitVertexFactory();
                }
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

void
UArianeDrawing::UpdateUsedMaterials()
{
    UsedMaterials.Empty();

    FArianeObject::Traverse( RootGroupID.GetObject( this )
                            , FArianeObject::ETraversalPolicy::AllChildren
                            , [ this ] ( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->HasBaseClass( FArianePath::StaticClass() ) )
            {
                FArianePath* Path = static_cast<FArianePath*>(Object);

                if( Path->GetMaterial() )
                {
                    UsedMaterials.AddUnique( Path->GetMaterial() );
                }
            }

            if( Object->HasBaseClass( FArianeCycle::StaticClass() ) )
            {
                FArianeCycle* Cycle = static_cast<FArianeCycle*>(Object);

                if( Cycle->GetMaterial() )
                {
                    UsedMaterials.AddUnique( Cycle->GetMaterial() );
                }
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

void
UArianeDrawing::Invalidate()
{
    // VectorLayer can be null for orphan images (stored in animation keys)
    if( VectorLayer.IsValid() )
    {
        VectorLayer->Invalidate();
    }

    // check for materials and render items.
    // Not skipped when the materials are already to update: the Hierarchy flag is needed too
    FArianeObject::Traverse( RootGroupID.GetObject( this )
                           , FArianeObject::ETraversalPolicy::InvalidatedChildren
                           , [ this ] ( FArianeObject* Object ) -> FArianeObject::ETraversalReturnValue
        {
            if( Object->GetInvalidationFlags().Color || Object->GetInvalidationFlags().Hierarchy )
            {
                bUpdateUsedMaterials = true;
            }

            if( Object->GetInvalidationFlags().Hierarchy )
            {
                // Meshes added, removed or reordered: the RenderIndex change
                bRenderItemsNeedRebuild = true;
            }

            return FArianeObject::ETraversalReturnValue::Continue;
        } );
}

/*
void
UArianeDrawing::BindDelegates()
{
    if( RootGroupID.GetObject( this ) )
    {
        RootGroupID.GetObject( this )->GetOnPostInvalidatedDelegate().AddUObject( this, &UArianeDrawing::OnRootObjectInvalidated );
    }
}

void
UArianeDrawing::UnbindDelegates()
{
    if( RootGroupID.GetObject( this ) )
    {
        RootGroupID.GetObject( this )->GetOnPostInvalidatedDelegate().RemoveAll( this );
    }
}
*/

void
UArianeDrawing::ResetHierarchy()
{
    //UnbindDelegates( );

    InstancedObjects.Empty();
    UsedMaterials.Empty();
    SelectedObjects.Empty();

    RootGroupID = FArianeObjectID( AllocGroup( "Root Group", EArianeAllocationModel::InstancedStruct ) );

    RootGroupID.GetObject( this )->ResetDrawing( this );
    RootGroupID.GetObject( this )->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );
    RootGroupID.GetObject( this )->UpdateTransform();



    //BindDelegates();
}

void
UArianeDrawing::DeleteInstancedObject( FArianeObject* Object )
{
    InstancedObjects.RemoveAll( [Object]( FInstancedStruct& Struct ) -> bool
    {
        return ( Object == Struct.GetPtr<FArianeObject>() ) ? true : false;
    } );
}

void
UArianeDrawing::ClearObjectSelection()
{
    SelectedObjects.RemoveAll(
        []( FArianeObject* SelectedObject )
        {
            SelectedObject->SetSelected( false );

            return true;
        } );
}

void
UArianeDrawing::UnselectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Remove( ObjectToSelect );

    ObjectToSelect->SetSelected( false );
}

void
UArianeDrawing::SelectObject( FArianeObject* ObjectToSelect )
{
    SelectedObjects.Add( ObjectToSelect );

    ObjectToSelect->SetSelected( true );
}

const TArray<FArianeObject*>&
UArianeDrawing::GetSelectedObjects() const
{
    return SelectedObjects;
}

TArray<FArianeObject*>&
UArianeDrawing::GetSelectedObjects()
{
    return SelectedObjects;
}

void
UArianeDrawing::AppendSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
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
UArianeDrawing::GetSelectedTrees( TArray<FArianeObject*>& SelectedTrees )
{
    SelectedTrees.Empty();

    AppendSelectedTrees( SelectedTrees );
}

#undef LOCTEXT_NAMESPACE
