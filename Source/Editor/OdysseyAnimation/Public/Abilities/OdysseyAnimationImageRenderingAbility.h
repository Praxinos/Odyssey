// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"

class ODYSSEYANIMATION_API FOdysseyAnimationImageRenderingAbility
    : public IOdysseyAbility
{
public:
    static const FGuid& Id()
    {
        static FGuid Id(0x8b511cae, 0x20a24b19, 0x975d2347, 0x58d024de);
        return Id;
    }

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnRenderImageDataChanged, const FOnRenderImageDataChangedEvent& iEvent)

    //Called when some data of this object, related with image rendering changed interactively
    FOnRenderImageDataChanged& OnImageRenderingDataChanged();

    //Called when some data of this object, related with image rendering has been commited
    FOnRenderImageDataChanged& OnImageRenderingDataCommited();

    //Called if the RenderImageId, including any of the underlying ids has changed interactively
    FSimpleMulticastDelegate& OnRenderImageIdChanged();

    //Called if the RenderImageId, including any of the underlying ids has been commited
    FSimpleMulticastDelegate& OnRenderImageIdCommited();

public:
    virtual TArray<::ULIS::FEvent> RenderImage(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>  ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) = 0;

    /**
     * @brief Returns only this object's part of the RenderImageId, not taking into account the eventual underlying ids
     * 
     * @return const FOdysseyAnimationRenderImageId& 
     */
    virtual const FOdysseyAnimationRenderImageIdFragment& GetRenderImageIdFragment() const = 0;

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     * 
     * @return const FOdysseyAnimationRenderImageId& 
     */
    virtual const FOdysseyAnimationRenderImageId& GetRenderImageId() const = 0;
};