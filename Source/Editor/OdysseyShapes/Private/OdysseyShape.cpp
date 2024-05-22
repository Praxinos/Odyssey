// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyShape.h"
#include "HUDViewportElement/OdysseyHUDElement.h"


UOdysseyShape::~UOdysseyShape()
{
}

bool
UOdysseyShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
	return false;
}

bool
UOdysseyShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
	return false;
}

void
UOdysseyShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyShape::OnKeyDown(const FKey& iKey)
{
	if( iKey == EKeys::LeftShift || iKey == EKeys::RightShift )
	{
		Uniform = !Uniform;
		return true;
	}

	return false;
}

bool
UOdysseyShape::OnKeyUp(const FKey& iKey)
{
    if (iKey == EKeys::LeftShift || iKey == EKeys::RightShift)
    {
        Uniform = !Uniform;
		return true;
    }

	return false;
}

void
UOdysseyShape::Tick(float iDeltaTime)
{

}

void
UOdysseyShape::ApplyOverrides(const TMap< TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides)
{

}

void 
UOdysseyShape::SetHUD(TSharedPtr<FOdysseyHUDElement> iHUD)
{
	mHUD = iHUD;
}

float UOdysseyShape::GetStep() const
{
	return Step;
}

bool UOdysseyShape::AbortShape()
{
    mOnPathResetDelegate.Broadcast();
    mOnPathAbortDelegate.Broadcast();

	return true;
}

void UOdysseyShape::Draw(::ULIS::FBlock* iBlock, FOdysseyShapeDrawOptions& iOptions)
{

}
