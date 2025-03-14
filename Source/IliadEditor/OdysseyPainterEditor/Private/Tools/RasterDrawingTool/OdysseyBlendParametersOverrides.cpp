// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

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
