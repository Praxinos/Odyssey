// IDDN FR.001.250001.005.S.P.2019.000.00000
// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

class FOdysseyAnimationCellClipboard
{
public:
    static FOdysseyAnimationCellClipboard* Get();

private:
    FOdysseyAnimationCellClipboard();

public:
    void Copy(TSharedRef<FOdysseyAnimationCellsContainer> iCellContainer, const FInt32Range& iSelectedFrames);
    void Paste(UOdysseyAnimationLayer* iLayer, TSharedRef<FOdysseyAnimationCellsContainer> iCellContainer, int iFrame);
    TArray<TSharedPtr<FOdysseyAnimationCell>> GetCells() const;

private:
    struct FCellCopy
    {
        TSharedPtr<FOdysseyAnimationCell> mCell;
        int mLength;
    };

    TArray<FCellCopy> mCellCopies;
};
