// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"

/**
 * @brief Represents the ability to render an image in a given block
 * Each instance of FOdysseyAnimationImageRenderingAbility has a unique id (a single Guid), so that it can be identified.
 * Each instance of FOdysseyAnimationImageRenderingAbility can be composed of other instances of FOdysseyAnimationImageRenderingAbility
 * in which case, GetImageRenderingId() also returns the underlying instances ids.
 * This allows tracking of data changes in each FOdysseyAnimationImageRenderingAbility
 * without overwelming complexity of bubbling events.
 */

class ODYSSEYANIMATION_API IOdysseyAnimationImageRenderingAbility
    : public IOdysseyAbility
{
public:
    static const FGuid& Id()
    {
        static FGuid Id(0x8b511cae, 0x20a24b19, 0x975d2347, 0x58d024de);
        return Id;
    }

public:
    DECLARE_MULTICAST_DELEGATE_TwoParams(FOnChanged, const FGuid&, const TArray<::ULIS::FRectI>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnCompositionChanged, const FGuid&);

    //Called when some data of this object, related with image rendering changed interactively
    static FOnChanged& OnChanged();

    //Called when some data of this object, related with image rendering has been commited
    static FOnChanged& OnCommited();

    //Called when the list of ImageRenderingAbility composing this ability changed interactively, including its ordering
    static FOnCompositionChanged& OnCompositionChanged();

    //Called when the list of ImageRenderingAbility composing this ability changed, including its ordering
    static FOnCompositionChanged& OnCompositionCommited();

public:
    IOdysseyAnimationImageRenderingAbility();

public:
    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetRects(int iFrame) const = 0;

    /**
     * @brief Renders over (by blending for example) the given block
     * By default does the same thing as RenderInBlock
     *
     * @param ioBlock
     * @param iFrame
     * @param iRect
     * @param iPos
     * @param iWaitList
     * @return TArray<::ULIS::FEvent>
     */
    virtual TArray<::ULIS::FEvent> RenderOverBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList);

    /**
     * @brief Renders in (without blending for example) the given block
     *
     * @param ioBlock
     * @param iFrame
     * @param iRect
     * @param iPos
     * @param iWaitList
     * @return TArray<::ULIS::FEvent>
     */
    virtual TArray<::ULIS::FEvent> RenderInBlock(TSharedPtr<::ULIS::FBlock> ioBlock, int iFrame, const ::ULIS::FRectI& iRect, const ::ULIS::FVec2I& iPos, const TArray<::ULIS::FEvent>& iWaitList) = 0;

    /**
     * @brief Renders a new block
     *
     * @param iFrame
     * @param iRect
     * @param oEvents
     * @return TSharedPtr<::ULIS::FBlock>
     */
    virtual TSharedPtr<::ULIS::FBlock> RenderInNewBlock(int iFrame, ::ULIS::eFormat iFormat, const ::ULIS::FRectI& iRect, TArray<::ULIS::FEvent>& oEvents);

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetComposition(int iFrameIndex) const = 0;

    /**
     * @brief Returns the image rendering id of this rendering ability, without underlying ids
     *
     * @return const FGuid&
     */
    virtual const FGuid& GetId() const;

    /**
     * @brief Preloads in memory everything needed to make RenderImage() as fast as possible
     *
     * @param iFrame
     */
    virtual TSharedPtr<IOdysseyHandle> Preload(int iFrame);

public:
    FGuid mId; //TODO: Move to somewhere it can be serialized
};