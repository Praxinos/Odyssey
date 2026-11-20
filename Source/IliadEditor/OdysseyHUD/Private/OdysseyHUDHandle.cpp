// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyHUDHandle.h"

#include "CanvasTypes.h"
#include "Engine/Texture.h"
#include "Input/OdysseyPoint.h"
#include "OdysseyHUDElement.h"
#include "TextureResource.h"

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
FOdysseyHUDHandle::DrawHUD(const FOdysseyHUDElement::FDrawHUDParams& iParams)
{
    if (iParams.mCanvas->IsHitTesting() && mIsInteractable)
        iParams.mCanvas->SetHitProxy(new HOdysseyHUDHandleHitProxy(SharedThis(this)));

    const float HandleSize = mIsHovered && mIsInteractable ? mHoveredSize : mSize;

    FVector2D position(mPosition);
    switch(GetReference())
    {
        case EOdysseyHUDReference::Texture:
        {
            position = iParams.mTextureToHUD.Execute(mPosition);
        }
        break;

        case EOdysseyHUDReference::HUD:
        {
            position = mPosition;
        }
        break;
    }

    FVector2D origin = position - FVector2D(HandleSize / 2.f, HandleSize / 2.f);

    if (mHandleTexture)
        iParams.mCanvas->DrawTile(origin.X, origin.Y, HandleSize, HandleSize, 0, 0, 1.f, 1.f, mColor, mHandleTexture->GetResource(), true);

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

bool FOdysseyHUDHandle::IsHovered() const
{
    return mIsHovered;
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

void FOdysseyHUDHandle::SetColor(const FLinearColor& iColor)
{
    mColor = iColor;
}

const FLinearColor& FOdysseyHUDHandle::GetColor() const
{
    return mColor;
}

void FOdysseyHUDHandle::SetSize(float iSize)
{
    mSize = FMath::Max(0.f, iSize);
}

float FOdysseyHUDHandle::GetSize() const
{
    return mSize;
}

void FOdysseyHUDHandle::SetHoveredSize(float iSize)
{
    mHoveredSize = FMath::Max(0.f, iSize);
}

float FOdysseyHUDHandle::GetHoveredSize() const
{
    return mHoveredSize;
}

void FOdysseyHUDHandle::SetHandleTexture(UTexture* iTexture)
{
    mHandleTexture = iTexture;
}

UTexture* FOdysseyHUDHandle::GetHandleTexture() const
{
    return mHandleTexture;
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
