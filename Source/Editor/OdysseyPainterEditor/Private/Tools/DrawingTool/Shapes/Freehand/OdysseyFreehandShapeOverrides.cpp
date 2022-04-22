// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/Shapes/Freehand/OdysseyFreehandShapeOverrides.h"

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
}