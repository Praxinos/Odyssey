// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "Engine/Texture.h"
#include "TextureResource.h"
#include "OdysseyLayerStack.h"

#include "OdysseyTexture.generated.h"


UCLASS(hidecategories=Object, MinimalAPI, BlueprintType)
class UOdysseyTexture : public UTexture
{
	GENERATED_UCLASS_BODY()

public:

	//~ Begin UObject Interface.
	virtual void Serialize(FArchive& Ar) override;
#if WITH_EDITOR
	virtual void PostLinkerChange() override;
	virtual void PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent) override;
#endif // WITH_EDITOR
	virtual void BeginDestroy() override;
	virtual bool IsReadyForAsyncPostLoad() const override;
	virtual void PostLoad() override;
	virtual void PreSave(const class ITargetPlatform* TargetPlatform) override;
	virtual void GetAssetRegistryTags(TArray<FAssetRegistryTag>& OutTags) const override;
	virtual FString GetDesc() override;
	//~ End UObject Interface.

	//~ Begin UTexture Interface.
	virtual float GetSurfaceWidth() const override { return 0.f; }
	virtual float GetSurfaceHeight() const override { return 0.f; }
	virtual FTextureResource* CreateResource() override;
	virtual EMaterialValueType GetMaterialType() const override;
	virtual void UpdateResource() override;
	virtual float GetAverageBrightness(bool bIgnoreTrueBlack, bool bUseGrayscale) override;
	virtual FTexturePlatformData** GetRunningPlatformData() final override { return NULL; }
#if WITH_EDITOR
    virtual TMap<FString,FTexturePlatformData*>* GetCookedPlatformData() override { return NULL; }
#endif
    //~ End UTexture Interface.

    
public:
	virtual uint32 CalcTextureMemorySizeEnum( ETextureMipCount Enum ) const override;
	virtual void WaitForStreaming() override;
	virtual void GetResourceSizeEx(FResourceSizeEx& CumulativeResourceSize) override;
	virtual void PostEditUndo() override;
    virtual bool IsCurrentlyVirtualTextured() const override;
    
public:
    UTexture2D* GetResultTexture2D();
    void SetResultTexture2D( UTexture2D* iTexture );
    
private:
    FOdysseyLayerStack* LayerStack;
    
    UPROPERTY(EditAnywhere)
    UTexture2D* ResultTexture;
};
