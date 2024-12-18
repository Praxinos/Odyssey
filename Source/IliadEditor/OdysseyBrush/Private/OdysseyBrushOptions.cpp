// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyBrushOptions.h"

UOdysseyBrushOptions::UOdysseyBrushOptions(const FObjectInitializer& iObjectInitializer)
    :  Super(iObjectInitializer)

    //Properties
    , Color( ::ULIS::FColor::Black )
{
}

void
UOdysseyBrushOptions::PostEditChangeProperty(struct FPropertyChangedEvent & PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    if (PropertyChangedEvent.ChangeType & EPropertyChangeType::Interactive)
        return;

    mOnPropertyChangedDelegate.Broadcast();
}
