// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePath.h"

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


FArianeObject*
UArianeLayerDrawing::GetRootObject()
{
    return RootObjectID.GetObject();
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
    RootObjectID.InvalidateCache();

    BindDelegates();

    // Empty the map before reloading. Each path will populate or increment the attached value.
    UsedMaterials.Empty();

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
    RootObjectID.InvalidateCache();

    BindDelegates();

    // Empty the map before reloading. Each path will populate or increment the attached value.
    UsedMaterials.Empty();

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
UArianeLayerDrawing::AllocObject()
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianeObject>( this ) );
    InstancedObjectsAccessRW.Unlock();

    FArianeObject* NewObject = InstancedObjects.Last().GetMutablePtr<FArianeObject>();

    return NewObject;
}

FArianePath*
UArianeLayerDrawing::AllocPath( UMaterialInterface* MaterialInterface )
{
    InstancedObjectsAccessRW.Lock();
    InstancedObjects.Add( FInstancedStruct::Make<FArianePath>( this ) );
    InstancedObjectsAccessRW.Unlock();

    FArianePath* NewPath = InstancedObjects.Last().GetMutablePtr<FArianePath>();

    NewPath->SetMaterial( MaterialInterface ? MaterialInterface
                                            : GEngine->VertexColorMaterial );

    // Will force the creation of a render proxy, which will retrieve all the materials used to draw the meshes.
    GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();

    return NewPath;
}

const TMap<UMaterialInterface*, uint32>&
UArianeLayerDrawing::GetUsedMaterials()
{
    return UsedMaterials;
}

void
UArianeLayerDrawing::IncrementMaterial( UMaterialInterface* MaterialInterface )
{
    UsedMaterials.FindOrAdd( MaterialInterface )++;
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
}

TArray<FInstancedStruct>&
UArianeLayerDrawing::GetInstancedObjects()
{
    return InstancedObjects;
}

void
UArianeLayerDrawing::Update( bool bInteractive )
{
    RootObjectID.GetObject()->Update( true );

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
    if( RootObjectID.GetObject() )
    {
        RootObjectID.GetObject()->GetOnPostInvalidatedDelegate().AddUObject( this, &UArianeLayerDrawing::OnRootObjectInvalidated );
    }
}

void
UArianeLayerDrawing::UnbindDelegates()
{
    if( RootObjectID.GetObject() )
    {
        RootObjectID.GetObject()->GetOnPostInvalidatedDelegate().RemoveAll( this );
    }
}

void
UArianeLayerDrawing::ResetHierarchy()
{
    UnbindDelegates( );

    InstancedObjects.Empty();

    RootObjectID = FArianeObjectID( AllocObject() );

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
