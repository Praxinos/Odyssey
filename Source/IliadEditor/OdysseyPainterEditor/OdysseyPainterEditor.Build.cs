// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyPainterEditor : ModuleRules
{
    public OdysseyPainterEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "RawMesh",
                "Slate",
                "SlateCore",
                "Sequencer",
                "ToolMenus",
                "ToolWidgets",
                "UnrealEd",
                "ViewportInteraction",

                "OdysseyAnimation",
                "OdysseyAnimationTracks",
                "OdysseyCore",
                "OdysseyHUD",
                "OdysseyImaging",
                "OdysseyLayerStackEditor",
                "OdysseyMaths",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyTexture",
                "OdysseyVector",
                "OdysseyWidgets",

                "blend2d"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyHUD",
                "OdysseyMedia",
                "OdysseyPalette",
                "OdysseyVector",

                "ULIS",
                "ULISLoader"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Animation"),
                Path.Combine(ModuleDirectory, "Private", "BrushContext"),
                Path.Combine(ModuleDirectory, "Private", "Flipbook"),
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "PaintEngine"),
                Path.Combine(ModuleDirectory, "Private", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
                Path.Combine(ModuleDirectory, "Private", "Shapes"),
                Path.Combine(ModuleDirectory, "Private", "Shortcuts"),
                Path.Combine(ModuleDirectory, "Private", "StandaloneEditor"),
                Path.Combine(ModuleDirectory, "Private", "Texture"),
                Path.Combine(ModuleDirectory, "Private", "Tools"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Animation"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Brush"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Color"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Flipbook"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Palette"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Tab"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Texture"),
                Path.Combine(ModuleDirectory, "Private", "Widgets", "Tools"),
                Path.Combine(ModuleDirectory, "Private", "WorldCentricEditor"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "Animation"),
                Path.Combine(ModuleDirectory, "Public", "BrushContext"),
                Path.Combine(ModuleDirectory, "Public", "Models"),
                Path.Combine(ModuleDirectory, "Public", "PaintEngine"),
                Path.Combine(ModuleDirectory, "Public", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Public", "Settings"),
                Path.Combine(ModuleDirectory, "Public", "Shapes"),
                Path.Combine(ModuleDirectory, "Public", "Texture"),
                Path.Combine(ModuleDirectory, "Public", "Tools"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
                Path.Combine(ModuleDirectory, "Public", "Widgets", "Animation"),
                Path.Combine(ModuleDirectory, "Public", "Widgets", "Tab"),
                Path.Combine(ModuleDirectory, "Public", "WorldCentricEditor"),
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

        //--- WIBU

        string enable_wibu_encryption = Environment.GetEnvironmentVariable("ENABLE_WIBU_ENCRYPTION");
        if( enable_wibu_encryption != null )
        {
            PCHUsage = PCHUsageMode.NoPCHs;
            PublicDefinitions.Add("USE_WIBU_CTP");

            PublicAdditionalLibraries.Add("/usr/local/lib/libcpsrt.dylib");
        }
    }
}
