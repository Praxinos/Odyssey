// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyAnimationTracks : ModuleRules
    {
        public OdysseyAnimationTracks(ReadOnlyTargetRules Target) : base(Target)
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
                    // ... add public include paths required here ...
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    // ... add other private include paths required here ...
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    // ... add other public dependencies that you statically link with here ...
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "Media",
                    "MediaAssets",
                    "MovieScene",
                    "OdysseyAnimation",
                    "OdysseyLayerStack",
                    "OdysseyRendering", //TODO: required with OdysseyAnimation otherwise in OdysseyAnimation: OdysseyAnimation.h(9): fatal error C1083: Cannot open include file: 'OdysseyImageRenderingAbility.h': No such file or directory
                    "OdysseyCore",
                    "RenderCore",
                    "Slate",
                    "SlateCore",
                    "RHI", //To have access to GPixelFormats
                    // ... add private dependencies that you statically link with here ...
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    // ... add any modules that your module loads dynamically here ...
                }
                );
        }
    }
}
