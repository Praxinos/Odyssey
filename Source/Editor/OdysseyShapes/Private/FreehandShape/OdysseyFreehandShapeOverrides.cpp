// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FreehandShape/OdysseyFreehandShapeOverrides.h"

#include "ObjectEditorUtils.h"

UOdysseyFreehandShapeOverrides::UOdysseyFreehandShapeOverrides()
    : bOverride_SmoothingMethod      ( false                                         )
    , bOverride_SmoothingStrength    ( false                                         )
    , bOverride_SmoothingEnabled     ( false                                         )
    , bOverride_SmoothingRealTime    ( false                                         )
    , bOverride_SmoothingCatchUp     ( false                                         )
    , bOverride_Step                 ( false                                         )
    , bOverride_AdaptativeStep       ( false                                         )
    , bOverride_InterpolationType    ( false                                         )
    , SmoothingMethod                ( EOdysseySmoothingMethod::kAverage             )
    , SmoothingStrength              ( 10                                            )
    , SmoothingEnabled               ( true                                          )
    , SmoothingRealTime              ( true                                          )
    , SmoothingCatchUp               ( true                                          )
    , Step                           ( 20                                            )
    , AdaptativeStep                 ( true                                          )
    , InterpolationType              ( EOdysseyInterpolationType::kCatmullRom   )
{}

void
UOdysseyFreehandShapeOverrides::Override(UOdysseyFreehandShape* iFreehandShape) const
{
    FOdysseySmoothingOptions smoothingOptions = iFreehandShape->GetSmoothingOptions();
    if(bOverride_SmoothingMethod)
        smoothingOptions.SmoothingMethod = SmoothingMethod;
    if(bOverride_SmoothingStrength)
        smoothingOptions.SmoothingStrength = SmoothingStrength;
    if(bOverride_SmoothingEnabled)
        smoothingOptions.SmoothingEnabled = SmoothingEnabled;
    if(bOverride_SmoothingRealTime)
        smoothingOptions.SmoothingRealTime = SmoothingRealTime;
    if(bOverride_SmoothingCatchUp)
        smoothingOptions.SmoothingCatchUp = SmoothingCatchUp;
    
    FObjectEditorUtils::SetPropertyValue(iFreehandShape, "SmoothingOptions", smoothingOptions);

    if (bOverride_Step)
        FObjectEditorUtils::SetPropertyValue(iFreehandShape, "Step", Step);
    if (bOverride_AdaptativeStep)
        FObjectEditorUtils::SetPropertyValue(iFreehandShape, "AdaptativeStep", AdaptativeStep);
    if (bOverride_InterpolationType)
        FObjectEditorUtils::SetPropertyValue(iFreehandShape, "InterpolationType", InterpolationType);
}