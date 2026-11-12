// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyTiledImage.h"

#include "OdysseyPaintableTexture2D.generated.h"

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
class ODYSSEYRENDERING_API UOdysseyPaintableTexture2D
    : public UObject
{
    GENERATED_BODY()

public:
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void Initialize(int Width, int Height, ETextureRenderTargetFormat Format);

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    int GetWidth() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    int GetHeight() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    ETextureRenderTargetFormat GetFormat() const;

    /**
     * Starts the drawing process on this texture
     * Creates the needed Tiles
     * Don't forget to Stop the drawing process using EndDraw()
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void BeginDraw();

    /**
     * Draws pixels in SourceRect from Source in WetTiles at DestinationPosition
     * Can only be called after BeginDraw() has been called and can be called multiple times until EndDraw() is called.
     * Does not commit the result of the blend in Texture
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void Draw(UTexture* SourceTexture, FIntRect Rect, FIntPoint Position);

    /**
     * Cancels all the calls of Draw() since the last call of CommitDraw() or BeginDraw()
     * Clears WetTiles
     * Cancels Undo Recording
     * Can only be called after BeginDraw() has been called
     * You will still need to call EndDraw() to finish the drawing process
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void ResetDraw();

    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void CommitDraw(bool IsUndoable);

    /**
     * Stops the drawing process and commits what has been drawn into Texture
     * Blends WetTiles over DryTiles
     * End Undo Recording
     * Calls CommitTilesToTexture() and clears WetTiles
     * Can only be called after BeginDraw() has been called
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void EndDraw(bool IsUndoable);

    /**
     *  Returns wether the drawing process is active or not (see BeginDraw(), EndDraw() CancelDraw())
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    bool GetIsDrawing() const;

    /**
     *  Renders the texture into a render target
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void Render(UTextureRenderTarget2D* Destination, FIntRect Rect, FIntPoint Position) const;

protected:
    /**
     * Allows to commit what has been drawn into mImage before saving
     * Calls CommitRenderTargetToImage
    */
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;
private:
    /** Load the Render Target from mImage or DDC*/
    void LoadRenderTarget();

    /**
     * Commits what has been drawn into mImage
     */
    void CommitRenderTargetToImage();
    void CopyRenderTargetToUndoRT();
    void CopyUndoRTToRenderTarget();
    void ExtractTile(const FImage& InImage, FImage& OutImage, const FIntRect& Rect) const;

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
    ETextureRenderTargetFormat Format = RTF_RGBA8;

    /**
     * Is the drawing process active
     */
    bool IsDrawing = false;

    /**
     * Render Target  on which we draw
     * Only present between calls to BeginDraw() and EndDraw()
     */
    TStrongObjectPtr<UTextureRenderTarget2D> mRenderTarget;

    /**
     * Render Target  on which we draw
     * Only present between calls to BeginDraw() and EndDraw()
     * Updated on CommitDraw()
     */
    TStrongObjectPtr<UTextureRenderTarget2D> mUndoRenderTarget;

    /**
     * Contains invalid tiles in RenderTarget compared to UndoRenderTarget
    */
    FOdysseyInvalidTileMap mUndoTileMap;

    struct FTileData
    {
        TSharedPtr<FImage> mImage;
    }
    struct FTile
    {
        FIntRect mRect;

        //mData can be nullptr, which means the tile is fully transparent
        TSharedPtr<FTileData> mData;
    };

    /** Stores tiles outside of the GPU (on RAM) */
    TArray<TArray<FTile>> mTiles;
};
