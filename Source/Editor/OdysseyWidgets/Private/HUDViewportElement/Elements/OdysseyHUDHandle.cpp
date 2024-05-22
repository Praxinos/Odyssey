// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDHandle.h"

#define HANDLE_SMALL_SIZE 1
#define HANDLE_BIG_SIZE 2

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
    , mHandleMaterial(LoadObject<UMaterial>(nullptr, TEXT("/Engine/EditorMaterials/WidgetVertexColorMaterial")))
{
    mHandleSize = HANDLE_SMALL_SIZE;
    mReferencePoint = iReferencePoint;
}

void
FOdysseyHUDHandle::Render(const FOdysseyHUDSystem::FRenderParams& iParams)
{
    const FLinearColor color(1.f, 0.f, 0.f);
    FVector handlePosition = iParams.mOrigin
        + mReferencePoint->X / iParams.mTextureWidth * iParams.mPlaneWidth * iParams.mXAxis
        + mReferencePoint->Y / iParams.mTextureHeight * iParams.mPlaneHeight * iParams.mYAxis;
    
    if (iParams.mPDI->IsHitTesting() && mIsInteractable)
	    iParams.mPDI->SetHitProxy(new HOdysseyHUDHandleHitProxy(SharedThis(this)));

    FMaterialRenderProxy* handleMaterialProxy = mHandleMaterial->GetRenderProxy();
    const int32 numSides = 64;
    DrawDisc(
        iParams.mPDI,
        handlePosition,
        iParams.mXAxis,
        iParams.mYAxis,
        color.ToFColor(true),
        mHandleSize,
        64,
        handleMaterialProxy,
        SDPG_Foreground
    );

    if (iParams.mPDI->IsHitTesting() && mIsInteractable)
	    iParams.mPDI->SetHitProxy(nullptr);

    FOdysseyHUDElement::Render(iParams);
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
FOdysseyHUDHandle::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{
    /* float distSquared = FVector2D::DistSquared(*mReferencePoint, FVector2D(iPointInTexture.x, iPointInTexture.y));
    if ( mHandleSize != HANDLE_BIG_SIZE && distSquared < HANDLE_BIG_SIZE * HANDLE_BIG_SIZE)
    {
        mHandleSize = HANDLE_BIG_SIZE;
        mIsInvalid = true;
    }
    else if( mHandleSize != HANDLE_SMALL_SIZE && distSquared > HANDLE_BIG_SIZE * HANDLE_BIG_SIZE )
    {
        mHandleSize = HANDLE_SMALL_SIZE;
        mIsInvalid = true;
    } */
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
