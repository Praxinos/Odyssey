// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

class FUICommandList;

class IOdysseyEditorShortcuts
{
public:
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) = 0;
};

class ODYSSEYEDITOR_API FOdysseyEditorShortcuts
    : public IOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyEditorShortcuts() {};

    //Shortcuts
    virtual void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList) override;
    void Add(TSharedPtr<IOdysseyEditorShortcuts> iShortcuts);

private:
    TArray<TSharedPtr<IOdysseyEditorShortcuts>> mShortcuts;
};
