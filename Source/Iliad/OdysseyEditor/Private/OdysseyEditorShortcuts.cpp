// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditorShortcuts.h"

void
FOdysseyEditorShortcuts::MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList)
{
    for (TSharedPtr<IOdysseyEditorShortcuts> shortcuts : mShortcuts)
    {
        shortcuts->MapActionsToCommandList(iCommandList);
    }
}

void
FOdysseyEditorShortcuts::Add(TSharedPtr<IOdysseyEditorShortcuts> iShortcuts)
{
    mShortcuts.Add(iShortcuts);
}
