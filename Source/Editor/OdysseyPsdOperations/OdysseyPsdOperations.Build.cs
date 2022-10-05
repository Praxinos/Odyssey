// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class OdysseyPsdOperations : ModuleRules
{
    public OdysseyPsdOperations(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "Engine",
                "OdysseyCore",
                "OdysseyMaths",
                "OdysseyLayer",
                "OdysseyImaging",
                "ULIS",
                "ULISLoader",
                "zlib"
             }
        );
    }
}
