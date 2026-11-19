// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyAnimationEditor : ModuleRules
    {
        public OdysseyAnimationEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            /**
            * Sometimes Unreal can compile Editor modules even if it builds Game only modules
            * It happens when an editor module is defined as a dependency in a non editor module
            * and it is not encapsulated with if (Target.Type == TargetType.Editor)
            * So we make sure here to throw an error if this module is used in a game compilation
            *
            * If you hit this assert, search for this module being a non editor module's dependency.
            * It could also be an indirect dependency.
            */
            if (Target.Type == TargetType.Game)
            {
                throw new InvalidOperationException("ERROR in OdysseyAnimationEditor Module : Target.Type == TargetType.Game");
            }

            //Inactivate Unity builds to force devs to include all the necessary include files overywhere it is needed
            //Inactivate Code Optimization in Debug configurations
            //Because Engine modules and Engine Plugin Modules are always optimized by default
            //and we don't want that
            if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
            {
                bMergeUnityFiles = false;
                PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
                bUseUnity = false;
                OptimizeCode = CodeOptimization.Never;
            }

            /** Animation LayerStack data (layers, cells, lighttables) is editor only and lives in this module. */
            PublicIncludePaths.AddRange(
                new string[]
                {
                    Path.Combine(ModuleDirectory, "Public", "LayerStack"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells", "CellImageRaster"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells", "CellImageStagger"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells", "CellImageVector"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerFolder"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerImageRaster"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerImageVector"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Lighttable"),
                }
                );

            PrivateIncludePaths.AddRange(
                new string[]
                {
                    Path.Combine(ModuleDirectory, "Private", "LayerStack"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells", "CellImageRaster"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells", "CellImageStagger"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells", "CellImageVector"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerFolder"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerImageRaster"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerImageVector"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Lighttable"),
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "MediaAssets",
                    "OdysseyAnimation",
                    "OdysseyLayerStack",
                    "OdysseyPalette",
                    "OdysseyRendering",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "AssetTools",
                    "AppFramework",
                    "Core",
                    "CoreUObject",
                    "DesktopPlatform",
                    "DataValidation",
                    "DeveloperSettings",
                    "EditorStyle",
                    "EditorWidgets",
                    "Engine",
                    "InputCore",
                    "LevelSequence",
                    "ToolMenus",
                    "Media",
                    "MediaAssets",
                    "MovieScene",
                    "MovieSceneTools",
                    "Paper2D",
                    "Paper2DEditor",
                    "PropertyEditor",
                    "RenderCore",
                    "RHI",
                    "Slate",
                    "SlateCore",
                    "Sequencer",
                    "SequencerCore",
                    "ToolWidgets",
                    "TypedElementRuntime",
                    "UnrealEd",
                    "EditorFramework",

                    "OdysseyBrush",
                    "OdysseyCore",
                    "OdysseyCoreEditor",
                    "OdysseyEditor",
                    "OdysseyFile",
                    "OdysseyHUD",
                    "OdysseyImaging",
                    "OdysseyMedia",
                    "OdysseyStyle",
                    "OdysseyVector",

                    "ULIS",
                    "ULISLoader",
                }
                );

            /** Painter is opened through its virtual module interface and links back to animation editing APIs. */
            PrivateIncludePathModuleNames.Add("OdysseyPainterEditor");

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    "OdysseyPainterEditor",
                }
                );
        }
    }
}
