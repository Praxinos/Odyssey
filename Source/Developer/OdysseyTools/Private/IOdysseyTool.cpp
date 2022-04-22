// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyTool.h"


UOdysseyTool::UOdysseyTool(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void IOdysseyTool::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{

}

FReply IOdysseyTool::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    return FReply::Unhandled();
}

void IOdysseyTool::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{

}

bool IOdysseyTool::IsReadyToBeApplied() const
{
    return mIsReadyToBeApplied;
}
