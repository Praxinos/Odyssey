// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include <ULIS>

class IOdysseyLayer;

/**
 * Odyssey Layer
 * An abstract class for a layer, which can be of various types (drawing, sound, folder...)
 */
class ODYSSEYLAYER_API IOdysseyLayerImageRenderingCapability
{
public:
    // Can be triggered with nullptr and 0 size, this means change the whole size.
    DECLARE_MULTICAST_DELEGATE_TwoParams( FOdysseyLayerImageResultChanged, const ::ULIS::FRectI*, const uint32 );

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
    virtual TArray<::ULIS::FEvent> RenderImage( ::ULIS::FBlock** ioBlocks, const ::ULIS::FRectI* iRects, const ::ULIS::FVec2I* iPositions, const uint32 iNum ) = 0;

public:
    FOdysseyLayerImageResultChanged& ImageResultChangedDelegate();

public:
    FOdysseyLayerImageResultChanged mImageResultChangedDelegate;
};
