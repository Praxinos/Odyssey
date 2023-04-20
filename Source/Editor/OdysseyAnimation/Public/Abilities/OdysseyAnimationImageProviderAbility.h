// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"

/* class ODYSSEYANIMATION_API FOdysseyAnimationImageProviderAbility
    : public IOdysseyAbility
{
public:
    static const FGuid& Id()
    {
        static FGuid Id(0x8b511cae, 0x20a24b19, 0x975d2347, 0x58d024de);
        return Id;
    }

public:
    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnBlockDataDataChanged, FOdysseyAnimationImageProviderAbility*, const FOdysseyAnimationRenderImageIdFragment&, const TArray<::ULIS::FRectI>&)
    static FOnBlockDataChanged& OnBlockDataChanged();
    , const TArray<::ULIS::FRectI>&
    OnRenderImageChanged()

    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnBlockDataDataChanged, FOdysseyAnimationImageProviderAbility*, const TArray<::ULIS::FRectI>&)
    static FOnBlockDataChanged& OnBlockDataChanged();

    DECLARE_MULTICAST_DELEGATE_ThreeParams(FOnBlockDataDataCommited, FOdysseyAnimationImageProviderAbility*, const TArray<::ULIS::FRectI>&)
    static FOnBlockDataCommited& OnBlockDataCommited();

public:
    //virtual TSharedPtr<::ULIS::FBlock> RenderImage(uint32 iFrameIndex) const = 0;

    virtual TSharedPtr<::ULIS::FBlock> GetBlock(uint32 iFrameIndex) const = 0;
    //virtual FString GetFrameId(uint32 iFrameIndex) const = 0;
    virtual const FOdysseyAnimationRenderImageId& GetRenderFrameIdFragment() const = 0;
}; */