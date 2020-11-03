// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyLayerImageRenderingCapability.h"

//---

IOdysseyLayerImageRenderingCapability::~IOdysseyLayerImageRenderingCapability()
{
}

IOdysseyLayerImageRenderingCapability::IOdysseyLayerImageRenderingCapability(const IOdysseyLayerImageRenderingCapability& iCapability)
{
}

IOdysseyLayerImageRenderingCapability::IOdysseyLayerImageRenderingCapability()
{
}

//---

IOdysseyLayerImageRenderingCapability::FOdysseyLayerImageResultChanged&
IOdysseyLayerImageRenderingCapability::ImageResultChangedDelegate()
{
    return mImageResultChangedDelegate;
}

const FGuid&
IOdysseyLayerImageRenderingCapability::GetGuid()
{
	static FGuid guid = FGuid::NewGuid();
	return guid;
}

TArray<FGuid>
IOdysseyLayerImageRenderingCapability::GetGuids()
{
    TArray<FGuid> guids;
    guids.Add(GetGuid());
    return guids;
}
