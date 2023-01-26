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
                "AppFramework",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "InputCore",
                "Engine",
                "RHI",
                "RenderCore",
                "RawMesh",
                "ToolMenus",
                "EditorStyle",
                "PropertyEditor",
                "Projects",
                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyImaging",
                "OdysseyHUDSystem",
                "OdysseyLayerStack",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyTexture",
                "OdysseyTextureEditor",
                "OdysseyVector",
                "OdysseyWidgets"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyLayerStack",
                "OdysseyPaintEngine",
                "OdysseyShapes",
                "OdysseyTexture",
                "OdysseyTextureEditor",
                "OdysseyVector",
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
