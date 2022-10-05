// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                "ULIS",
                "ULISLoader",
            }
        );
    }
}
