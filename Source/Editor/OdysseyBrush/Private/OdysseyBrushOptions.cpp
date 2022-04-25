// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyBrushOptions.h"

UOdysseyBrushOptions::UOdysseyBrushOptions(const FObjectInitializer& iObjectInitializer)
    :  Super(iObjectInitializer)

    //Properties
    , Color( ::ULIS::FColor::Black )
    , mInterpolator(MakeShared<FOdysseyInterpolationCatmullRom>())
{
    float adaptedStep = FMath::Max( 1.f, ( Step / 100.f ) * mSizeModifier );
    mInterpolator->SetStep(SizeAdaptative ? adaptedStep : Step);
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
        float adaptedStep = FMath::Max( 1.f, ( Step / 100.f ) * mSizeModifier );
        mInterpolator->SetStep(SizeAdaptative ? adaptedStep : Step);
    }

    mOnPropertyChangedDelegate.Broadcast();
}
