// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using UnrealBuildTool;

public class OdysseyEditorBrushBlueprints : ModuleRules
{
    public OdysseyEditorBrushBlueprints(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",

                "OdysseyBrush",
                "OdysseyLayer",
                "OdysseyPainterEditor",
                "OdysseyTextureEditor",
                "OdysseyTools",

                "ULIS",
                "ULISLoader",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
            }
        );
    }
}
