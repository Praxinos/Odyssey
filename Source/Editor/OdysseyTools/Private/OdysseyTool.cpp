// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyTool.h"

UOdysseyTool::UOdysseyTool()
{

}

void
UOdysseyTool::Activate()
{

}

void
UOdysseyTool::Inactivate()
{

}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------- Mouse events

void
UOdysseyTool::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{

}

void
UOdysseyTool::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{

}

void
UOdysseyTool::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyTool::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyTool::OnKeyDown(const FKey& iKey)
{

}

void
UOdysseyTool::OnKeyUp(const FKey& iKey)
{

}

void
UOdysseyTool::Tick(float iDeltaTime)
{

}

void
UOdysseyTool::Flush()
{

}

void
UOdysseyTool::Commit()
{

}

/* void UOdysseyTool::MouseMove(FViewport* iViewport, int32 iX, int32 iY)
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
} */

void
UOdysseyTool::SetTransform(const FTransform2D& iTransform)
{
    mTransform = iTransform;
}

void
UOdysseyTool::BindShortcuts(FBaseToolkit* iToolkit)
{

}

void
UOdysseyTool::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{

}

TSharedPtr<SWidget>
UOdysseyTool::GetWidget()
{
    return SNullWidget::NullWidget;
}
