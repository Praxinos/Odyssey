// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleManager.h"
#include "Misc/Change.h"

class FOdysseyVectorCell;

class FOdysseyPaletteUndo : public FCommandChange
{
public:
    FOdysseyPaletteUndo( const TArray<FOdysseyVectorCell*>& iCells );

    void Refresh();

    /** Called when redoing */
    virtual void Apply(UObject* iIgnored) override;

    /** called when undoing */
    virtual void Revert(UObject* iIgnored) override;

    virtual FString ToString() const override;

private:
    TArray<FOdysseyVectorCell*> mCells;
};
