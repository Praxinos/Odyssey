// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDHandle.h"
#include "CanvasTypes.h"

#define HANDLE_SMALL_SIZE 10
#define HANDLE_BIG_SIZE 15

struct HOdysseyHUDHandleHitProxy : public HOdysseyHUDElementHitProxy
{
	DECLARE_HIT_PROXY();

    HOdysseyHUDHandleHitProxy(TSharedPtr<FOdysseyHUDHandle> iHUDHandle, TOptional<EMouseCursor::Type> iMouseCursor = TOptional<EMouseCursor::Type>())
        : HOdysseyHUDElementHitProxy(iHUDHandle, iMouseCursor)
	{
	}  
};

IMPLEMENT_HIT_PROXY(HOdysseyHUDHandleHitProxy, HOdysseyHUDElementHitProxy)

FOdysseyHUDHandle::~FOdysseyHUDHandle()
{

}

FOdysseyHUDHandle::FOdysseyHUDHandle(FName iName, FVector2D* iReferencePoint)
    : FOdysseyHUDElement(iName)
    , mHandleTexture(LoadObject<UTexture>(nullptr, TEXT("/Iliad/HUD/T_HUD_Handle")))
    , mHandleMaterial(LoadObject<UMaterial>(nullptr, TEXT("/Iliad/HUD/M_HUD_Handle")))
{
    mReferencePoint = iReferencePoint;
}

void
FOdysseyHUDHandle::DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams)
{
    const FLinearColor color(1.f, 0.f, 0.f);
    
    if (iParams.mCanvas->IsHitTesting() && mIsInteractable)
	    iParams.mCanvas->SetHitProxy(new HOdysseyHUDHandleHitProxy(SharedThis(this)));
        
    int handleSize = mIsHovered && mIsInteractable ? HANDLE_BIG_SIZE : HANDLE_SMALL_SIZE;

    FVector2D origin = iParams.mTextureToHUD.Execute(*mReferencePoint) - FVector2D(handleSize / 2.f, handleSize / 2.f);

    FMaterialRenderProxy* handleMaterialProxy = mHandleMaterial->GetRenderProxy();
    iParams.mCanvas->DrawTile(origin.X, origin.Y, handleSize, handleSize, 0, 0, 1.f, 1.f, color, mHandleTexture->GetResource(), true);

    if (iParams.mCanvas->IsHitTesting() && mIsInteractable)
	    iParams.mCanvas->SetHitProxy(nullptr);

    FOdysseyHUDElement::DrawHUD(iParams); 
}

bool
FOdysseyHUDHandle::IsInteractable() const
{
    return mIsInteractable;
}

void
FOdysseyHUDHandle::IsInteractable(bool iIsInteractable)
{
    mIsInteractable = iIsInteractable;
}

bool
FOdysseyHUDHandle::IsPositionLocked() const
{
    return mIsPositionLocked;
}

void
FOdysseyHUDHandle::IsPositionLocked(bool iIsPositionLocked)
{
    mIsPositionLocked = iIsPositionLocked;
}

void FOdysseyHUDHandle::SetPosition(FVector2D iNewPosition)
{
    if( mReferencePoint )
        mReferencePoint->Set( iNewPosition.X, iNewPosition.Y );
}

FVector2D FOdysseyHUDHandle::GetPosition()
{
    if( mReferencePoint )
        return *mReferencePoint;

    return FVector2D( -1, -1 );
}

//HitProxy version
bool
FOdysseyHUDHandle::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey == EKeys::LeftMouseButton)
    {
        Capture(true);
        mOnDragBegin.Broadcast();
        return true;  //capture the event
    }
    
    return FOdysseyHUDElement::OnMouseDown(iPointInTexture, iKey);
}

bool
FOdysseyHUDHandle::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    if (iKey == EKeys::LeftMouseButton)
    {
        Capture(false);
        mOnDragEnd.Broadcast();
        return true; //capture the event
    }

    return FOdysseyHUDElement::OnMouseUp(iPointInTexture, iKey);
}

void
FOdysseyHUDHandle::OnMouseEnter()
{
    mIsHovered = true;
}

void
FOdysseyHUDHandle::OnMouseLeave()
{
    mIsHovered = false;
}

void
FOdysseyHUDHandle::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{
    if (!mIsPositionLocked)
        mReferencePoint->Set(iPointInTexture.x, iPointInTexture.y);

    mOnDragged.Broadcast();
}

FSimpleMulticastDelegate&
FOdysseyHUDHandle::OnDragBegin()
{
    return mOnDragBegin;
}

FSimpleMulticastDelegate&
FOdysseyHUDHandle::OnDragged()
{
    return mOnDragged;
}

FSimpleMulticastDelegate&
FOdysseyHUDHandle::OnDragEnd()
{
    return mOnDragEnd;
}
