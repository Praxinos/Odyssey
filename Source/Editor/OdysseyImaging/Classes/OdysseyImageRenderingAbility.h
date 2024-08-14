// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderer.h"
#include "OdysseyImageRenderingAbility.h"

class ODYSSEYIMAGING_API FOdysseyImageRenderingChangedEvent
{
public:
    enum eEventType
    {
        kValueChange,
        kCompositionChange
    };

public:
    FOdysseyImageRenderingChangedEvent(eEventType iType, bool iIsInteractive, const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);

public:
    eEventType GetType() const;
    bool IsInteractive() const;
    const FGuid& GetId() const;
    const TArray<::ULIS::FRectI>& GetRects() const;

private:
    eEventType mType;
    bool mIsInteractive;
    FGuid mId;
    TArray<::ULIS::FRectI> mRects;
};

class ODYSSEYIMAGING_API FOdysseyImageRenderingAbility
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChanged, const FOdysseyImageRenderingChangedEvent&);

    //Called before OnChanged() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    static FOnChanged& OnImageRenderingPreChangedDelegate();

    //Called when some data of this object, related with image rendering changed interactively
    static FOnChanged& OnImageRenderingChangedDelegate();

public:
    FOdysseyImageRenderingAbility();

public:
    void ImageRenderingChanged(bool iIsInteractive = false); //Changes the whole rect
    void ImageRenderingChanged(const TArray<::ULIS::FRectI>& iRects, bool iIsInteractive = false);
    void ImageRenderingCompositionChanged(bool iIsInteractive = false);

public:
    /**
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0, FImageRendererFilter iFilter = FImageRendererFilter()) const;

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex = 0) const;

public:
    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const;

    /**
     * @brief Returns the image rendering id of this rendering ability, without underlying ids
     *
     * @return const FGuid&
     */
    virtual FGuid GetImageRenderingId() const;

private:
	FGuid mImageRenderingId;
};