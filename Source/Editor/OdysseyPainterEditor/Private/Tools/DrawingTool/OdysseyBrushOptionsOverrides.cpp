// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "Tools/DrawingTool/OdysseyBrushOptionsOverrides.h"

UOdysseyBrushOptionsOverrides::UOdysseyBrushOptionsOverrides()
    : bOverride_Size        ( false                                         )
    , bOverride_Flow        ( false                                         )
    , bOverride_Step        ( false                                         )
    , bOverride_Adaptative  ( false                                         )
    , bOverride_InterpolationType ( false                                         )
    , Size                  ( 20                                            )
    , Flow                  ( 100                                           )
    , Step                  ( 20                                            )
    , SizeAdaptative        ( true                                          )
    , InterpolationType     ( EOdysseyBrushInterpolationType::kCatmullRom   )
{}

//Applies the Overrides to the given object
void
UOdysseyBrushOptionsOverrides::Override(UOdysseyBrushOptions* iBlendParameters) const
{
    if (bOverride_Size)
        FObjectEditorUtils::SetPropertyValue(iBlendParameters, "Size", Size);
    if (bOverride_Flow)
        FObjectEditorUtils::SetPropertyValue(iBlendParameters, "Flow", Flow);
    if (bOverride_Step)
        FObjectEditorUtils::SetPropertyValue(iBlendParameters, "Step", Step);
    if (bOverride_Adaptative)
        FObjectEditorUtils::SetPropertyValue(iBlendParameters, "SizeAdaptative", SizeAdaptative);
    if (bOverride_InterpolationType)
        FObjectEditorUtils::SetPropertyValue(iBlendParameters, "InterpolationType", InterpolationType);
}