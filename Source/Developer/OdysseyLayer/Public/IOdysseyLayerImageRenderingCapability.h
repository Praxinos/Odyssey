// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include <ULIS3>

class FOdysseyBlock;
class IOdysseyLayer;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayerImageRenderingCapability
{
public:
    // Layer Blending Mode Changed Event
    // ::ul3::eBlendingMode is for the previous value
    DECLARE_MULTICAST_DELEGATE(FOdysseyLayerImageResultChanged);

public:
    // Construction / Destruction
    virtual ~IOdysseyLayerImageRenderingCapability() = 0;
    IOdysseyLayerImageRenderingCapability(const IOdysseyLayerImageRenderingCapability& iCapability);
    IOdysseyLayerImageRenderingCapability();

public:
    //Returns all capabilities guid compatible with this capability
    static TArray<FGuid> GetGuids();

    //Returns only the guid of THIS capability
    static const FGuid& GetGuid();

	template<typename T>
    static void* GetCapabilityPtrFromGuid(T* iValue, FGuid iGuid)
    {
        if (GetGuid() == iGuid)
            return reinterpret_cast<void*>(static_cast<IOdysseyLayerImageRenderingCapability*>(iValue));

        if (IOdysseyLayerImageRenderingCapability::GetGuid() == iGuid)
            return IOdysseyLayerImageRenderingCapability::GetCapabilityPtrFromGuid(iValue, iGuid);

        return nullptr;
    }

public:
    virtual void RenderImage(::ul3::FBlock* ioBlock, const ::ul3::FRect& iRect, ::ul3::FVec2F iPos) = 0;

public:
    FOdysseyLayerImageResultChanged& ImageResultChangedDelegate();

protected:
    FOdysseyLayerImageResultChanged mImageResultChangedDelegate;
};
