// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYANIMATION_API FOdysseyAnimationFrameProxy
{
public:
    virtual ~FOdysseyAnimationProxy();
    FOdysseyAnimationProxy(TSharedPtr<IOdysseyImageRenderer> iRenderer);

public:
    TSharedPtr<::ULIS::FBlock> GetBlock();
	void Serialize(FArchive& Ar);

private:
    void OnImageRenderingPreChanged(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);

private:
    TSharedPtr<IOdysseyImageRenderer> mRenderer;
    TArray<FGuid> mBlockComposition;
    TSharedPtr<FOdysseyRasterBlock> mBlock;
    TArray<::ULIS::FRectI> mInvalidRects;
};