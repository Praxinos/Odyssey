// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "StrokeEngine/OdysseyBlendParametersOverrides.h"

#include "ObjectEditorUtils.h"
#include "StrokeEngine/OdysseyStrokeEngine.h"

UOdysseyBlendParametersOverrides::UOdysseyBlendParametersOverrides()
    : bOverride_Opacity     ( false                                         )
    , bOverride_BlendingMode( false                                         )
    , bOverride_AlphaMode   ( false                                         )
    , Opacity               ( 100                                           )
    , BlendingMode          ( EOdysseyBlendingMode::kNormal                 )
    , AlphaMode             ( EOdysseyAlphaMode::kNormal                    )
{}

void
UOdysseyBlendParametersOverrides::Override(FOdysseyBlendParameters& iBendParameters) const
{
    if (bOverride_Opacity)
        iBendParameters.Opacity = Opacity;

    if (bOverride_BlendingMode)
        iBendParameters.BlendingMode = BlendingMode;
    
    if (bOverride_AlphaMode)
        iBendParameters.AlphaMode = AlphaMode;
}