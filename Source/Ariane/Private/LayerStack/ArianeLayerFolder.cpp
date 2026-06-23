// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayerFolder.h"
#include "ArianeLayerFolderInvalidationFlags.h"

UArianeLayerFolder::~UArianeLayerFolder()
{
}

UArianeLayerFolder::UArianeLayerFolder()
    : bExpanded ( true )
{
    InvalidationFlags = new FArianeLayerFolderInvalidationFlags();
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
UArianeLayerFolder::SetExpanded( bool bInExpanded )
{
    bExpanded = bInExpanded;
}

bool
UArianeLayerFolder::IsExpanded()
{
    return bExpanded;
}

const TArray<UArianeLayer*>&
UArianeLayerFolder::GetChildLayers()
{
    return ChildLayers;
}

void
UArianeLayerFolder::AddChildLayer( UArianeLayer* Orphan )
{
    Orphan->SetParentFolder( this );

    ChildLayers.Add( Orphan );

    Invalidate( FArianeLayerFolderInvalidationFlags().SetHierarchy() );

    if ( Orphan->IsRegistered() )
    {
        Orphan->AttachToComponent( this,  FAttachmentTransformRules::KeepWorldTransform );
    }
    else
    {
        // Typically called when AddChildLayer is run in a constructor
        Orphan->SetupAttachment(this);
    }
}

void
UArianeLayerFolder::RemoveChildLayer( UArianeLayer* Child )
{
    Child->SetParentFolder( nullptr );

    ChildLayers.Remove( Child );

    InvalidatedChildLayers.Remove( Child );

    Invalidate( FArianeLayerFolderInvalidationFlags().SetHierarchy() );

    Child->DetachFromComponent( FDetachmentTransformRules::KeepWorldTransform );

    // Rename() is used to define the parent object
    Child->Rename( nullptr, nullptr );
}


UArianeLayerFolder::ETraversalReturnValue
UArianeLayerFolder::Traverse_Private( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    ETraversalReturnValue Ret = Callback( this );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ( Ret == ETraversalReturnValue::IgnoreChildren ) == 0 )
    {
        for( UArianeLayer* Child : ChildLayers )
        {
            UArianeLayerFolder* ChildFolder = Cast<UArianeLayerFolder>(Child);
            ETraversalReturnValue ChildRet = ChildFolder ? ChildFolder->Traverse_Private( Callback )
                                                        : Callback( Child );

            if( ChildRet == ETraversalReturnValue::Stop )
            {
                return ChildRet;
            }
        }
    }

    return Ret;
}

void
UArianeLayerFolder::Traverse( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    Traverse_Private( Callback );
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

        bool bHasAnyFlags = Layer->GetInvalidationFlags()->HasAny();
        // if the layer has any flag, it will remain in the list of invalidated layers
        Layer->SetInvalidatedInParentFolder( bHasAnyFlags );

        return bHasAnyFlags ? false : true;
    } );

    UpdateBounds();

    Super::Update( bInteractive );
}

void
UArianeLayerFolder::UpdateBounds()
{
    // ForceInit makes the box invalid and excludes it from the computation unitl it is valid
    Bounds = FBoxSphereBounds(ForceInit);

    for( UArianeLayer* Child : ChildLayers )
    {
        Bounds = Bounds + Child->GetBounds();
    }
}
