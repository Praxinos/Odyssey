// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "StrokeEngine/Shapes/OdysseyFreehandShapeOverrides.h"

UOdysseyFreehandShapeOverrides::UOdysseyFreehandShapeOverrides()
    : bOverride_SmoothingMethod      ( false                                         )
    , bOverride_SmoothingStrength    ( false                                         )
    , bOverride_SmoothingEnabled     ( false                                         )
    , bOverride_SmoothingRealTime    ( false                                         )
    , bOverride_SmoothingCatchUp     ( false                                         )
    , SmoothingMethod                ( EOdysseySmoothingMethod::kAverage             )
    , SmoothingStrength              ( 10                                            )
    , SmoothingEnabled               ( true                                          )
    , SmoothingRealTime              ( true                                          )
    , SmoothingCatchUp               ( true                                          )
{}

void
UOdysseyFreehandShapeOverrides::Override(UOdysseyFreehandShape* iFreehandShape) const
{
    FOdysseyStrokeOptions strokeOptions = iFreehandShape->GetStrokeOptions();
    if(bOverride_SmoothingMethod)
        strokeOptions.SmoothingMethod = SmoothingMethod;
    if(bOverride_SmoothingStrength)
        strokeOptions.SmoothingStrength = SmoothingStrength;
    if(bOverride_SmoothingEnabled)
        strokeOptions.SmoothingEnabled = SmoothingEnabled;
    if(bOverride_SmoothingRealTime)
        strokeOptions.SmoothingRealTime = SmoothingRealTime;
    if(bOverride_SmoothingCatchUp)
        strokeOptions.SmoothingCatchUp = SmoothingCatchUp;
    FObjectEditorUtils::SetPropertyValue(iFreehandShape, "StrokeOptions", strokeOptions);
}