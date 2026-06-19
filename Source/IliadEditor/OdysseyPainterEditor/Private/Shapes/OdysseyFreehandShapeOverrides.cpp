// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyFreehandShapeOverrides.h"
#include "UObject/OdysseyObjectEditorUtils.h"

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
    , InterpolationType              ( EOdysseyInterpolationType::kCatmullRom        )
{}
