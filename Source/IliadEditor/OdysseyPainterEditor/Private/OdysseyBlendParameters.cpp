// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendParameters.h"
#include "Serialization/CustomVersion.h"

FOdysseyBlendParameters::FOdysseyBlendParameters()
    : bIsComposite(false)
    , BlendMode(EOdysseyBlendMode::Normal)
    , CompositeColorBlendMode(EOdysseyColorBlendMode::Normal)
    , CompositeAlphaBlendMode(EOdysseyAlphaBlendMode::Normal)
    , Opacity(100.0f)
{
}

FOdysseyBlendParameters::FOdysseyBlendParameters(EOdysseyBlendMode InBlendMode, float InOpacity)
    : bIsComposite(false)
    , BlendMode(InBlendMode)
    , CompositeColorBlendMode(EOdysseyColorBlendMode::Normal)
    , CompositeAlphaBlendMode(EOdysseyAlphaBlendMode::Normal)
    , Opacity(InOpacity)
{
}

FOdysseyBlendParameters::FOdysseyBlendParameters(EOdysseyColorBlendMode InColorBlendMode, EOdysseyAlphaBlendMode InAlphaBlendMode, float InOpacity)
    : bIsComposite(true)
    , BlendMode(EOdysseyBlendMode::Normal)
    , CompositeColorBlendMode(InColorBlendMode)
    , CompositeAlphaBlendMode(InAlphaBlendMode)
    , Opacity(InOpacity)
{
}
struct FOdysseyBlendParametersCustomVersion
{
    enum Type
    {
        // Before any version changes were made
        BeforeCustomVersionWasAdded,

        // Reworked how anim blueprint root nodes are recovered
        NewBlendModes,

        // -----<new versions can be added above this line>-------------------------------------------------
        VersionPlusOne,
        LatestVersion = VersionPlusOne - 1
    };

    // The GUID for this custom version number
    const FGuid GUID;
    FCustomVersionRegistration Registration;

    FOdysseyBlendParametersCustomVersion()
    : GUID{0x67CA77F8, 0x1F874407, 0xB5E84A70, 0xA0A2AA41}
    , Registration(GUID, FOdysseyBlendParametersCustomVersion::LatestVersion, TEXT("OdysseyBlendParameters"))
    {}
};
static FOdysseyBlendParametersCustomVersion GOdysseyBlendParametersCustomVersion;

bool
FOdysseyBlendParameters::Serialize(FArchive& Ar)
{
    Ar.UsingCustomVersion(GOdysseyBlendParametersCustomVersion.GUID);
    // doesn't actually serialize, just write the custom version for PostSerialize
    return false;
}

void
FOdysseyBlendParameters::PostSerialize(const FArchive& Ar)
{
    if (!Ar.IsLoading())
        return;

    if (Ar.CustomVer(GOdysseyBlendParametersCustomVersion.GUID) < FOdysseyBlendParametersCustomVersion::NewBlendModes)
    {
        bIsComposite = true;
        if (bEraserMode_DEPRECATED)
        {
            CompositeColorBlendMode = EOdysseyColorBlendMode::Back;
            CompositeAlphaBlendMode = EOdysseyAlphaBlendMode::Mask;
        }
        else
        {
            GetColorAndAlphaBlendModesFromDeprecatedBlendingMode(BlendingMode_DEPRECATED, AlphaMode_DEPRECATED, CompositeColorBlendMode, CompositeAlphaBlendMode);
        }
    }
}

void
FOdysseyBlendParameters::SetOpacity(float InOpacity)
{
    Opacity = InOpacity;
}

float
FOdysseyBlendParameters::GetOpacity() const
{
    return Opacity;
}

void
FOdysseyBlendParameters::SetBlendMode(EOdysseyBlendMode InBlendMode)
{
    BlendMode = InBlendMode;
}

EOdysseyBlendMode
FOdysseyBlendParameters::GetBlendMode() const
{
    return BlendMode;
}

EOdysseyColorBlendMode
FOdysseyBlendParameters::GetColorBlendMode() const
{
    if (bIsComposite)
        return CompositeColorBlendMode;

    return GetColorBlendModeFromBlendMode(BlendMode);
}

EOdysseyAlphaBlendMode
FOdysseyBlendParameters::GetAlphaBlendMode() const
{
    if (bIsComposite)
        return CompositeAlphaBlendMode;

    return GetAlphaBlendModeFromBlendMode(BlendMode);
}

void
FOdysseyBlendParameters::SetIsComposite(bool InIsComposite)
{
    bIsComposite = InIsComposite;
}

bool
FOdysseyBlendParameters::GetIsComposite() const
{
    return bIsComposite;
}

void
FOdysseyBlendParameters::SetCompositeColorBlendMode(EOdysseyColorBlendMode InColorBlendMode)
{
    CompositeColorBlendMode = InColorBlendMode;
}

void
FOdysseyBlendParameters::SetCompositeAlphaBlendMode(EOdysseyAlphaBlendMode InAlphaBlendMode)
{
    CompositeAlphaBlendMode = InAlphaBlendMode;
}

EOdysseyColorBlendMode
FOdysseyBlendParameters::GetCompositeColorBlendMode() const
{
    return CompositeColorBlendMode;
}

EOdysseyAlphaBlendMode
FOdysseyBlendParameters::GetCompositeAlphaBlendMode() const
{
    return CompositeAlphaBlendMode;
}

bool
FOdysseyBlendParameters::operator==(const FOdysseyBlendParameters& Other) const
{
    return bIsComposite == Other.bIsComposite
        && BlendMode == Other.BlendMode
        && CompositeColorBlendMode == Other.CompositeColorBlendMode
        && CompositeAlphaBlendMode == Other.CompositeAlphaBlendMode
        && Opacity == Other.Opacity;
}

bool
FOdysseyBlendParameters::operator!=(const FOdysseyBlendParameters& Other) const
{
    return !operator==(Other);
}
