// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "PixelFormat.h"
#include "Serialization/EditorBulkData.h"

#include "OdysseyTileManager.h"

#include "OdysseyTiledImage.generated.h"

UENUM(BlueprintType)
enum class EOdysseyTiledImageFormat : uint8
{
    RGBA8,
    RGBA16F,
    RGBA32F,
};

/**
 * A class containing raster data split in tiles.
 * Tiles are the positioned in space to provide
 * a technically infinite amount of Raster data
 */

UCLASS(BlueprintType)
class ODYSSEYIMAGING_API UOdysseyTiledImage
    : public UObject
{
    GENERATED_BODY()

public:
    virtual ~UOdysseyTiledImage();
    UOdysseyTiledImage();

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|TiledImage")
    void Initialize(int TileSize, EOdysseyTiledImageFormat Format);

    UFUNCTION(BlueprintPure, Category="Odyssey|TiledImage")
    EOdysseyTiledImageFormat GetFormat() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|TiledImage")
    EPixelFormat GetPixelFormat() const;

    UFUNCTION(BlueprintCallable, Category="Odyssey|TiledImage")
    void CopyFromTexture(UTexture* Texture, FIntRect Rect, FIntPoint Position);

    /**
     *  Renders the texture into a render target
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|TiledImage")
    void Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const;

public:
    virtual void Serialize(FArchive& Ar) override;

private:
    uint32 TileSize = 64;

    /**
     *
     */
    EOdysseyTiledImageFormat Format = EOdysseyTiledImageFormat::RGBA8;

    /** TODO:
     * Replace this map with a structure
     * optimized for binary searching
     *
     * struct FTileRegistryRedirector
     * {
     *      int Value; //can be X or Y
     *      uint32 index;
     * };
     *
     * struct FTileRegistry
     * {
     *      TArray<FTileRegistryRedirector> YRedirector; //Redirects Y positions to an index in XRedirector
     *      TArray<TArray<FTileRegistryRedirector>> XRedirector; //Redirects (Y*, X) positions to an index in Tiles;
     *      TArray<FTile> Tiles;
     * }
    */

    UPROPERTY(Transient)
    TMap<FIntPoint, FOdysseyTileId> Tiles;
};
