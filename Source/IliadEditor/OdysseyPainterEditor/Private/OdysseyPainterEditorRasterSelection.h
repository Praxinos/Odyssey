// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "OdysseyHUDElement.h"

#include <ULIS>

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

class FOdysseyPainterEditorRasterSelection
{
public:
    // Construction / Destruction
    ~FOdysseyPainterEditorRasterSelection();
    FOdysseyPainterEditorRasterSelection();

    void Init(int iWidth, int iHeight);
    void Add( const TArray<FVector2D>& iPolygon );
    void Substract( const TArray<FVector2D>& iPolygon );
    void Invert();
    void Clear();
    void Reset();

    bool IsEmpty() const;

    ::ULIS::FRectI GetMaskBoundingRect() const;
    TSharedPtr<::ULIS::FBlock> GetBlock();
    TSharedPtr<FOdysseyHUDElement> GetHUD();

    FSimpleMulticastDelegate& OnChanged();
    void RefreshHUD();

private:
    ::ULIS::FRectI ComputeBoundingRect(const TArray<FVector2D>& iPoints) const;
    TArray<TArray<FVector2D>> BuildContours(const TArray<FIntEdge>& Edges);

private:
    TSharedPtr<::ULIS::FBlock> mBlock;
    TSharedPtr<FOdysseyHUDElement> mHUD;
    ::ULIS::FRectI mBoundingRect;
    FOdysseyHUDElement::FHUDCustomization mDottedSelectionCustomization;

    FSimpleMulticastDelegate mOnChanged;
};
