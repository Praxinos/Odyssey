// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendMode.h"

EOdysseyColorBlendMode
GetColorBlendModeFromBlendMode(EOdysseyBlendMode InBlendMode)
{
    switch(InBlendMode)
    {
        case EOdysseyBlendMode::Normal: return EOdysseyColorBlendMode::Normal; break;
        case EOdysseyBlendMode::Behind: return EOdysseyColorBlendMode::Behind; break;

        case EOdysseyBlendMode::Mask: return EOdysseyColorBlendMode::Back; break;
        case EOdysseyBlendMode::Stencil: return EOdysseyColorBlendMode::Back; break;

        case EOdysseyBlendMode::Darken: return EOdysseyColorBlendMode::Darken; break;
        case EOdysseyBlendMode::Multiply: return EOdysseyColorBlendMode::Multiply; break;
        case EOdysseyBlendMode::ColorBurn: return EOdysseyColorBlendMode::ColorBurn; break;
        case EOdysseyBlendMode::LinearBurn: return EOdysseyColorBlendMode::LinearBurn; break;
        case EOdysseyBlendMode::DarkerColor: return EOdysseyColorBlendMode::DarkerColor; break;

        case EOdysseyBlendMode::Lighten: return EOdysseyColorBlendMode::Lighten; break;
        case EOdysseyBlendMode::Screen: return EOdysseyColorBlendMode::Screen; break;
        case EOdysseyBlendMode::ColorDodge: return EOdysseyColorBlendMode::ColorDodge; break;
        case EOdysseyBlendMode::LinearDodge: return EOdysseyColorBlendMode::LinearDodge; break;
        case EOdysseyBlendMode::LighterColor: return EOdysseyColorBlendMode::LighterColor; break;

        case EOdysseyBlendMode::Overlay: return EOdysseyColorBlendMode::Overlay; break;
        case EOdysseyBlendMode::SoftLight: return EOdysseyColorBlendMode::SoftLight; break;
        case EOdysseyBlendMode::HardLight: return EOdysseyColorBlendMode::HardLight; break;
        case EOdysseyBlendMode::VividLight: return EOdysseyColorBlendMode::VividLight; break;
        case EOdysseyBlendMode::LinearLight: return EOdysseyColorBlendMode::LinearLight; break;
        case EOdysseyBlendMode::PinLight: return EOdysseyColorBlendMode::PinLight; break;
        case EOdysseyBlendMode::HardMix: return EOdysseyColorBlendMode::HardMix; break;

        case EOdysseyBlendMode::Phoenix: return EOdysseyColorBlendMode::Phoenix; break;
        case EOdysseyBlendMode::Reflect: return EOdysseyColorBlendMode::Reflect; break;
        case EOdysseyBlendMode::Glow: return EOdysseyColorBlendMode::Glow; break;

        case EOdysseyBlendMode::Difference: return EOdysseyColorBlendMode::Difference; break;
        case EOdysseyBlendMode::Exclusion: return EOdysseyColorBlendMode::Exclusion; break;
        case EOdysseyBlendMode::Add: return EOdysseyColorBlendMode::Add; break;
        case EOdysseyBlendMode::Substract: return EOdysseyColorBlendMode::Substract; break;
        case EOdysseyBlendMode::Divide: return EOdysseyColorBlendMode::Divide; break;
        case EOdysseyBlendMode::Average: return EOdysseyColorBlendMode::Average; break;
        case EOdysseyBlendMode::Negation: return EOdysseyColorBlendMode::Negation; break;

        case EOdysseyBlendMode::Hue: return EOdysseyColorBlendMode::Hue; break;
        case EOdysseyBlendMode::Saturation: return EOdysseyColorBlendMode::Saturation; break;
        case EOdysseyBlendMode::Color: return EOdysseyColorBlendMode::Color; break;
        case EOdysseyBlendMode::Luminosity: return EOdysseyColorBlendMode::Luminosity; break;

        case EOdysseyBlendMode::PartialDerivative: return EOdysseyColorBlendMode::PartialDerivative; break;
        case EOdysseyBlendMode::WhiteOut: return EOdysseyColorBlendMode::WhiteOut; break;
        case EOdysseyBlendMode::AngleCorrected: return EOdysseyColorBlendMode::AngleCorrected; break;

        case EOdysseyBlendMode::Dissolve: return EOdysseyColorBlendMode::Dissolve; break;
        case EOdysseyBlendMode::BayerDither8x8: return EOdysseyColorBlendMode::BayerDither8x8; break;
    }

    check(false);
    return EOdysseyColorBlendMode::Normal;
}

EOdysseyAlphaBlendMode
GetAlphaBlendModeFromBlendMode(EOdysseyBlendMode InBlendMode)
{
    switch(InBlendMode)
    {
        case EOdysseyBlendMode::Normal: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Behind: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Mask: return EOdysseyAlphaBlendMode::Mask;
        case EOdysseyBlendMode::Stencil: return EOdysseyAlphaBlendMode::Stencil;

        case EOdysseyBlendMode::Darken: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Multiply: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::ColorBurn: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::LinearBurn: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::DarkerColor: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Lighten: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Screen: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::ColorDodge: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::LinearDodge: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::LighterColor: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Overlay: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::SoftLight: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::HardLight: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::VividLight: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::LinearLight: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::PinLight: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::HardMix: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Phoenix: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Reflect: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Glow: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Difference: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Exclusion: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Add: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Substract: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Divide: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Average: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Negation: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Hue: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Saturation: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Color: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::Luminosity: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::PartialDerivative: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::WhiteOut: return EOdysseyAlphaBlendMode::Normal;
        case EOdysseyBlendMode::AngleCorrected: return EOdysseyAlphaBlendMode::Normal;

        case EOdysseyBlendMode::Dissolve: return EOdysseyAlphaBlendMode::Dissolve; break;
        case EOdysseyBlendMode::BayerDither8x8: return EOdysseyAlphaBlendMode::BayerDither8x8; break;
    }

    check(false);
    return EOdysseyAlphaBlendMode::Normal;
}
