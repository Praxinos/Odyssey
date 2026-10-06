// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Attribute.h"

#include "OdysseyLayerCellImageStagger.h"

class UOdysseyAnimation;
class FUICommandList;

class ODYSSEYPAINTEREDITOR_API FOdysseyAnimationTimelineCellsShortcuts
{
public:
    FOdysseyAnimationTimelineCellsShortcuts(
        const TAttribute<UOdysseyAnimation*>& iAnimation,
        const TAttribute<int>& iCurrentFrame
    );

public:
    //Shortcuts
    void MapActionsToCommandList(TSharedRef<FUICommandList> iCommandList);

public:
    //Common Shortcuts
    void Action_Copy();
    void Action_Cut();
    void Action_Paste();
    void Action_SelectAll();
    void Action_Delete();
    void Action_CreateStaggerCell( EOdysseyLayerCellImageStaggerBehaviour iBehavior );
    void Action_AddCellsBefore();
    void Action_AddCellsAfter();
private:
    void Action_AddCellsBeforeOrAfter( int32 iNumberOfCellsToAddBeforeOrAfter);
public:
    void Action_IncreaseOneCellExposure();
    void Action_DecreaseOneCellExposure();
    void Action_IncreaseNCellExposure();
    void Action_DecreaseNCellExposure();
private:
    void Action_IncreaseOrDecreaseCellExposure( int32 iNumberOfExposuresToAddOrRemove );
public:
    void Action_SetCellExposure();
    void Action_ReverseSelectedCells();

    bool CanAction_Copy();
    bool CanAction_Cut();
    bool CanAction_Paste();
    bool CanAction_SelectAll();
    bool CanAction_Delete();
    bool CanAction_CreateStaggerCell();
    bool CanAction_AddCellsBefore();
    bool CanAction_AddCellsAfter();
    bool CanAction_ManageCellExposure();
    bool CanAction_ReverseSelectedCells();

private:
    TAttribute<UOdysseyAnimation*> mAnimation;
    TAttribute<int> mCurrentFrame;
};
