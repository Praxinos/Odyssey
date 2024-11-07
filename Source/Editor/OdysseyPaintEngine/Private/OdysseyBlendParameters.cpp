// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyBlendParameters.h"

FOdysseyBlendParameters::FOdysseyBlendParameters()
    : bEraserMode(false)
    , BlendingMode(EOdysseyBlendingMode::kNormal)
    , AlphaMode(EOdysseyAlphaMode::kNormal)
    , Opacity(100.0f)
{
}

FOdysseyBlendParameters::FOdysseyBlendParameters(bool iEraserMode, EOdysseyBlendingMode iBlendingMode, EOdysseyAlphaMode iAlphaMode, float iOpacity)
    : bEraserMode(iEraserMode)
    , BlendingMode(iBlendingMode)
    , AlphaMode(iAlphaMode)
    , Opacity(iOpacity)
{
}

bool
FOdysseyBlendParameters::operator==(const FOdysseyBlendParameters& rhs) const
{
    return BlendingMode == rhs.BlendingMode && AlphaMode == rhs.AlphaMode && Opacity == rhs.Opacity && bEraserMode == rhs.bEraserMode; // or another approach as above
}

bool
FOdysseyBlendParameters::operator!=(const FOdysseyBlendParameters& rhs) const
{
    return !operator==(rhs);
}