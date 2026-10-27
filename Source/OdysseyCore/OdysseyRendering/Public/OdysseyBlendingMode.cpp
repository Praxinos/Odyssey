// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendingMode.h"


void
GetColorAndAlphaBlendModesFromDeprecatedBlendingMode(EOdysseyBlendingMode InBlendingMode, EOdysseyAlphaMode InAlphaMode, EOdysseyColorBlendMode& OutColorBlendMode, EOdysseyAlphaBlendMode& OutAlphaBlendMode)
{
    OutColorBlendMode = EOdysseyColorBlendMode::Normal;
    OutAlphaBlendMode = EOdysseyAlphaBlendMode::Normal;

    switch(InBlendingMode)
    {
        case EOdysseyBlendingMode::kNormal:
            OutColorBlendMode = EOdysseyColorBlendMode::Normal;
        break;

        case EOdysseyBlendingMode::kTop:
            OutColorBlendMode = EOdysseyColorBlendMode::Top;
        break;

        case EOdysseyBlendingMode::kBack:
            OutColorBlendMode = EOdysseyColorBlendMode::Back;
        break;

        case EOdysseyBlendingMode::kBehind:
            OutColorBlendMode = EOdysseyColorBlendMode::Behind;
        break;

        case EOdysseyBlendingMode::kDissolve:
        {
            OutColorBlendMode = EOdysseyColorBlendMode::Dissolve;
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Dissolve;
            return;
        }
        //break;

        case EOdysseyBlendingMode::kBayerDither8x8:
        {
            OutColorBlendMode = EOdysseyColorBlendMode::BayerDither8x8;
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::BayerDither8x8;
            return;
        }
        //break;

        case EOdysseyBlendingMode::kDarken:
            OutColorBlendMode = EOdysseyColorBlendMode::Darken;
        break;

        case EOdysseyBlendingMode::kMultiply:
            OutColorBlendMode = EOdysseyColorBlendMode::Multiply;
        break;

        case EOdysseyBlendingMode::kColorBurn:
            OutColorBlendMode = EOdysseyColorBlendMode::ColorBurn;
        break;

        case EOdysseyBlendingMode::kLinearBurn:
            OutColorBlendMode = EOdysseyColorBlendMode::LinearBurn;
        break;

        case EOdysseyBlendingMode::kDarkerColor:
            OutColorBlendMode = EOdysseyColorBlendMode::DarkerColor;
        break;

        case EOdysseyBlendingMode::kLighten:
            OutColorBlendMode = EOdysseyColorBlendMode::Lighten;
        break;

        case EOdysseyBlendingMode::kScreen:
            OutColorBlendMode = EOdysseyColorBlendMode::Screen;
        break;

        case EOdysseyBlendingMode::kColorDodge:
            OutColorBlendMode = EOdysseyColorBlendMode::ColorDodge;
        break;

        case EOdysseyBlendingMode::kLinearDodge:
            OutColorBlendMode = EOdysseyColorBlendMode::LinearDodge;
        break;

        case EOdysseyBlendingMode::kLighterColor:
            OutColorBlendMode = EOdysseyColorBlendMode::LighterColor;
        break;

        case EOdysseyBlendingMode::kOverlay:
            OutColorBlendMode = EOdysseyColorBlendMode::Overlay;
        break;

        case EOdysseyBlendingMode::kSoftLight:
            OutColorBlendMode = EOdysseyColorBlendMode::SoftLight;
        break;

        case EOdysseyBlendingMode::kHardLight:
            OutColorBlendMode = EOdysseyColorBlendMode::HardLight;
        break;

        case EOdysseyBlendingMode::kVividLight:
            OutColorBlendMode = EOdysseyColorBlendMode::VividLight;
        break;

        case EOdysseyBlendingMode::kLinearLight:
            OutColorBlendMode = EOdysseyColorBlendMode::LinearLight;
        break;

        case EOdysseyBlendingMode::kPinLight:
            OutColorBlendMode = EOdysseyColorBlendMode::PinLight;
        break;

        case EOdysseyBlendingMode::kHardMix:
            OutColorBlendMode = EOdysseyColorBlendMode::HardMix;
        break;

        case EOdysseyBlendingMode::kPhoenix:
            OutColorBlendMode = EOdysseyColorBlendMode::Phoenix;
        break;

        case EOdysseyBlendingMode::kReflect:
            OutColorBlendMode = EOdysseyColorBlendMode::Reflect;
        break;

        case EOdysseyBlendingMode::kGlow:
            OutColorBlendMode = EOdysseyColorBlendMode::Glow;
        break;

        case EOdysseyBlendingMode::kDifference:
            OutColorBlendMode = EOdysseyColorBlendMode::Difference;
        break;

        case EOdysseyBlendingMode::kExclusion:
            OutColorBlendMode = EOdysseyColorBlendMode::Exclusion;
        break;

        case EOdysseyBlendingMode::kAdd:
            OutColorBlendMode = EOdysseyColorBlendMode::Add;
        break;

        case EOdysseyBlendingMode::kSubstract:
            OutColorBlendMode = EOdysseyColorBlendMode::Substract;
        break;

        case EOdysseyBlendingMode::kDivide:
            OutColorBlendMode = EOdysseyColorBlendMode::Divide;
        break;

        case EOdysseyBlendingMode::kAverage:
            OutColorBlendMode = EOdysseyColorBlendMode::Average;
        break;

        case EOdysseyBlendingMode::kNegation:
            OutColorBlendMode = EOdysseyColorBlendMode::Negation;
        break;

        case EOdysseyBlendingMode::kHue:
            OutColorBlendMode = EOdysseyColorBlendMode::Hue;
        break;

        case EOdysseyBlendingMode::kSaturation:
            OutColorBlendMode = EOdysseyColorBlendMode::Saturation;
        break;

        case EOdysseyBlendingMode::kColor:
            OutColorBlendMode = EOdysseyColorBlendMode::Color;
        break;

        case EOdysseyBlendingMode::kLuminosity:
            OutColorBlendMode = EOdysseyColorBlendMode::Luminosity;
        break;

        case EOdysseyBlendingMode::kPartialDerivative:
            OutColorBlendMode = EOdysseyColorBlendMode::PartialDerivative;
        break;

        case EOdysseyBlendingMode::kWhiteOut:
            OutColorBlendMode = EOdysseyColorBlendMode::WhiteOut;
        break;

        case EOdysseyBlendingMode::kAngleCorrected:
            OutColorBlendMode = EOdysseyColorBlendMode::AngleCorrected;
        break;
    }

    switch(InAlphaMode)
    {
        case EOdysseyAlphaMode::kNormal:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Normal;
        break;

        case EOdysseyAlphaMode::kErase:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Erase;
        break;

        case EOdysseyAlphaMode::kTop:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Top;
        break;

        case EOdysseyAlphaMode::kBack:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Back;
        break;

        case EOdysseyAlphaMode::kSub:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Substract;
        break;

        case EOdysseyAlphaMode::kAdd:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Add;
        break;

        case EOdysseyAlphaMode::kMul:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Multiply;
        break;

        case EOdysseyAlphaMode::kMin:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Min;
        break;

        case EOdysseyAlphaMode::kMax:
            OutAlphaBlendMode = EOdysseyAlphaBlendMode::Max;
        break;
    }
}

