// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDElement.h"

#include <ULIS>

#include "OdysseyPainterEditorRasterSelection.generated.h"


// Used to create the correct pixel snapping polygon represented by the selection
struct FIntEdge
{
    FIntPoint Start;
    FIntPoint End;

    // For hash map usage
    bool operator==(const FIntEdge& Other) const
    {
        return (Start == Other.Start && End == Other.End);
    }
};

FORCEINLINE uint32 GetTypeHash(const FIntEdge& Edge)
{
    return HashCombine(GetTypeHash(Edge.Start), GetTypeHash(Edge.End));
}

class FOdysseyRasterContent;
class UOdysseyTiledImage;

/**
 * The raster selection of the painter editor: a mask of the pixels the raster tools edit.
 *
 * The mask is held by a tiled image, in the alpha of its tiles, so that changes are undoable through Modify()
 * and unselected areas hold no tile. Tools read a CPU block derived from it, of grey floats where 1 is selected.
 */
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorRasterSelection
    : public UObject
{
    GENERATED_BODY()

public:
    // Construction / Destruction
    ~UOdysseyPainterEditorRasterSelection();
    UOdysseyPainterEditorRasterSelection();

    void Init(int iWidth, int iHeight);
    void Add( const TArray<FVector2D>& iPolygon );
    void Substract( const TArray<FVector2D>& iPolygon );
    void Invert();
    void Clear();
    void Reset();

    bool IsEmpty() const;

    ::ULIS::FRectI GetMaskBoundingRect() const;

    /** Returns the mask, of grey floats where 1 is selected; null before Init(). Must not be changed: use EditMask(). */
    TSharedPtr<::ULIS::FBlock> GetBlock();

    /**
     * Changes the whole mask, then writes it, undoable within a transaction, and notifies the change.
     * @param Edit Changes the mask. Pending ULIS operations must be finished when it returns.
     */
    void EditMask(TFunctionRef<void(::ULIS::FBlock& Mask)> Edit);

    /**
     * Copies the selected pixels of a content into a new block covering its canvas, in the format the owner expects:
     * their alpha is multiplied by the mask, the other pixels are transparent. Only the area of the selection is read.
     * @param iContent The content to copy from.
     * @return Null when the content cannot be read.
     */
    TSharedPtr<::ULIS::FBlock> CopySelectedPixels(FOdysseyRasterContent& iContent);

    TSharedPtr<FOdysseyHUDElement> GetHUD();

    FSimpleMulticastDelegate& OnChanged();
    void RefreshHUD();
    void HideSelectionHUD();
    void ShowSelectionHUD();

private:
    ::ULIS::FRectI ComputeBoundingRect(const TArray<FVector2D>& iPoints) const;
    TArray<TArray<FVector2D>> BuildContours(const TArray<FIntEdge>& Edges);

private:
    /** Changes an area of the mask, then writes it and notifies the change, see EditMask(). */
    void EditMaskArea(TFunctionRef<void(::ULIS::FBlock& Mask)> Edit, const ::ULIS::FRectI& Area);

    /** Reads the mask back from the tiles an undo or a redo restored. */
    void OnMaskUndone();

private:
    /** The mask, derived from the tiles; tools read it. */
    TSharedPtr<::ULIS::FBlock> mBlock;

    /** The mask, in the alpha of the tiles: the undoable storage of the selection. */
    UPROPERTY(Transient)
    TObjectPtr<UOdysseyTiledImage> MaskImage;

    TSharedPtr<FOdysseyHUDElement> mHUD;
    bool mShowHUD;
    ::ULIS::FRectI mBoundingRect;
    FOdysseyHUDElement::FHUDCustomization mDottedSelectionCustomization;

    FSimpleMulticastDelegate mOnChanged;
};
