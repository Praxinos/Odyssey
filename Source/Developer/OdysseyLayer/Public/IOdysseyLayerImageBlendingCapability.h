// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "IOdysseyLayerImageRenderingCapability.h"

#include <ULIS3>

class FOdysseyBlock;
class IOdysseyLayer;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayerImageBlendingCapability : public IOdysseyLayerImageRenderingCapability
{
public:
    // Layer Blending Mode Changed Event
    // ::ul3::eBlendingMode is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerBlendingModeChanged, ::ul3::eBlendingMode);
    
    // Layer Opacity Changed Event
    // float is for the previous value
    DECLARE_MULTICAST_DELEGATE_OneParam(FOdysseyLayerOpacityChanged, float);

public:
    // Construction / Destruction
    virtual ~IOdysseyLayerImageBlendingCapability() = 0;
    IOdysseyLayerImageBlendingCapability( const IOdysseyLayerImageBlendingCapability& iLayer );
    IOdysseyLayerImageBlendingCapability( ::ul3::eBlendingMode iBlendingMode = ::ul3::BM_NORMAL, float iOpacity = 1.0f );

public:
    //Returns all capabilities guid compatible with this capability
    static TArray<FGuid> GetGuids();

    //Returns only the guid of THIS capability
    static const FGuid& GetGuid();

public:
    virtual ::ul3::eBlendingMode    GetBlendingMode() const;
    virtual FText                   GetBlendingModeAsText() const;
    virtual TArray< TSharedPtr< FText > > GetBlendingModesAsText();
    virtual void                    SetBlendingMode(::ul3::eBlendingMode iBlendingMode);
    virtual void                    SetBlendingMode(FText iBlendingMode);

    virtual float GetOpacity() const;
    virtual void  SetOpacity(float iOpacity);

    virtual void Blend(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) = 0;

    void SerializeImageBlendingCapability(FArchive &Ar);

	template<typename T>
    static void* GetCapabilityPtrFromGuid(T* iValue, FGuid iGuid)
    {
        if (GetGuid() == iGuid)
            return reinterpret_cast<void*>(static_cast<IOdysseyLayerImageBlendingCapability*>(iValue));

        return nullptr;
    }

public:
    FOdysseyLayerBlendingModeChanged& BlendingModeChangedDelegate();
    FOdysseyLayerOpacityChanged& OpacityChangedDelegate();

private:
    void OnBlendingModeChanged(::ul3::eBlendingMode iOldValue);
    void OnOpacityChanged(float iOldValue);

protected:
    ::ul3::eBlendingMode    mBlendingMode;
    float                   mOpacity;

    FOdysseyLayerBlendingModeChanged mBlendingModeChangedDelegate;
    FOdysseyLayerOpacityChanged mOpacityChangedDelegate;
};