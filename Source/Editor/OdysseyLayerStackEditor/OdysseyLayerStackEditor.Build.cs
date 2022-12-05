// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;

public class OdysseyLayerStackEditor : ModuleRules
{
    public OdysseyLayerStackEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
				"CoreUObject",
                "EditorStyle",
				"Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "UMG",
                "UnrealEd",
                "ToolMenus",

                "OdysseyCore",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyMaths",
                "OdysseyStyle",

                "ULIS",
                "ULISLoader"
            }
        );
    }
}
