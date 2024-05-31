// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <ULIS>

class FOdysseyHUDElement;
class FOdysseyHUDLine;

class FOdysseyPainterEditorRasterSelection 
{
public:
    // Construction / Destruction
    ~FOdysseyPainterEditorRasterSelection();
    FOdysseyPainterEditorRasterSelection();

    void Init(int iWidth, int iHeight);
    void Add( const TArray<FVector2D>& iPolygon );
    void Substract( const TArray<FVector2D>& iPolygon );
    void Clear();
    void Reset();

    bool IsEmpty() const;

    ::ULIS::FRectI GetMaskBoundingRect() const;
    TSharedPtr<::ULIS::FBlock> GetBlock();
    TSharedPtr<FOdysseyHUDElement> GetHUD();

    FSimpleMulticastDelegate& OnChanged();
    void RefreshHUD();

private:
    ::ULIS::FRectI ComputeBoundingRect(const TArray<FVector2D>& iPoints ) const;

private:
    TSharedPtr<::ULIS::FBlock> mBlock;
    TSharedPtr<FOdysseyHUDElement> mHUD;
    ::ULIS::FRectI mBoundingRect;

    FSimpleMulticastDelegate mOnChanged;
};
