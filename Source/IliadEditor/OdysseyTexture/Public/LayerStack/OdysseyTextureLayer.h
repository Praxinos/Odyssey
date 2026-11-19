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
    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UTexture2D* GetTexture() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Layer")
    UTexture2D* GetRenderTexture() const;

public:
    virtual void PostLoad() override;
    virtual void PostDuplicate( EDuplicateMode::Type iDuplicateMode ) override;

public:
    virtual bool BuildsCompositeContent() const override { return bCanHaveChildren; }
    virtual void InitTexture();
    virtual bool BuildRenderPipelineInternal( IOdysseyTextureRenderingAbility::FRenderFunction& oRenderFunction, const TArray<const IOdysseyTextureRenderingAbility*>& iParents, const FOdysseyRenderContext* RenderContext ) const override;

public:
#if WITH_EDITOR
    /* UFUNCTION(BlueprintCallable, Category="Odyssey|Layer")
    UTexture2D* ExportAsTexture(FString iAssetName, FString iPath ); */

    FSimpleMulticastDelegate& OnThumbnailChanged();
    FSimpleMulticastDelegate& OnThumbnailDirtied();
#endif

protected:
    /** Returns the source format of the layer texture for a texture of the given source format. */
    static ETextureSourceFormat GetLayerTextureFormat(ETextureSourceFormat TextureFormat);

    /** Returns the render texture without creating it. */
    UTexture2D* GetExistingRenderTexture() const;

    /** Drops the render texture, which is then neither rendered nor saved. */
    void DiscardRenderTexture();

private:
    void UpdateTextureSize();

private:
    UPROPERTY(NonTransactional)
    mutable TObjectPtr<UTexture2D> Texture; //mutable is temporary, will be removed when layers will be 100% GPU based and there's no more dependency on ULIS

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
