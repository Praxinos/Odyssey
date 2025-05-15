// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyAntiAliasing.h"

#include "RHIStaticStates.h"

namespace Odyssey
{

FSamplerStateRHIRef GetSamplerStateForAntiAliasing(EOdysseyAntiAliasing iAntiAliasing)
{
    FSamplerStateRHIRef samplerStateRHI = TStaticSamplerState< SF_Point, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
    switch (iAntiAliasing)
    {
    case EOdysseyAntiAliasing::NearestNeighbor:
        samplerStateRHI = TStaticSamplerState< SF_Point, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
        break;
    case EOdysseyAntiAliasing::Bilinear:
        samplerStateRHI = TStaticSamplerState< SF_Bilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
        break;
    case EOdysseyAntiAliasing::Trilinear:
        samplerStateRHI = TStaticSamplerState< SF_Trilinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
        break;
    case EOdysseyAntiAliasing::Anisotropic:
        samplerStateRHI = TStaticSamplerState< SF_AnisotropicPoint, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
        break;
    case EOdysseyAntiAliasing::AnisotropicLinear:
        samplerStateRHI = TStaticSamplerState< SF_AnisotropicLinear, AM_Clamp, AM_Clamp, AM_Clamp >::GetRHI();
        break;
    }

    return samplerStateRHI;
}

}
