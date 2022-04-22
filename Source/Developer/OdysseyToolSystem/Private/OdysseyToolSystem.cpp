// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyToolSystem.h"
#include "OdysseyToolLine.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyToolSystem::~FOdysseyToolSystem()
{
    if( mSelectedTool )
        delete mSelectedTool;
}

FOdysseyToolSystem::FOdysseyToolSystem():
    mSelectedTool(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

IOdysseyTool* FOdysseyToolSystem::GetSelectedTool() const
{
    return mSelectedTool;
}

void FOdysseyToolSystem::SetSelectedTool(IOdysseyTool* iTool)
{
    if( mSelectedTool )
        delete mSelectedTool;

    mSelectedTool = iTool;
}