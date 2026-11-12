// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDElement.h"
#include "OdysseyRasterBlock.h"
#include "OdysseyRasterBlockMutator.h"

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
    void Add( const TArray<TArray<FVector2D>>& iPolygons );
    void Add( TSharedPtr<::ULIS::FBlock> iMaskBlock );
    void Substract( const TArray<FVector2D>& iPolygon );
    void Substract( const TArray<TArray<FVector2D>>& iPolygons );
    void Substract( TSharedPtr<::ULIS::FBlock> iMaskBlock );
    void Invert();
    void Clear();
    void Reset();

    bool IsEmpty() const;

    ::ULIS::FRectI GetMaskBoundingRect() const;
    TSharedPtr<FOdysseyRasterBlock> GetRasterBlock();
    TSharedPtr<::ULIS::FBlock> GetBlock();
    TSharedPtr<::ULIS::FBlock> GetFeatheredBlock();

    int GetFeatherRadius() const;
    void SetFeatherRadius( int iRadius );

    TSharedPtr<FOdysseyHUDElement> GetHUD();

    FSimpleMulticastDelegate& OnChanged();
    void RefreshHUD();
    void HideSelectionHUD();
    void ShowSelectionHUD();

private:
    ::ULIS::FRectI ComputeBoundingRect(const TArray<FVector2D>& iPoints) const;
    TArray<TArray<FVector2D>> BuildContours(const TArray<FIntEdge>& Edges);

    // Utils for feathered block
    bool GenerateFeatheredBlock();
    ::ULIS::FKernel MakeGaussianKernel(int iRadius, bool iHorizontal);

private:
    void OnBlockCommited(const TArray<::ULIS::FRectI>& iRects);

private:
    TSharedPtr<::ULIS::FBlock> mBlock;

    int mFeatherRadius; // The radius of the gaussian blur to apply to the edges of the selection
    TSharedPtr<::ULIS::FBlock> mFeatheredBlock; // The resulting block after the application of the gaussian blur to mBlock

    TSharedPtr<FOdysseyRasterBlock> mRasterBlock;
    FOdysseyRasterBlockMutator mRasterMutator;

    TSharedPtr<FOdysseyHUDElement> mHUD;
    bool mShowHUD;
    ::ULIS::FRectI mBoundingRect;
    FOdysseyHUDElement::FHUDCustomization mDottedSelectionCustomization;

    FSimpleMulticastDelegate mOnChanged;
};
