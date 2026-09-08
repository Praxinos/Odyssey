// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

// Ariane headers
#include "ArianeLayer.h"
#include "ArianeLayerFolder.h"
#include "ArianeLayerDrawing.h"
#include "ArianeLayerStack.h"
#include "ArianePainting3DComponent.h"
#include "ArianePainting3DActor.h"
#include "ArianeImage.h"
#include "ArianeCycle.h"
#include "ArianeVertex.h"
#include "ArianePath.h"
#include "ArianeGroup.h"

UArianeLayer::~UArianeLayer()
{
}

UArianeLayer::UArianeLayer()
    : //bVisible ( true )
      ParentFolder( nullptr )
    , bLocked ( false )
    , bSelected ( false )
    , bInvalidatedInParentFolder ( false )
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

    // Painting3DActor can be null if the layer is copied in the clipboard
    return Painting3DActor ? Painting3DActor->GetPainting3DComponent()->GetLayerStack() : nullptr;
    //return Cast<UArianeLayerStack>(GetOuter());
}

#if WITH_EDITOR
void
UArianeLayer::PreEditUndo()
{
    Super::PreEditUndo();
}

void
UArianeLayer::PostEditUndo()
{
    Super::PostEditUndo();

    if( IsRegistered() == false )
    {
        RegisterComponent();
    }
}
#endif

void
UArianeLayer::PostLoad()
{
    Super::PostLoad();
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
    MarkPackageDirty();
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

void
UArianeLayer::Update( bool Interactive )
{
    // will call CalcBounds (nb: calling UMeshComponent::UpdateBounds() does not work sometimes, especially when then
    // path starts empty but this works.
    UpdateComponentToWorld();
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

UArianeLayer::ETraversalReturnValue
UArianeLayer::TraverseBackwards_Private( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    ETraversalReturnValue Ret = Callback( this );

    if( Ret == ETraversalReturnValue::Stop )
    {
        return Ret;
    }

    if( ParentFolder )
    {
        ETraversalReturnValue ParentRet = ParentFolder->TraverseBackwards_Private( Callback );

        if( ParentRet == ETraversalReturnValue::Stop )
        {
            return ParentRet;
        }
    }

    return Ret;
}

void
UArianeLayer::TraverseBackwards( TFunction<ETraversalReturnValue(UArianeLayer*)> Callback )
{
    TraverseBackwards_Private( Callback );
}
