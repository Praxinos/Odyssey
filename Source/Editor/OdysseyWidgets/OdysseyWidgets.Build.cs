// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;
using UnrealBuildTool;

public class OdysseyWidgets : ModuleRules
{
    public OdysseyWidgets(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "InputCore",
                "EditorStyle",
                "EditorWidgets",
                "Engine",
                "UnrealEd",
                "Layers",
                "MainFrame",
                "GraphEditor",
                "Projects",
                "PropertyEditor",
                "OdysseyBrush",
                "OdysseyImaging",
                "OdysseyLayer",
                "OdysseyStyle",
                "OdysseyStrokePipeline",
                "OdysseyTransaction",
                "OdysseyMaths",
                "RenderCore",
                "Paper2D",
				"KismetWidgets"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader",
                "EditorWidgets",
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Timeline"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public", "Timeline"),
                Path.Combine(ModuleDirectory, "Public", "Timeline", "Events"),
                Path.Combine(ModuleDirectory, "Public", "Common"),
                Path.Combine(ModuleDirectory, "Public", "Common", "Events"),
            }
        );
    }
}
