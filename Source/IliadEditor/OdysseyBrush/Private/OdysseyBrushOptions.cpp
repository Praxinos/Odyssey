// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBrushOptions.h"

UOdysseyBrushOptions::UOdysseyBrushOptions(const FObjectInitializer& iObjectInitializer)
    :  Super(iObjectInitializer)

    //Properties
    , Color( ::ULIS::FColor::Black )
{
}

void
UOdysseyBrushOptions::BeginInteractiveMode()
{
    mIsInInteractiveMode = true;
}

void
UOdysseyBrushOptions::EndInteractiveMode()
{
    mIsInInteractiveMode = false;
}

bool
UOdysseyBrushOptions::IsInInteractiveMode() const
{
    return mIsInInteractiveMode;
}

void
UOdysseyBrushOptions::SetSize(float InSize)
{
    Size = InSize;
}

float
UOdysseyBrushOptions::GetSize() const
{
    return Size;
}

void
UOdysseyBrushOptions::SetFlow(float InFlow)
{
    Flow = InFlow;
}

float
UOdysseyBrushOptions::GetFlow() const
{
    return Flow;
}

void
UOdysseyBrushOptions::SetColor(FOdysseyBrushColor InColor)
{
    Color = InColor;
}

FOdysseyBrushColor
UOdysseyBrushOptions::GetColor() const
{
    return Color;
}

void
UOdysseyBrushOptions::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    mOnPropertyChangedDelegate.Broadcast();
}
