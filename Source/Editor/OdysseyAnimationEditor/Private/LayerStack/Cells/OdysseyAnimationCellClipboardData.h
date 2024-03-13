// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyClipboard.h"

class FOdysseyAnimationCellClipboardData
    : public IOdysseyClipboardData
{
public:
    virtual ~FOdysseyAnimationCellClipboardData() {};
    FOdysseyAnimationCellClipboardData();
    FOdysseyAnimationCellClipboardData(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iCells);

public:
    static const FGuid& StaticId();

public:
    bool CanPaste(UOdysseyAnimationLayer* iLayer) const;
    void Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const;
    void Move(UOdysseyAnimationLayer* iLayer, int iFrame) const;
    int GetCellCount() const;

private:
    void Copy(const TArray<TSharedPtr<FOdysseyAnimationCell>>& iCells);

private:
    UOdysseyAnimationLayer* mLayer;
    struct FCellCopy
    {
        TSharedPtr<FOdysseyAnimationCell> mCell;
        int mLength;
    };
    TArray<FCellCopy> mCellCopies;
};
