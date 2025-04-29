// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingType.h"

class ODYSSEYCORE_API FOdysseyRenderingChangedEvent
{
public:
    enum eEventType
    {
        kValueChange,
        kCompositionChange
    };

public:
    FOdysseyRenderingChangedEvent(eEventType iType, bool iIsInteractive, const FGuid& iId, const TArray<FIntRect>& iRects);

public:
    eEventType GetType() const;
    bool IsInteractive() const;
    const FGuid& GetId() const;
    const TArray<FIntRect>& GetRects() const;

private:
    eEventType mType;
    bool mIsInteractive;
    FGuid mId;
    TArray<FIntRect> mRects;
};

class ODYSSEYCORE_API FOdysseyRenderingAbility
{
public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChanged, const FOdysseyRenderingChangedEvent&);

    //Called before OnChanged() is called, so that some part of Odyssey can react prior to other parts (ex: animation Proxy invalidation)
    static FOnChanged& OnRenderingPreChangedDelegate();

    //Called when some data of this object, related with image rendering changed interactively
    static FOnChanged& OnRenderingChangedDelegate();

public:
    FOdysseyRenderingAbility();

public:
    void RenderingChanged(bool iIsInteractive = false); //Changes the whole rect
    void RenderingChanged(const TArray<FIntRect>& iRects, bool iIsInteractive = false);
    void RenderingCompositionChanged(bool iIsInteractive = false);

public:
    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex = 0) const;

public:
    /**
     * @brief Returns the full rect that can be rendered
     *
     * @return ::ULIS::FRect
     */
    virtual TArray<FIntRect> GetRenderingRects() const;

    /**
     * @brief Returns the image rendering id of this rendering ability, without underlying ids
     *
     * @return const FGuid&
     */
    virtual FGuid GetRenderingId() const;

private:
    FGuid mRenderingId;
};
