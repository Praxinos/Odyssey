// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class Ariane : ModuleRules
{
    public Ariane(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Components"),
                Path.Combine(ModuleDirectory, "Private", "Actors"),
                Path.Combine(ModuleDirectory, "Private", "Geometry"),
                Path.Combine(ModuleDirectory, "Private", "LayerStack"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "Components"),
                Path.Combine(ModuleDirectory, "Public", "Actors"),
                Path.Combine(ModuleDirectory, "Public", "Geometry"),
                Path.Combine(ModuleDirectory, "Public", "LayerStack"),
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
                //"OdysseyMedia",
                //"OdysseyRendering",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "SlateCore",
                "Engine",

                "OdysseyPalette",
                "ULIS",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject",
                "RenderCore",
                "Renderer",
                "RHI",
                "TypedElementRuntime",

                "OdysseyPalette",
                //"OdysseyMedia",
                //"OdysseyRendering",
            }
        );
    }
}
