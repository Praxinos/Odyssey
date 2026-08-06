// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyLayerStack : ModuleRules
{
    public OdysseyLayerStack(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
                "OdysseyMedia",
                "OdysseyRendering",
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
                "Engine"
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "CoreUObject",
                "ImageCore",
                "RenderCore",
                "Renderer",
                "RHI",
                "TypedElementRuntime",
                "OdysseyCore",
                "OdysseyMedia",
                "OdysseyRendering",
            }
        );
    }
}
