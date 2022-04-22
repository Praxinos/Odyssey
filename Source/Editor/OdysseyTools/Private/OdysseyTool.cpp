// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTool.h"

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void UOdysseyTool::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
{

}

FReply UOdysseyTool::InputKey(FViewport* iViewport, int32 iControllerId, FKey iKey, EInputEvent iEvent, float iAmountDepressed, bool iGamepad, FReply& ioReply)
{
    return FReply::Unhandled();
}

void UOdysseyTool::CapturedMouseMove(FViewport* iViewport, int32 iX, int32 iY)
{

}

bool UOdysseyTool::IsReadyToBeApplied() const
{
    return mIsReadyToBeApplied;
}
