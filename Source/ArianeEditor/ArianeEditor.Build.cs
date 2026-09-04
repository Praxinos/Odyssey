// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class ArianeEditor : ModuleRules
{
    public ArianeEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        //Inactivate Unity builds to force devs to include all the necessary include files overywhere it is needed
        //Inactivate Code Optimization in Debug configurations
        //Because Engine modules and Engine Plugin Modules are always optimized by default
        //and we don't want that
        if (Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            bMergeUnityFiles = false;
            bUseUnity = false;
            OptimizeCode = CodeOptimization.Never;
        }

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
                "Engine",
                "EditorStyle",
                "EditorFramework",
                "EditorWidgets",
                "InputCore",
                "Json",
                "LevelEditor",
                "MediaAssets",
                "MediaPlate",
                "MeshPaint",
                "MovieScene",
                "Paper2D",
                "Paper2DEditor",
                "PropertyEditor",
                "Projects",
                "RHI",
                "RenderCore",
                "Renderer",
                "RawMesh",
                "Slate",
                "SlateCore",
                "StylusInput",
                "Sequencer",
                "ToolMenus",
                "ToolWidgets",
                "UnrealEd",
                "ViewportInteraction",
                "Sequencer",
                "LevelSequence",
                "MovieScene",
                "MovieSceneTracks",
                "MovieSceneTools", // FKeyframeTrackEditor
                "InteractiveToolsFramework",
                //"OdysseyAnimation",
                //"OdysseyAnimationTracks",
                //"OdysseyCore",
                //"OdysseyCommonWidgets",
                //"OdysseyHUD",
                "OdysseyImaging",
                //"OdysseyLayerStack",
                //"OdysseyLayerStackEditor",
                "OdysseyPainterEditor",
                //"OdysseyRendering",
                "Ariane",
                "OdysseyStyle",
                "OdysseyStylusInput",
                //"OdysseyTexture",
                //"OdysseyVector",
                "OdysseyWidgets",

                //"blend2d"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                //"OdysseyBrush",
                "OdysseyCoreEditor",
                //"OdysseyEditor",
                //"OdysseyHUD",
                "OdysseyImaging",
                //"OdysseyMedia",
                "OdysseyPalette",
                "OdysseyPainterEditor",
                //"OdysseyPaletteEditor",
                //"OdysseyRendering",
                //"OdysseyVector",
                "OdysseyWidgets",
                "Ariane",
                "OdysseyStyle",
                "StylusInput",
                "ApplicationCore",
                "ULIS",
                //"ULISLoader"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                //Path.Combine(ModuleDirectory, "Private", "Animation"),
                //Path.Combine(ModuleDirectory, "Private", "Flipbook"),
                //Path.Combine(ModuleDirectory, "Private", "Layouts"),
                //Path.Combine(ModuleDirectory, "Private", "Shapes"),
                //Path.Combine(ModuleDirectory, "Private", "Shortcuts"),
                //Path.Combine(ModuleDirectory, "Private", "Sources"),
                //Path.Combine(ModuleDirectory, "Private", "Tabs"),
                //Path.Combine(ModuleDirectory, "Private", "Tabs", "VectorSceneTreeView"),
                //Path.Combine(ModuleDirectory, "Private", "Tools"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Animation"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Brush"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Color"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Flipbook"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Palette"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Tab"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Texture"),
                //Path.Combine(ModuleDirectory, "Private", "Widgets", "Tools"),
                Path.Combine(ModuleDirectory, "Private", "Tabs"),
                Path.Combine(ModuleDirectory, "Private", "Tools"),
                Path.Combine(ModuleDirectory, "Private", "Styles"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
                Path.Combine(ModuleDirectory, "Private", "Animation"),
                Path.Combine(ModuleDirectory, "Private", "Customizations"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                //Path.Combine(ModuleDirectory, "Public", "Animation"),
                //Path.Combine(ModuleDirectory, "Public", "Sources"),
                //Path.Combine(ModuleDirectory, "Public", "Tools"),
                //Path.Combine(ModuleDirectory, "Public", "Tools", "OutOfPegsTool"),
                //Path.Combine(ModuleDirectory, "Public", "Widgets"),
                //Path.Combine(ModuleDirectory, "Public", "Widgets", "Animation"),
                //Path.Combine(ModuleDirectory, "Public", "Widgets", "Animation", "Timeline"),
                Path.Combine(ModuleDirectory, "Public", "WorldCentricEditor"),
                Path.Combine(ModuleDirectory, "Public", "Tabs"),
                Path.Combine(ModuleDirectory, "Public", "Tools"),
                Path.Combine(ModuleDirectory, "Public", "Styles"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
                Path.Combine(ModuleDirectory, "Public", "Animation"),
                Path.Combine(ModuleDirectory, "Public", "Customizations"),
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
