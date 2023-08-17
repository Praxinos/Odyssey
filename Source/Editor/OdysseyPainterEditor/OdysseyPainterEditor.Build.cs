// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class OdysseyPainterEditor : ModuleRules
{
    public OdysseyPainterEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
                "PropertyEditor",
                "AssetTools",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "blend2d",
                "AppFramework",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "InputCore",
                "LevelEditor",
                "Engine",
                "RHI",
                "RenderCore",
                "RawMesh",
                "ToolMenus",
                "EditorStyle",
                "EditorFramework",
                "PropertyEditor",
                "Projects",
                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyHUDSystem",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyVector",
                "OdysseyWidgets"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyPaintEngine",
                "OdysseyShapes",
                "OdysseyVector",
                "OdysseyPalette",
                "OdysseyMedia",
                "ULIS",
                "ULISLoader"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Public", "Settings"),
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
             new string[] {
                "MainFrame",
                "WorkspaceMenuStructure",
                "AssetTools",
                "PackagesDialog"
             }
        );

    }
}
