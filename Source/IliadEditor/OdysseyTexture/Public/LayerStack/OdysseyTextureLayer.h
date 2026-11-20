// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "Engine/TextureDefines.h"

#include "OdysseyLayer.h"

#include "OdysseyMediaProvider.h"

#include "OdysseyTextureLayer.generated.h"

UCLASS(Abstract, HideDropdown, BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayer
    : public UOdysseyLayer
{
    GENERATED_BODY()

public:
    /** Returns the texture whose layer stack holds this layer. */
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UTexture2D* GetTexture() const;

public:
    virtual bool BuildsCompositeContent() const override { return bCanHaveChildren; }

    /** Applies a change of the color settings of the texture or of the layer stack, e.g. its sRGB setting. */
    virtual void ColorSettingsChanged();

public:
#if WITH_EDITOR
    FSimpleMulticastDelegate& OnThumbnailChanged();
    FSimpleMulticastDelegate& OnThumbnailDirtied();
#endif

protected:
    /** Encodes the linear colors of the layer when its stack blends sRGB encoded colors. */
    virtual EOdysseyColorConversion GetContentColorConversion(const FOdysseyRenderContext* RenderContext) const override;

    /** Returns the source format of the pixels of a layer for a texture of the given source format. */
    static ETextureSourceFormat GetLayerTextureFormat(ETextureSourceFormat TextureFormat);

private:
#if WITH_EDITORONLY_DATA
    UPROPERTY(NonTransactional)
    bool ThumbnailIsDirty = false;
#endif

private:
#if WITH_EDITOR
    FSimpleMulticastDelegate mOnThumbnailChanged;
    FSimpleMulticastDelegate mOnThumbnailDirtied;
#endif
};
