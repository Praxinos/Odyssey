// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Templates/SharedPointer.h"
#include "UObject/ObjectMacros.h"
#include "UObject/ScriptMacros.h"
#include "OdysseyTextureLayerStack.h"
#include "Engine/AssetUserData.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "UObject/ObjectSaveContext.h"
#include "Palette/OdysseyPalette.h"
#include "UObject/SoftObjectPath.h"
#include "OdysseyPaletteReferencer.h"

#include "OdysseyTextureLayerStackUserData.generated.h"

UCLASS(BlueprintType)
class ODYSSEYTEXTURE_API UOdysseyTextureLayerStackUserData
    : public UAssetUserData
    , public IOdysseyPaletteReferencer
{
    GENERATED_BODY()

public:
    void InitWithEmptyLayerStack();
    void InitWithDefaultLayerStack();
    void InitWithDuplicateLayerStack(UOdysseyTextureLayerStack* iLayerStack);
    void InitWithEmptyVectorLayer();
    void InitWithEmptyRasterLayer();


    /**
     * @brief Get the Layer Stack object
     * If there is no layerStack, creates it from the texture, with a single raster layer
     *
     * @return UOdysseyTextureLayerStack*
     */
    UOdysseyTextureLayerStack* GetLayerStack();
    UTexture2D* GetTexture();

private:
#if WITH_EDITOR
    virtual void OnRefreshReferencedPalette(UOdysseyPalette* iPalette) override;
#endif

public:
    UPROPERTY(BlueprintReadOnly, Category="Odyssey|LayerStack")
    TObjectPtr<UOdysseyTextureLayerStack> LayerStack;

    UPROPERTY()
    TArray<UOdysseyPaletteSet*> Palettes;

    // We store the path to the Tool Collections Assets, so that we don't have to include the painterEditorModule (where the tools and tool collections are) in the build.cs
    UPROPERTY()
    TArray<FSoftObjectPath> ToolCollections;
};
