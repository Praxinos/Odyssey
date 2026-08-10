// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlendingMode.h"
#include "OdysseyTiledRenderTarget.h"

#include "OdysseyPaintableTexture2D.generated.h"

/**
 * Class allowing drawing on a Texture2D view Tiless
 * Also includes an efficient Undo system
 *
 * Usage:
 *
 * BeginDraw() -> Draw() -> Reset -> EndDraw()
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

    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void SetBlendParamters( EOdysseyBlendingMode BlendMode, EOdysseyAlphaMode AlphaMode, float Opacity, bool PreserveAlpha );

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    int GetWidth() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    int GetHeight() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    ETextureRenderTargetFormat GetFormat() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    EOdysseyBlendingMode GetBlendMode() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    EOdysseyAlphaMode GetAlphaMode() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    float GetOpacity() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    bool GetPreserveAlpha() const;

    /**
     * Starts the drawing process on this texture
     * Creates the needed Tiless
     * Don't forget to Stop the drawing process using EndDraw()
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void BeginDraw();

    /**
     * Starts to register an undo
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void BeginUndoRecording();

    /**
     * Stops to register an undo
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void EndUndoRecording();

    /**
     * Cancel registering an undo
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void CancelUndoRecording();

    /**
     * Draws pixels in SourceRect from Source in WetTiles at DestinationPosition
     * Can only be called after BeginDraw() has been called and can be called multiple times until EndDraw() is called.
     * Does not commit the result of the blend in Texture
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void Draw(UTexture* SourceTexture, FIntRect SourceRect, FIntPoint DestinationPosition);

    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void Dry();

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
    void CommitDraw();

    /**
     * Stops the drawing process and commits what has been drawn into Texture
     * Blends WetTiles over DryTiles
     * End Undo Recording
     * Calls CommitTilesToTexture() and clears WetTiles
     * Can only be called after BeginDraw() has been called
     */
    UFUNCTION(BlueprintCallable, Category="Odyssey|Rendering|PaintableTexture2D")
    void EndDraw();

    /**
     *  Returns wether the drawing process is active or not (see BeginDraw(), EndDraw() CancelDraw())
     */
    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    bool GetIsDrawing() const;

    UFUNCTION(BlueprintPure, Category="Odyssey|Rendering|PaintableTexture2D")
    UTexture2D* GetTexture() const;

protected:
    /**
     * Allows to commit what has been drawn as a Source of this Texture before saving
     * Calls CommitToSource()
     */
    virtual void PreSave(FObjectPreSaveContext SaveContext) override;

private:
    /**
     * commit what has been drawn as a Source of this Texture
     */
    void CommitToSource();

    void UpdateTexture();

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
     *
     */
    EOdysseyBlendingMode BlendMode = EOdysseyBlendingMode::kNormal;

    /**
     *
     */
    EOdysseyAlphaMode AlphaMode = EOdysseyAlphaMode::kNormal;

    /**
     *
     */
    float Opacity = 1.0f;

    /**
     *
     */
    bool PreserveAlpha = false;

    /**
     * The final texture, is updated in realtime
     */
    UPROPERTY()
    TObjectPtr<UTexture2D> Texture;

    FOdysseyTiledRenderTarget WetTiles;
    FOdysseyTiledRenderTarget DryTiles;
    FOdysseyTiledRenderTarget OriginalTiles;

    FOdysseyTiledRenderTarget CancellableWetTiles;
    FOdysseyTiledRenderTarget CancellableDryTiles;
};
