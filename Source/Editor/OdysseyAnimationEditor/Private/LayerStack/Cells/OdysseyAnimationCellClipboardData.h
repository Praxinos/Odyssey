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
    void Move(UOdysseyAnimationLayer* iLayer, int iFrame) const;
    int GetCellCount() const;
    UOdysseyAnimationLayer* GetLayer() const;
    const FInt32Range& GetSelectedFrames() const;

private:
    void Copy();
    void DeleteSelectedFrames() const; 

private:
    UOdysseyAnimationLayer* mLayer;
    struct FCellCopy
    {
        TSharedPtr<FOdysseyAnimationCell> mCell;
        int mLength;
    };

    TArray<FCellCopy> mCellCopies;
    FInt32Range mSelectedFrames;
};
