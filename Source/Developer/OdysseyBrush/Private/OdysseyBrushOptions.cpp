// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushOptions.h"

UOdysseyBrushOptions::UOdysseyBrushOptions(const FObjectInitializer& iObjectInitializer)
    :  Super(iObjectInitializer)

    //Properties
    , mInterpolator(MakeShared<FOdysseyInterpolationCatmullRom>())
{
    mInterpolator->SetStep(Step * (SizeAdaptative ? Size : 1.0));
}

TSharedPtr<IOdysseyInterpolation>
UOdysseyBrushOptions::GetInterpolator()
{
    return mInterpolator;
}

void
UOdysseyBrushOptions::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;
        
    FName propertyName = PropertyChangedEvent.GetPropertyName();

    if (propertyName == "InterpolationType")
    {
        switch(InterpolationType)
        {
            case EOdysseyBrushInterpolationType::kCatmullRom: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;
            case EOdysseyBrushInterpolationType::kBezier: mInterpolator = MakeShared<FOdysseyInterpolationBezier>(); break;
            case EOdysseyBrushInterpolationType::kLine: mInterpolator = MakeShared<FOdysseyInterpolationCatmullRom>(); break;

            default: break;
        }
    }

    if (propertyName == "InterpolationType" || propertyName == "Step" || propertyName == "SizeAdaptative")
    {
        mInterpolator->SetStep(Step * (SizeAdaptative ? Size : 1.0));
    }

    mOnPropertyChangedDelegate.Broadcast();
}
