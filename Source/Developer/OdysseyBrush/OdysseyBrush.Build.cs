// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyBrush : ModuleRules
{
    public OdysseyBrush(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "OdysseyCore",
                "OdysseyImaging",
                "OdysseyPaintEngine",
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
                "InputCore",
                "OdysseyCore",
                "Engine",
                "ULIS"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                // "OdysseyCore",
                // "OdysseyImaging",
                // "OdysseyPaintEngine",
                "ULIS",
                "ULISLoader",
            }
        );
    }
}
