// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;
using UnrealBuildTool;

public class OdysseyTextureEditorBlueprint : ModuleRules
{
    public OdysseyTextureEditorBlueprint(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "OdysseyBrush",
                "OdysseyImaging",
                "OdysseyLayer",
                "OdysseyTextureEditor",
                "ULIS",
                "ULISLoader"
            }
        );
    }
}
