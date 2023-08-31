// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "OdysseyImageRenderer.h"

#include <ULIS>

class ODYSSEYIMAGING_API FOdysseyImageRenderingChangedEvent
{
public:
    enum eEventType
    {
        kValueChange,
        kCompositionChange
    };

public:
    FOdysseyImageRenderingChangedEvent(eEventType iType, bool iIsCommit, const FGuid& iId, const TArray<::ULIS::FRectI>& iRects);

public:
    eEventType GetType() const;
    bool IsCommit() const;
    const FGuid& GetId() const;
    const TArray<::ULIS::FRectI>& GetRects() const;

private:
    eEventType mType;   
    bool mIsCommit;
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

    //Called before OnChanged() is called, so that some part of ILIAD can react prior to other parts (ex: animation Proxy invalidation)
    //FOnChanged& OnImageRenderingPreChanged();

    //Called when some data of this object, related with image rendering changed interactively
    //FOnChanged& OnImageRenderingChanged();

public:
    FOdysseyImageRenderingAbility();

public:
    void ImageRenderingChanged(); //Changes the whole rect
    void ImageRenderingChanged(const TArray<::ULIS::FRectI>& iRects);
    void ImageRenderingCommited(); //Changes the whole rect
    void ImageRenderingCommited(const TArray<::ULIS::FRectI>& iRects);
    void ImageRenderingCompositionChanged();
    void ImageRenderingCompositionCommited();

public:
    /**
     * @brief Called when a child ImageRendering has changed
     * Event if the child is not a direct child
     * 
     * @param iDependencyId 
     * @param iEvent 
     */
    //virtual void OnImageRenderingDependencyChanged(const FGuid& iDependencyId, const FOdysseyImageRenderingChangedEvent& iEvent);

    /**
     * @brief Creates a renderer able to render an image at the specified frame
     * This renderer is made to always render the same rendering composition
     * For example : if you delete a layer, you should create a new renderer
     * but if you are just drawing on the layer, you can reuse the renderer
     */
    virtual TSharedPtr<IOdysseyImageRenderer> BuildImageRenderer(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const;

    /**
     * @brief Returns the full rect that can be rendered
     * 
     * @return ::ULIS::FRect 
     */
    virtual TArray<::ULIS::FRectI> GetImageRenderingRects() const;

    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetImageRenderingComposition(IOdysseyImageRenderer::eRenderType iRenderType, int iFrameIndex = 0) const;

    /**
     * @brief Returns the image rendering id of this rendering ability, without underlying ids
     *
     * @return const FGuid&
     */
    virtual FGuid GetImageRenderingId() const;

    /**
     * @brief Preloads in memory everything needed to make RenderImage() as fast as possible
     *
     * @param iFrame
     */
    virtual TSharedPtr<IOdysseyHandle> PreloadImageRendering(IOdysseyImageRenderer::eRenderType iRenderType, int iFrame = 0) const;

    virtual ::ULIS::eBlendMode GetImageRenderingBlendMode() const;

    virtual float GetImageRenderingOpacity() const;

private:
	FGuid mImageRenderingId;
};