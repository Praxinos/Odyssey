// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class OdysseyViewportDrawingEditor : ModuleRules
{
    public OdysseyViewportDrawingEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "MeshPaint",
                "ULIS",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "DesktopPlatform",
                "Engine",
                "EditorFramework",
                "Projects",
                "InputCore",
                "RenderCore",
                "RHI",
                "Slate",
                "SlateCore",
                "Sequencer",
                "EditorStyle",
                "UnrealEd",
                "RawMesh",
                "SourceControl",
                "ViewportInteraction",
                "VREditor",
                "ToolMenus",
                "PropertyEditor",
                "MainFrame",
                "MeshPaint",
                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyMeshPaintRendering",
                "OdysseyWidgets",
                "OdysseyLayer",
                "OdysseyLayerStack",
                "OdysseyEditor",
                "OdysseyPainterEditor",
                "OdysseyTexture",
                "OdysseyTextureEditor",
                "OdysseyImaging",
                "OdysseyPaintEngine",
                "OdysseyStylusInput",
                "OdysseyStyle"
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "ViewportDrawingEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
                Path.Combine(ModuleDirectory, "Private", "ModeToolbar"),
                Path.Combine(ModuleDirectory, "Private", "Painter"),

            }
        );

        PrivateIncludePathModuleNames.AddRange(
			new string[]
			{
				"AssetTools",
				"LevelEditor"
            });

		DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools"
            }
        );
    }
}
