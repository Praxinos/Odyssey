// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDHandle.h"

#include "CanvasTypes.h"
#include "Engine/Texture.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyHUDElement.h"
#include "TextureResource.h"

#define HANDLE_SMALL_SIZE 10
#define HANDLE_BIG_SIZE 15

struct HOdysseyHUDHandleHitProxy : public HOdysseyHUDElementHitProxy
{
    DECLARE_HIT_PROXY();

    HOdysseyHUDHandleHitProxy( TSharedPtr<FOdysseyHUDHandle> iHUDHandle
                             , TOptional<EMouseCursor::Type> iMouseCursor = TOptional<EMouseCursor::Type>() )
        : HOdysseyHUDElementHitProxy(iHUDHandle, iMouseCursor)
    {
    }
};

IMPLEMENT_HIT_PROXY(HOdysseyHUDHandleHitProxy, HOdysseyHUDElementHitProxy)

FOdysseyHUDHandle::~FOdysseyHUDHandle()
{

}

FOdysseyHUDHandle::FOdysseyHUDHandle(const FVector2D& iPosition)
    : mPosition(iPosition)
    , mHandleTexture(LoadObject<UTexture>(nullptr, TEXT("/Odyssey/HUD/T_HUD_Handle")))
{
}

void
FOdysseyHUDHandle::DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams)
{
    const FLinearColor color(1.f, 0.f, 0.f);

    if (iParams.mCanvas->IsHitTesting() && mIsInteractable)
        iParams.mCanvas->SetHitProxy(new HOdysseyHUDHandleHitProxy(SharedThis(this)));

    int handleSize = mIsHovered && mIsInteractable ? HANDLE_BIG_SIZE : HANDLE_SMALL_SIZE;

    FVector2D origin = iParams.mTextureToHUD.Execute(mPosition) - FVector2D(handleSize / 2.f, handleSize / 2.f);

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

void FOdysseyHUDHandle::SetPosition(const FVector2D& iPosition)
{
    mPosition = iPosition;
}

const FVector2D&
FOdysseyHUDHandle::GetPosition() const
{
    return mPosition;
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
        mPosition = iPointInTexture;

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

void
FOdysseyHUDHandle::AddReferencedObjects(FReferenceCollector& Collector)
{
    FOdysseyHUDElement::AddReferencedObjects(Collector);

    Collector.AddReferencedObject(mHandleTexture);
}
