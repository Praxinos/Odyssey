// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyClipboard.h"

class UOdysseyAnimationCell;
class UOdysseyAnimationLayer;

class FOdysseyAnimationCellClipboardData
    : public IOdysseyClipboardData
    , public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    virtual ~FOdysseyAnimationCellClipboardData() {};
    FOdysseyAnimationCellClipboardData();
    FOdysseyAnimationCellClipboardData(const TArray<UOdysseyAnimationCell*>& iCells);

public:
    static const FGuid& StaticId();

public:
    bool CanPaste(UOdysseyAnimationLayer* iLayer) const;
    void Paste(UOdysseyAnimationLayer* iLayer, int iFrame) const;
    void Move(UOdysseyAnimationLayer* iLayer, int iFrame) const;
    int GetCellCount() const;

public:
    // FGCObject implementation
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

private:
    void Copy(const TArray<UOdysseyAnimationCell*>& iCells);

private:
    struct FCellCopy
    {
        UOdysseyAnimationCell* mCell;
        int mExposure;
    };
    TArray<FCellCopy> mCellCopies;
};
