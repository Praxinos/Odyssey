// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"
#include "OdysseyHUDSystem.h"

/////////////////////////////////////////////////////
// FOdysseyHUDHandle
class ODYSSEYHUDSYSTEM_API FOdysseyHUDHandle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDHandle();

    //Constructor
    FOdysseyHUDHandle( FName iName, FVector2D* iReferencePoint );

//FOdysseyHUDElement overrides
public:
    virtual void DrawHUD(const FOdysseyHUDSystem::FDrawHUDParams& iParams) override;

public:
    //HitProxy version
    virtual bool OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual bool OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey) override;
    virtual void OnMouseEnter() override;
    virtual void OnMouseLeave() override;
    virtual void OnMouseDrag(const FOdysseyPoint& iPointInTexture) override;

public:
    bool IsInteractable() const;
    void IsInteractable(bool iIsInteractable);

    bool IsPositionLocked() const;
    void IsPositionLocked(bool iIsPositionLocked);

    void SetPosition(FVector2D iNewPosition);
    FVector2D GetPosition();

    FSimpleMulticastDelegate& OnDragged();
    FSimpleMulticastDelegate& OnDragBegin();
    FSimpleMulticastDelegate& OnDragEnd();

private:
    FVector2D* mReferencePoint;

    
    UTexture* mHandleTexture;
    UMaterial* mHandleMaterial;
    FSimpleMulticastDelegate mOnDragged;
    FSimpleMulticastDelegate mOnDragBegin;
    FSimpleMulticastDelegate mOnDragEnd;

    bool mIsInteractable = true;
    bool mIsPositionLocked = false;
    bool mIsHovered = false;
};
