// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyHUDElement.h"
#include "OdysseyHUD.h"
#include "Engine/Texture.h"

/////////////////////////////////////////////////////
// FOdysseyHUDHandle
class ODYSSEYHUD_API FOdysseyHUDHandle : public FOdysseyHUDElement
{
public:
    // Destructor
    virtual ~FOdysseyHUDHandle();

    //Constructor
    FOdysseyHUDHandle( const FVector2D& iPosition );

//FOdysseyHUDElement overrides
public:
    virtual void DrawHUD(const FOdysseyHUD::FDrawHUDParams& iParams) override;

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

    void SetPosition(const FVector2D& iPosition);
    const FVector2D& GetPosition() const;

    FSimpleMulticastDelegate& OnDragged();
    FSimpleMulticastDelegate& OnDragBegin();
    FSimpleMulticastDelegate& OnDragEnd();

protected:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;

private:
    FVector2D mPosition;

    TObjectPtr<UTexture> mHandleTexture;
    FSimpleMulticastDelegate mOnDragged;
    FSimpleMulticastDelegate mOnDragBegin;
    FSimpleMulticastDelegate mOnDragEnd;

    bool mIsInteractable = true;
    bool mIsPositionLocked = false;
    bool mIsHovered = false;
};
