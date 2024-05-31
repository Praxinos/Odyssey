// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class OdysseyVector : ModuleRules
{
    public OdysseyVector(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "Slate",
                "SlateCore",
                "EditorScriptingUtilities",
                "XmlParser",
                "OdysseyFile",
                "OdysseyCore",
                "OdysseyStyle",
                "OdysseyPalette"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "XmlParser",
                "ULIS",
                "ULISLoader",
                "blend2d",
                "libeigen"
            }
        );
    }
}
