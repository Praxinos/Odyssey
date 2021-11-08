// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;
using UnrealBuildTool;

public class OdysseyTextureEditor : ModuleRules
{
    public OdysseyTextureEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "DesktopPlatform",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "InputCore",
                "Engine",
                "RHI",
                "RenderCore",
                "ToolMenus",
                "RawMesh",
                "EditorStyle",
                "PropertyEditor",
                "Projects",
                "OdysseyBrush",
                "OdysseyWidgets",
                "OdysseyEditor",
                "OdysseyImaging",
                "OdysseyPaintEngine",
                "OdysseyPainterEditor",
                "OdysseyStrokePipeline",
                "OdysseyLayer",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyTexture",
                "OdysseyTransaction",
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Blueprint"),
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "TextureEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public", "Blueprint"),
                Path.Combine(ModuleDirectory, "Public", "Settings"),
                Path.Combine(ModuleDirectory, "Public", "TextureEditor"),
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyPainterEditor",
                "OdysseyStyle",
                "OdysseyTexture",
                "ULIS",
                "ULISLoader"
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
