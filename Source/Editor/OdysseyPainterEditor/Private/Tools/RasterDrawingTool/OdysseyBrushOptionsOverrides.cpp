// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyBrushOptionsOverrides.h"

#include "UObject/OdysseyObjectEditorUtils.h"

UOdysseyBrushOptionsOverrides::UOdysseyBrushOptionsOverrides()
    : bOverride_Size        ( false                                         )
    , bOverride_Flow        ( false                                         )
    , Size                  ( 20                                            )
    , Flow                  ( 100                                           )
{}

//Applies the Overrides to the given object
void
UOdysseyBrushOptionsOverrides::Override(UOdysseyBrushOptions* iBlendParameters) const
{
    if (bOverride_Size)
        FOdysseyObjectEditorUtils::SetPropertyValue(iBlendParameters, "Size", Size);
    if (bOverride_Flow)
        FOdysseyObjectEditorUtils::SetPropertyValue(iBlendParameters, "Flow", Flow);
}