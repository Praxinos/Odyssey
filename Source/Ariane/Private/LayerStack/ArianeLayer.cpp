// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"

UArianeLayer::~UArianeLayer()
{
}

UArianeLayer::UArianeLayer()
    : //bVisible ( true )
      bLocked ( false )
    , bSelected ( false )
    , bInvalidatedInParentFolder ( false )
    , Bounds ( FBoxSphereBounds(ForceInit) )
    , InvalidationFlags ( nullptr )
{
    // for Transform operations
    SetMobility(EComponentMobility::Movable);

    bWantsOnUpdateTransform = true;
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
    AArianePainting3DActor* Painting3DActor = Cast<AArianePainting3DActor>(GetOwner());

    return Painting3DActor->GetPainting3DComponent()->GetLayerStack();
    //return Cast<UArianeLayerStack>(GetOuter());
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
    if( ParentFolder && ( bInvalidatedInParentFolder == false ) )
    {
        ParentFolder->InvalidateChildLayer( this );

        bInvalidatedInParentFolder = true;
    }

    InvalidationFlags->OR( InInvalidationFlags );

    //GetLayerStack()->GetPainting3DComponent()->MarkRenderStateDirty();
}

void UArianeLayer::SetInvalidatedInParentFolder( bool bInInvalidatedInParentFolder )
{
    bInvalidatedInParentFolder = bInInvalidatedInParentFolder;
}

FArianeLayerInvalidationFlags*
UArianeLayer::GetInvalidationFlags()
{
    return InvalidationFlags;
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

UArianeLayer::FOnTransformChanged&
UArianeLayer::GetOnTransformChangedDelegate()
{
    return OnTransformChanged;
}

void
UArianeLayer::OnUpdateTransform(EUpdateTransformFlags UpdateTransformFlags, ETeleportType Teleport)
{
    OnTransformChanged.Broadcast();
}
