// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "Engine/Texture.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "IOdysseyLayer.h"
#include "IOdysseyLayerImageBlendingCapability.h"
#include <ULIS3>

class FOdysseyBlock;

/**
 * Implements a layer which contains an image
 */
class ODYSSEYLAYER_API FOdysseyImageLayer :
    public IOdysseyLayer,
    public IOdysseyLayerImageBlendingCapability
{
public:
    // Layer Is Alpha Locked Changed Event
    // Bool is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerIsAlphaLockedChanged, bool);

public:
    // Construction / Destruction
    virtual ~FOdysseyImageLayer();
    FOdysseyImageLayer( const FOdysseyImageLayer& iLayer);
    FOdysseyImageLayer( const FName& iName,FVector2D iSize, ::ul3::tFormat iFormat);
    FOdysseyImageLayer( const FName& iName,FOdysseyBlock* iBlock);

    virtual FOdysseyImageLayer* Clone() const override; 
    virtual void Serialize(FArchive &Ar) override;
    virtual void Blend(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) override;
    virtual void RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) override;
    virtual bool ImplementsCapability(FGuid iGuid) const override;
    virtual void* GetCapabilityPtrFromGuid(FGuid iGuid) override;

public:
    // Public API
    FOdysseyBlock* GetBlock() const;
    void SetBlock(FOdysseyBlock* iBlock, bool iSendEvents = true, bool iDestroyPreviousBlock = true);

    bool  IsAlphaLocked() const;
    void  SetIsAlphaLocked(bool iIsAlphaLocked);

    virtual void SetIsVisible(bool iIsVisible) override;

public:
    FOdysseyLayerIsAlphaLockedChanged& IsAlphaLockedChangedDelegate();
	
private:
    // Private Data Members
    FOdysseyBlock*          mBlock;
    bool                    mIsAlphaLocked;

    FOdysseyLayerIsAlphaLockedChanged mIsAlphaLockedChangedDelegate;

    //TODO: Should we do an event for block invalidation ?
};
