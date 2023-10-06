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
    FOdysseyAnimationCellClipboardData(UOdysseyAnimationLayer* iLayer, const FInt32Range& iSelectedFrames);

public:
    static const FGuid& StaticId();

public:
    bool CanPaste(UOdysseyAnimationLayer* iLayer) const;
    void Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const;

private:
    void Copy(TSharedPtr<FOdysseyAnimationCellsContainer> iCellContainer, const FInt32Range& iSelectedFrames);

private:
    struct FCellCopy
    {
        TSharedPtr<FOdysseyAnimationCell> mCell;
        int mLength;
    };

    TArray<FCellCopy> mCellCopies;
};
