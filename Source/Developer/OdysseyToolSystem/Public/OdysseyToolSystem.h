// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "IOdysseyTool.h"

class ODYSSEYTOOLSYSTEM_API FOdysseyToolSystem 
{
public:
    // Construction / Destruction
    ~FOdysseyToolSystem();
    FOdysseyToolSystem();

public:
    // Getters/Setters
    IOdysseyTool* GetSelectedTool() const;
    void SetSelectedTool( IOdysseyTool* iTool );


private:
    IOdysseyTool*                     mSelectedTool;

};
