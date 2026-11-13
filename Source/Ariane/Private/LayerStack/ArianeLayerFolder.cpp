// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerFolder.h"
#include "ArianeLayerFolderInvalidationFlags.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DActor.h"

UArianeLayerFolder::~UArianeLayerFolder()
{
}

UArianeLayerFolder::UArianeLayerFolder()
    : bExpanded ( true )
{
/*
    ResetHierarchy();

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
UArianeLayerFolder::OnRegister()
{
    Super::OnRegister();

    for (UArianeLayer* ChildLayer : ChildLayers )
    {
        ChildLayer->RegisterComponent();
    }
}

bool
UArianeLayerFolder::IsNavigationRelevant() const
{
    // DO NOT DRAW
    return false;
}

void
UArianeLayerFolder::SetExpanded( bool bInExpanded )
{
    bExpanded = bInExpanded;
}

bool
UArianeLayerFolder::IsExpanded()
{
    return bExpanded;
}

TArray<UArianeLayer*>&
UArianeLayerFolder::GetChildLayers()
{
    return ChildLayers;
}

const TArray<UArianeLayer*>&
UArianeLayerFolder::GetChildLayers() const
{
    return ChildLayers;
}

void
UArianeLayerFolder::AddChildLayer( UArianeLayer* Orphan )
{
    AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(GetOwner());

    //Orphan->SetParentFolder( this );

    ChildLayers.Add( Orphan );

    Invalidate();

    if ( Orphan->IsRegistered() )
    {
        Orphan->AttachToComponent( this,  FAttachmentTransformRules::KeepWorldTransform );
    }
    else
    {
        // Typically called when AddChildLayer is run in a constructor
        Orphan->SetupAttachment(this);
        Orphan->RegisterComponent();
    }

    // commented-out: this would make the layer appear in the outliner details view. We don't want that
    // because it would display the FName and not the displayName among other inconvient things.
    //Painting3DActor->AddInstanceComponent(Orphan);

    // Force Update transforms (will update the transform of the rootGroup of drawing layers)
    Orphan->OnUpdateTransform( EUpdateTransformFlags::None, ETeleportType::None );
}

void
UArianeLayerFolder::RemoveChildLayer( UArianeLayer* Child, bool bDestroy )
{
    if( bDestroy )
    {
         // will call void UArianeLayer::OnComponentDestroyed() which will in turn call RemoveChildLayer( Child, false )
        Child->DestroyComponent();
    }
    else
    {
        ChildLayers.Remove( Child );

        InvalidatedChildLayers.Remove( Child );

/*
        if( Child->IsSelected() )
        {
            GetLayerStack()->UnselectLayer( Child, true );
        }
*/

        Child->DetachFromComponent( FDetachmentTransformRules::KeepRelativeTransform );
    }

    Invalidate();
}

void UArianeLayerFolder::OnChildDetached( USceneComponent* ChildComponent )
{
    UArianeLayer* ChildLayer = Cast<UArianeLayer>(ChildComponent);

    Super::OnChildDetached(ChildComponent);

    if ( ChildLayer )
    {
        UArianeLayerStack* LayerStack = GetLayerStack();

        if( ChildLayer->IsSelected() && LayerStack )
        {
            LayerStack->UnselectLayer( ChildLayer, false );
        }

        ChildLayers.Remove(ChildLayer);
        InvalidatedChildLayers.Remove(ChildLayer);
        Invalidate();
    }
}

// static
UArianeLayerFolder::ETraversalReturnValue
UArianeLayerFolder::Traverse_Private( UArianeLayerFolder* FolderLayer, TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    ETraversalReturnValue Ret = Callback( FolderLayer );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == ETraversalReturnValue::IgnoreChildren ) == 0 )
    {
        for( UArianeLayer* Child : FolderLayer->ChildLayers )
        {
            UArianeLayerFolder* ChildFolder = Cast<UArianeLayerFolder>(Child);
            ETraversalReturnValue ChildRet = ChildFolder ? Traverse_Private( ChildFolder, Callback )
                                                         : Callback( Child );

            if( ChildRet == ETraversalReturnValue::Stop )
            {
                return ChildRet;
            }
        }
    }

    return Ret;
}

// static
void
UArianeLayerFolder::Traverse( UArianeLayerFolder* FolderLayer, TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    Traverse_Private( FolderLayer, Callback );
}

void
UArianeLayerFolder::InvalidateChildLayer( UArianeLayer* Child )
{
    InvalidatedChildLayers.Add( Child );
}

void
UArianeLayerFolder::Update( bool bInteractive )
{
    InvalidatedChildLayers.RemoveAll( [bInteractive] ( UArianeLayer* Layer ) -> bool
    {
        Layer->Update( bInteractive );

        bool bHasAnyFlags = Layer->IsInvalidated();
        // if the layer has any flag, it will remain in the list of invalidated layers
        Layer->SetInvalidatedInParentFolder( bHasAnyFlags );

        return bHasAnyFlags ? false : true;
    } );

    Super::Update( bInteractive );
}
