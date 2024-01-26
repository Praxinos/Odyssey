// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorShortcuts.h"

class FUICommandList;

class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationGlobalShortcuts
    : public FOdysseyEditorShortcuts
{
public:
    virtual ~FOdysseyAnimationGlobalShortcuts() {};
    FOdysseyAnimationGlobalShortcuts(TSharedPtr<FOdysseyAnimationEditorExtension> iExtension);
};