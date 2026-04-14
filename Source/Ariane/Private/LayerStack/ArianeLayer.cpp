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
    , InvalidationFlags ( nullptr )
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
    UArianeLayerFolder* Candidate = this->GetParentFolder();

    while ( Candidate )
    {
        if( Candidate->GetParentFolder() == nullptr )
        {
            return Candidate;
        }

        Candidate = Candidate->GetParentFolder();
    }

    return nullptr;
}

UArianeLayerStack*
UArianeLayer::GetLayerStack()
{
    return Cast<UArianeLayerStack>(GetOuter());
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
    UArianeLayerFolder* ParentLayer = GetParentFolder();

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
UArianeLayer::SetParentFolder( UArianeLayerFolder* InParentFolder )
{
    ParentFolder = InParentFolder;
}

UArianeLayerFolder*
UArianeLayer::GetParentFolder()
{
    return ParentFolder;
}

void
UArianeLayer::Invalidate( const FArianeLayerInvalidationFlags& InInvalidationFlags )
{
    if( ParentFolder )
    {
        ParentFolder->InvalidateChildLayer( this );
    }

    InvalidationFlags->OR( InInvalidationFlags );

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
UArianeLayer::Update( bool Interactive )
{
    OnPreUpdate.Broadcast( Interactive );

    bInvalidated = false;

    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();

    OnPostUpdate.Broadcast( Interactive );
}

UArianeLayer::FOnUpdateDelegate&
UArianeLayer::OnPreUpdateDelegate()
{
    return OnPreUpdate;
}

UArianeLayer::FOnUpdateDelegate&
UArianeLayer::OnPostUpdateDelegate()
{
    return OnPostUpdate;
}
