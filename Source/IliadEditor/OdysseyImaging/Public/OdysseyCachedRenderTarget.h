// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Engine/TextureRenderTarget2D.h"
#include "OdysseyInvalidTileMap.h"

#include "OdysseyCachedRenderTarget.generated.h"

UENUM(BlueprintType)
enum class EOdysseyCachedRenderTargetFormat : uint8
{
    RGBA8,
    RGBA16F,
    RGBA32F,
};

/**
 * Class allowing drawing on a Texture2D view Tiless
 * Also includes an efficient Undo system
 *
 * Usage:
 *
 * BeginDraw() -> Draw() -> CommitDraw() -> EndDraw()
 *
 */
UCLASS(BlueprintType)
class ODYSSEYIMAGING_API UOdysseyCachedRenderTarget
    : public UObject
{
    GENERATED_BODY()

public:
    virtual ~UOdysseyCachedRenderTarget();
    UOdysseyCachedRenderTarget();

public:
    enum class eCacheState : uint8
    {
        DDC, //Data is only present in the DDC, the Render Target and the FImage have been unloaded
        Image, //Only the FImage is loaded, the Render Target has been unloaded
        RenderTarget //The RenderTarget and the FImage are loaded
    };

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void Initialize(int Width, int Height, EOdysseyCachedRenderTargetFormat Format);

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetWidth() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetHeight() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    EOdysseyCachedRenderTargetFormat GetFormat() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    EPixelFormat GetPixelFormat() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetChannelsPerPixel() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetBytesPerChannel() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetBytesPerPixel() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetStride() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    int GetTotalBytes() const;

    /**
     * Starts the drawing process on this texture
     * Creates the needed Tiles
     * Don't forget to Stop the drawing process using EndDraw()
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void BeginDraw();

    /**
     * Draws pixels in SourceRect from Source in WetTiles at DestinationPosition
     * Can only be called after BeginDraw() has been called and can be called multiple times until EndDraw() is called.
     * Does not commit the result of the blend in Texture
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void Draw(UTexture* SourceTexture, FIntRect Rect, FIntPoint Position);

    /**
     * Cancels all the calls of Draw() since the last call of CommitDraw() or BeginDraw()
     * Clears WetTiles
     * Cancels Undo Recording
     * Can only be called after BeginDraw() has been called
     * You will still need to call EndDraw() to finish the drawing process
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void ResetDraw();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void CommitDraw(bool IsUndoable);

    /**
     * Stops the drawing process and commits what has been drawn into Texture
     * Blends WetTiles over DryTiles
     * End Undo Recording
     * Calls CommitTilesToTexture() and clears WetTiles
     * Can only be called after BeginDraw() has been called
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void EndDraw(bool IsUndoable);

    /**
     *  Returns wether the drawing process is active or not (see BeginDraw(), EndDraw() CancelDraw())
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|CachedRenderTarget")
    bool GetIsDrawing() const;

    /**
     *  Renders the texture into a render target
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const;

public:
    /**
     * Returns the State
     */
    eCacheState GetCacheState() const;

    /**
     * Initializes RenderTarget with the content of mImage
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void LoadRenderTarget() const;

    /**
     * Copy the content of the RenderTarget into the mImage
     * Release RenderTarget after the copy is done
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void UnloadRenderTarget() const;

    /**
     * Initializes mImage with what is saved on the DDC
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void LoadImage() const;

    /**
     * Copy the content of mImage into the DDC
     * Release mImage after the copy is done
     * Calls CacheRenderTargetToImage() if mImage is not up to date yet
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|CachedRenderTarget")
    void UnloadImage() const;

    void CopyRenderTargetToResetRT();
    void CopyResetRTToRenderTarget();

public:
    virtual void Serialize(FArchive& Ar) override;

private:
    /**
     *
    */
    uint32 Width = 1024;

    /**
     *
     */
    uint32 Height = 1024;

    /**
     *
     */
    EOdysseyCachedRenderTargetFormat Format = EOdysseyCachedRenderTargetFormat::RGBA8;

    /**
     * Is the drawing process active
     */
    bool IsDrawing = false;

    /**
     * Render Target  on which we draw
     * Only present between calls to BeginDraw() and EndDraw()
     * Updated on CommitDraw()
     */
    TStrongObjectPtr<UTextureRenderTarget2D> mResetRenderTarget;

    /**
     * Contains invalid tiles in RenderTarget compared to mResetRenderTarget
    */
    FOdysseyInvalidTileMap mResetTileMap;

    /**
     * Once mImage is created and filled, it will never be changed again
     * unless we create a new instance of mImage
     * This allows us to have mImage as a SharedPtr in order for the RenderTarget to copy mImage content
     * without having to copy mImage itself in a temporary image to ensure thread safety.
     */
    FGuid mId; //DDC Key

    /**
     * Render Target  on which we draw
     * Only present between calls to BeginDraw() and EndDraw()
     */
    mutable TStrongObjectPtr<UTextureRenderTarget2D> mRenderTarget;
    //mutable TSharedPtr<FImage> mImage;

    mutable TSharedFuture<FUniqueBuffer> mImageFuture;
    mutable eCacheState mCacheState = eCacheState::DDC;
    mutable bool mIsImageCacheInvalid = false;
    mutable bool mIsDDCCacheInvalid = false;
};
