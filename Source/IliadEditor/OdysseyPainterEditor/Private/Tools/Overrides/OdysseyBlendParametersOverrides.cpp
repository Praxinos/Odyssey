// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyBlendParametersOverrides.h"

UOdysseyBlendParametersOverrides::UOdysseyBlendParametersOverrides()
    : bOverride_Opacity     ( false )
    , bOverride_IsComposite ( false )
    , bOverride_BlendMode   ( false )
    , bOverride_CompositeColorBlendMode   ( false )
    , bOverride_CompositeAlphaBlendMode   ( false )
    , Opacity               ( 100 )
    , bIsComposite ( true )
    , BlendMode   ( EOdysseyBlendMode::Normal )
    , CompositeColorBlendMode   ( EOdysseyColorBlendMode::Normal )
    , CompositeAlphaBlendMode   ( EOdysseyAlphaBlendMode::Normal )
{}
struct FOdysseyBlendParametersOverridesCustomVersion
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

    FOdysseyBlendParametersOverridesCustomVersion()
    : GUID{0x5991FD36, 0x1BB5492A, 0xA75A2D06, 0xC56A297A}
    , Registration(GUID, FOdysseyBlendParametersOverridesCustomVersion::LatestVersion, TEXT("OdysseyBlendParametersOverrides"))
    {}
};
static FOdysseyBlendParametersOverridesCustomVersion GOdysseyBlendParametersOverridesCustomVersion;

void
UOdysseyBlendParametersOverrides::Serialize(FArchive& Ar)
{
    Super::Serialize(Ar);

    Ar.UsingCustomVersion(GOdysseyBlendParametersOverridesCustomVersion.GUID);

    if (Ar.IsLoading() && Ar.CustomVer(GOdysseyBlendParametersOverridesCustomVersion.GUID) < FOdysseyBlendParametersOverridesCustomVersion::NewBlendModes)
    {
        bOverride_IsComposite = bOverride_BlendingMode_DEPRECATED | bOverride_AlphaMode_DEPRECATED;
        bOverride_CompositeColorBlendMode = bOverride_BlendingMode_DEPRECATED;
        bOverride_CompositeAlphaBlendMode = bOverride_AlphaMode_DEPRECATED;

        bIsComposite = bOverride_BlendingMode_DEPRECATED | bOverride_AlphaMode_DEPRECATED;
        GetColorAndAlphaBlendModesFromDeprecatedBlendingMode(BlendingMode_DEPRECATED, AlphaMode_DEPRECATED, CompositeColorBlendMode, CompositeAlphaBlendMode);
    }
}
