// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"

UArianeLayer::~UArianeLayer()
{
}

UArianeLayer::UArianeLayer()
    : //bVisible ( true )
      bLocked ( false )
    , bSelected ( false )
    , bInvalidated ( false )
    , Bounds ( FBoxSphereBounds(ForceInit) )
{
    // for Transform operations
    SetMobility(EComponentMobility::Movable);
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

UArianeLayerFolder*
UArianeLayer::GetRootFolder()
{
    UArianeLayer* Candidate = this;

    while ( Candidate )
    {
        if( Cast<UArianeLayerStack>(Candidate->GetOuter()) )
        {
            return Cast<UArianeLayerFolder>(Candidate);
        }

        Candidate = Candidate->GetParent();
    }

    return nullptr;
}

UArianeLayerStack*
UArianeLayer::GetLayerStack()
{
    UArianeLayerFolder* RootFolder = GetRootFolder();

    return RootFolder ? Cast<UArianeLayerStack>(RootFolder->GetOuter()) : nullptr;
}

UArianeLayerFolder*
UArianeLayer::GetParent()
{
    // Note: for the root folder, this will return null due to the cast.
    // Indeed, the Outer Object is the layer stack in that case
    return Cast<UArianeLayerFolder>(GetOuter());
}

/*
void
UArianeLayer::SetVisible( bool bInVisible )
{
    bVisible = bInVisible;
}

bool
UArianeLayer::IsVisible( bool bHierarchical )
{
    UArianeLayerFolder* ParentLayer = GetParent();

    if( bHierarchical && ParentLayer )
    {
        return ParentLayer->IsVisible( bHierarchical ) && bVisible;
    }

    return bVisible;
}
*/

void
UArianeLayer::SetLocked( bool bInLocked )
{
    bLocked = bInLocked;
}

bool
UArianeLayer::IsLocked( bool bHierarchical )
{
    UArianeLayerFolder* ParentLayer = GetParent();

    if( bHierarchical && ParentLayer )
    {
        return ParentLayer->IsLocked( bHierarchical ) || bLocked;
    }

    return bLocked;
}

#if WITH_EDITOR
void
UArianeLayer::SetSelected( bool bInSelected )
{
    bSelected = bInSelected;
}

bool
UArianeLayer::IsSelectedInEditor() const
{
    return bSelected;
}
#endif

void
UArianeLayer::Invalidate()
{
    UArianeLayerFolder* ParentFolder = GetParent();

    if( ParentFolder )
    {
        ParentFolder->InvalidateChildLayer( this );
    }

    //GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();
}

bool UArianeLayer::IsInvalidated()
{
    return bInvalidated;
}

void UArianeLayer::SetInvalidated( bool bInInvalidated )
{
    bInvalidated = bInInvalidated;
}

const FBoxSphereBounds&
UArianeLayer::GetBounds()
{
    return Bounds;
}

void
UArianeLayer::Update()
{
    bInvalidated = false;

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();
}
