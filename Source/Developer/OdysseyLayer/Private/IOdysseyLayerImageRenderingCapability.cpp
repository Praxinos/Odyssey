// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
