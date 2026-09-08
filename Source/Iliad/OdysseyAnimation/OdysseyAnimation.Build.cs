// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyAnimation : ModuleRules
    {
        public OdysseyAnimation(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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

            //bUseRTTI = true;

            PublicIncludePaths.AddRange(
                new string[] {
                    // ... add public include paths required here ...
                    Path.Combine(ModuleDirectory, "Public", "LayerStack"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells", "CellImageRaster"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Cells", "CellImageVector"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerFolder"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerImageRaster"),
                    Path.Combine(ModuleDirectory, "Public", "LayerStack", "Layers", "LayerImageVector"),
                    Path.Combine(ModuleDirectory, "Public", "Media"),
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    // ... add other private include paths required here ...
                    Path.Combine(ModuleDirectory, "Private", "LayerStack"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells", "CellImageRaster"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Cells", "CellImageVector"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerFolder"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerImageRaster"),
                    Path.Combine(ModuleDirectory, "Private", "LayerStack", "Layers", "LayerImageVector"),
                    Path.Combine(ModuleDirectory, "Private", "Media"),
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "MediaAssets",
                    "OdysseyRendering",
                    "OdysseyPalette",
                    // ... add other public dependencies that you statically link with here ...
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Media",
                    "RenderCore",
                    "MovieScene",
                    "RHI",
                    "CinematicCamera",

                    "OdysseyCore",
                    "OdysseyRendering",
                    "OdysseyLayerStack",
                    "NamingTokens",
                    "OdysseyTelemetry",
                    // ... add private dependencies that you statically link with here ...
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    // ... add any modules that your module loads dynamically here ...
                }
                );

            //--- Editor Only Dependencies
            // This is here because Animation LayerStack depends on ULIS and OdysseyVector
            // This can be removed once layers don't use ULIS anymore and OdysseyVector is Runtime Ready

            if (Target.Type == TargetType.Editor)
            {
                PrivateDependencyModuleNames.AddRange(
                    new string[] {
                        "EditorStyle",
                        "Renderer",
                        "SlateCore",
                        "UnrealEd",

                        "OdysseyCoreEditor",
                        "OdysseyImaging",
                        "OdysseyFile",
                        "OdysseyMedia",
                        "OdysseyStyle",
                        "OdysseyVector",

                        "ULIS",
                        "ULISLoader"
                    }
                );
            }
        }
    }
}
