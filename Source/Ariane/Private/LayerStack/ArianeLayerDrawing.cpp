// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"
#include "ArianeGroup.h"

UArianeLayerDrawing::~UArianeLayerDrawing()
{
    UnbindDelegates();
}

UArianeLayerDrawing::UArianeLayerDrawing()
    : DrawingOrigin ( EArianeLayerDrawingOrigin::Layer )
    , DrawingOrientation ( EArianeLayerDrawingOrientation::View )
{
    InvalidationFlags = new FArianeLayerInvalidationFlags();

    ResetHierarchy();

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

    Update( false );

    // recompute the bounding volumes or else nothing will draw
    GetLayerStack()->GetPainting3DComponent()->UpdateComponentToWorld();
}

void
UArianeLayerDrawing::PostEditUndo()
{
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

    Update( false );

    // recompute the bounding volumes or else nothing will draw
    GetLayerStack()->GetPainting3DComponent()->UpdateComponentToWorld();
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
UArianeLayerDrawing::AllocObject( const FName& InName )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this, InName, EArianeAllocationModel::InstancedStruct ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();

    return NewObject;
}

FArianeGroup*
UArianeLayerDrawing::AllocGroup( const FName& InName )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeGroup>( this, InName, EArianeAllocationModel::InstancedStruct ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeGroup* NewGroup = InstancedObjects.Last().GetMutablePtr<FArianeGroup>();

    return NewGroup;
}

FArianePath*
UArianeLayerDrawing::AllocPath( UMaterialInterface* InMaterialInterface, const FName& InName )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianePath>( this, InName, EArianeAllocationModel::InstancedStruct ) );
    InstancedObjectsAccessRW.Unlock();

    FArianePath* NewPath = InstancedObjects.Last().GetMutablePtr<FArianePath>();

    NewPath->SetMaterial( InMaterialInterface ? InMaterialInterface
                                              : GetLayerStack()->GetPainting3DComponent()->GetDefaultMaterial() );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();

    return NewPath;
}

void
UArianeLayerDrawing::AppendUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials )
{
    OutUsedMaterials.Reserve( OutUsedMaterials.Num() + UsedMaterials.Num() );

    for( auto Pair : UsedMaterials )
    {
        OutUsedMaterials.Add( Pair.Key );
    }
}

void
UArianeLayerDrawing::GetUsedMaterials( TArray<UMaterialInterface*>& OutUsedMaterials )
{
    OutUsedMaterials.Empty();

    AppendUsedMaterials( OutUsedMaterials );
}

void
UArianeLayerDrawing::IncrementMaterial( UMaterialInterface* MaterialInterface )
{
    uint32* value = UsedMaterials.Find( MaterialInterface );

    // not: do not use findOrAdd, it will not initialize value to zero.
    if( value == nullptr )
    {
        UsedMaterials.Add( MaterialInterface, 1 );
    }

    // we need to rebuild the proxy
    GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();
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
                                           // clear flags only if we are NOT interactive
    RootGroupID.GetObject()->Update( true, bInteractive ? false : true );

    UpdateBounds();

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

    RootGroupID = FArianeObjectID( AllocGroup( "Root Group" ) );

    RootGroupID.GetObject()->Invalidate( FArianeObjectInvalidationFlags().SetHierarchy() );

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

void
UArianeLayerDrawing::UpdateBounds()
{
    // ForceInit makes the box invalid and excludes it from the computation unitl it is valid
    Bounds = FBoxSphereBounds(ForceInit);

    for( const FInstancedStruct& InstancedObject : InstancedObjects )
    {
        const FArianeObject* Object = InstancedObject.GetPtr<FArianeObject>();

        if( const_cast<FArianeObject*>(Object)->GetClass() == FArianePath::StaticClass() )
        {
            const FArianePath* Path = static_cast<const FArianePath*>(Object);

            Bounds = Bounds + const_cast<FArianePath*>(Path)->GetBounds();
        }
    }
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
        SelectedObject->TraverseBackwards (
            [ SelectedObject
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
