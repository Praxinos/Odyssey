// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyAbility.h"
#include "OdysseyImageRenderer.h"

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

    //Called before OnChanged() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    static FOnChanged& OnPreChanged();

    //Called when some data of this object, related with image rendering changed interactively
    static FOnChanged& OnChanged();

    //Called before OnCommited() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    static FOnChanged& OnPreCommited();

    //Called when some data of this object, related with image rendering has been commited
    static FOnChanged& OnCommited();

    //Called before OnCompositionChanged() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    static FOnCompositionChanged& OnPreCompositionChanged();

    //Called when the list of ImageRenderingAbility composing this ability changed interactively, including its ordering
    static FOnCompositionChanged& OnCompositionChanged();

    //Called before OnCompositionCommited() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    static FOnCompositionChanged& OnPreCompositionCommited();

    //Called when the list of ImageRenderingAbility composing this ability changed, including its ordering
    static FOnCompositionChanged& OnCompositionCommited();

public:
    static void Changed(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);
    static void Commited(const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);
    static void CompositionChanged(const FGuid& iId);
    static void CompositionCommited(const FGuid& iId);

public:
    IOdysseyAnimationImageRenderingAbility();

public:
    /**
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     * 
     * iThreadSafe, defines if the created renderer will be used in an other thread than the main thread.
     * In that case, the renderer will be made so that it always renders exactly the same image all the time
     * and you will have to recreate the renderer even if you're are just drawing on the layer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildRenderer(int iFrame, bool iThreadSafe) const = 0;

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