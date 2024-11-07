// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/RasterDrawingTool/OdysseyBlendParametersOverrides.h"

#include "ObjectEditorUtils.h"

UOdysseyBlendParametersOverrides::UOdysseyBlendParametersOverrides()
    : bOverride_Opacity     ( false                                         )
    , bOverride_BlendingMode( false                                         )
    , bOverride_AlphaMode   ( false                                         )
    , Opacity               ( 100                                           )
    , BlendingMode          ( EOdysseyBlendingMode::kNormal                 )
    , AlphaMode             ( EOdysseyAlphaMode::kNormal                    )
{}
