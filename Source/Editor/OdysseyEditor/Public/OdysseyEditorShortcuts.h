// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

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