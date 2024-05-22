// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"

/////////////////////////////////////////////////////
// FOdysseyHUDHandle
class ODYSSEYWIDGETS_API FOdysseyHUDHandle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDHandle();

    //Constructor
    FOdysseyHUDHandle( FName iName, FOdysseyHUDElement* iParent, FVector2D* iReferencePoint, FTransform2D iTransform = FTransform2D() );

//FOdysseyHUDElement overrides
public:
    virtual void Draw(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;
    virtual void Render(const FOdysseyHUDSystem::FRenderParams& iParams) override;
    virtual void MouseMove( const FOdysseyPoint& iPointInTexture ) override;
    virtual bool OnKeyDown( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual bool OnKeyUp( const FOdysseyPoint& iPointInTexture, FKey iKey ) override;
    virtual void CapturedMouseMove( const FOdysseyPoint& iPointInTexture ) override;
    virtual void Erase(::ULIS::FBlock* ioBlock, FTransform2D iTransform = FTransform2D()) override;

public:
    //HitProxy version
    /* virtual bool OnMouseDown(FHitProxyId iHitProxyId, const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseDoubleClick(FHitProxyId iHitProxyId, const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual bool OnMouseUp(FHitProxyId iHitProxyId, const FOdysseyPoint& iPointInTexture, const FKey& iKey);
    virtual void OnMouseHover(FHitProxyId iHitProxyId, const FOdysseyPoint& iPointInTexture);
    virtual void OnMouseDrag(FHitProxyId iHitProxyId, const FOdysseyPoint& iPointInTexture);
    virtual bool OnKeyDown(const FKey& iKey);
    virtual bool OnKeyUp(const FKey& iKey); */

public:
    void SetPosition(FVector2D iNewPosition);
    FVector2D GetPosition();

private:
    FOdysseyHUDElement* mParent;

    FVector2D* mReferencePoint;
    FVector2D mPreviousPosition;
    int mPreviousHandleSize;

private:
    int mHandleSize;
    //FHitProxyId mHitProxyId;
};
