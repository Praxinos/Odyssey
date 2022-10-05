// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class OdysseyTools : ModuleRules
{
    public OdysseyTools(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "SlateCore",
                "Engine",
                "OdysseyCore",
                "OdysseyWidgets"
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader",
             }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "DrawingTools"),
            }
        );

    }
}