void
GetBlendModeFromDeprecatedBlendingMode(EOdysseyBlendingMode InBlendingMode, EOdysseyBlendMode& OutBlendMode)
{
    OutBlendMode = EOdysseyBlendMode::Normal;

    switch(InBlendingMode)
    {
        case EOdysseyBlendingMode::kNormal:
            OutBlendMode = EOdysseyBlendMode::Normal;
        break;

        case EOdysseyBlendingMode::kTop:
            OutBlendMode = EOdysseyBlendMode::Normal;
        break;

        case EOdysseyBlendingMode::kBack:
            OutBlendMode = EOdysseyBlendMode::Normal;
        break;

        case EOdysseyBlendingMode::kBehind:
            OutBlendMode = EOdysseyBlendMode::Behind;
        break;

        case EOdysseyBlendingMode::kDissolve:
            OutBlendMode = EOdysseyBlendMode::Dissolve;
        break;

        case EOdysseyBlendingMode::kBayerDither8x8:
            OutBlendMode = EOdysseyBlendMode::BayerDither8x8;
        break;

        case EOdysseyBlendingMode::kDarken:
            OutBlendMode = EOdysseyBlendMode::Darken;
        break;

        case EOdysseyBlendingMode::kMultiply:
            OutBlendMode = EOdysseyBlendMode::Multiply;
        break;

        case EOdysseyBlendingMode::kColorBurn:
            OutBlendMode = EOdysseyBlendMode::ColorBurn;
        break;

        case EOdysseyBlendingMode::kLinearBurn:
            OutBlendMode = EOdysseyBlendMode::LinearBurn;
        break;

        case EOdysseyBlendingMode::kDarkerColor:
            OutBlendMode = EOdysseyBlendMode::DarkerColor;
        break;

        case EOdysseyBlendingMode::kLighten:
            OutBlendMode = EOdysseyBlendMode::Lighten;
        break;

        case EOdysseyBlendingMode::kScreen:
            OutBlendMode = EOdysseyBlendMode::Screen;
        break;

        case EOdysseyBlendingMode::kColorDodge:
            OutBlendMode = EOdysseyBlendMode::ColorDodge;
        break;

        case EOdysseyBlendingMode::kLinearDodge:
            OutBlendMode = EOdysseyBlendMode::LinearDodge;
        break;

        case EOdysseyBlendingMode::kLighterColor:
            OutBlendMode = EOdysseyBlendMode::LighterColor;
        break;

        case EOdysseyBlendingMode::kOverlay:
            OutBlendMode = EOdysseyBlendMode::Overlay;
        break;

        case EOdysseyBlendingMode::kSoftLight:
            OutBlendMode = EOdysseyBlendMode::SoftLight;
        break;

        case EOdysseyBlendingMode::kHardLight:
            OutBlendMode = EOdysseyBlendMode::HardLight;
        break;

        case EOdysseyBlendingMode::kVividLight:
            OutBlendMode = EOdysseyBlendMode::VividLight;
        break;

        case EOdysseyBlendingMode::kLinearLight:
            OutBlendMode = EOdysseyBlendMode::LinearLight;
        break;

        case EOdysseyBlendingMode::kPinLight:
            OutBlendMode = EOdysseyBlendMode::PinLight;
        break;

        case EOdysseyBlendingMode::kHardMix:
            OutBlendMode = EOdysseyBlendMode::HardMix;
        break;

        case EOdysseyBlendingMode::kPhoenix:
            OutBlendMode = EOdysseyBlendMode::Phoenix;
        break;

        case EOdysseyBlendingMode::kReflect:
            OutBlendMode = EOdysseyBlendMode::Reflect;
        break;

        case EOdysseyBlendingMode::kGlow:
            OutBlendMode = EOdysseyBlendMode::Glow;
        break;

        case EOdysseyBlendingMode::kDifference:
            OutBlendMode = EOdysseyBlendMode::Difference;
        break;

        case EOdysseyBlendingMode::kExclusion:
            OutBlendMode = EOdysseyBlendMode::Exclusion;
        break;

        case EOdysseyBlendingMode::kAdd:
            OutBlendMode = EOdysseyBlendMode::Add;
        break;

        case EOdysseyBlendingMode::kSubstract:
            OutBlendMode = EOdysseyBlendMode::Substract;
        break;

        case EOdysseyBlendingMode::kDivide:
            OutBlendMode = EOdysseyBlendMode::Divide;
        break;

        case EOdysseyBlendingMode::kAverage:
            OutBlendMode = EOdysseyBlendMode::Average;
        break;

        case EOdysseyBlendingMode::kNegation:
            OutBlendMode = EOdysseyBlendMode::Negation;
        break;

        case EOdysseyBlendingMode::kHue:
            OutBlendMode = EOdysseyBlendMode::Hue;
        break;

        case EOdysseyBlendingMode::kSaturation:
            OutBlendMode = EOdysseyBlendMode::Saturation;
        break;

        case EOdysseyBlendingMode::kColor:
            OutBlendMode = EOdysseyBlendMode::Color;
        break;

        case EOdysseyBlendingMode::kLuminosity:
            OutBlendMode = EOdysseyBlendMode::Luminosity;
        break;

        case EOdysseyBlendingMode::kPartialDerivative:
            OutBlendMode = EOdysseyBlendMode::PartialDerivative;
        break;

        case EOdysseyBlendingMode::kWhiteOut:
            OutBlendMode = EOdysseyBlendMode::WhiteOut;
        break;

        case EOdysseyBlendingMode::kAngleCorrected:
            OutBlendMode = EOdysseyBlendMode::AngleCorrected;
        break;
    }
}
