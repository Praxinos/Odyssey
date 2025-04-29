// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyRenderingType.h"

#include "OdysseyRenderingAbility.generated.h"

class ODYSSEYRENDERING_API FOdysseyRenderingChangedEvent
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

UINTERFACE(Blueprintable)
class ODYSSEYRENDERING_API UOdysseyRenderingAbility : public UInterface
{
    GENERATED_BODY()
};

class ODYSSEYRENDERING_API IOdysseyRenderingAbility
{
    GENERATED_BODY()

public:
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnChanged, const FOdysseyRenderingChangedEvent&);

    //Called before OnChanged() is called, so that some part of Odyssey can react prior to other parts (ex: animation Proxy invalidation)
    static FOnChanged& OnRenderingPreChangedDelegate();

    //Called when some data of this object, related with image rendering changed interactively
    static FOnChanged& OnRenderingChangedDelegate();

public:
    IOdysseyRenderingAbility();

public:
    void RenderingChanged(bool iIsInteractive = false); //Changes the whole rect
    void RenderingChanged(const TArray<FIntRect>& iRects, bool iIsInteractive = false);
    void RenderingCompositionChanged(bool iIsInteractive = false);
    /**
     * @brief Returns the full Render Image Id, eventually composed of underlying ids
     *
     * @return const FGuid&
     */
    virtual TArray<FGuid> GetRenderingComposition(EOdysseyRenderingType iRenderType, int iFrameIndex = 0) const;

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

    virtual FInt32Range GetFrameRange() const = 0;

private:
    FGuid mRenderingId;
};
