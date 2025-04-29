// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
            if (Target.Configuration == UnrealTargetConfiguration.Debug)
            {
                bMergeUnityFiles = false;
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
                    "blend2d",
                    "ULIS",
                    "ULISLoader",
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
                    "OdysseyCore",
                    "OdysseyMaths",
                    "RenderCore",
                    "Slate",
                    "SlateCore",
                    "RHI", //To have access to GPixelFormats
                    // ... add private dependencies that you statically link with here ...

                    "OdysseyAnimation", //TODO: MUST BE the future runtime version !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                    "OdysseyImaging", //TODO: required with OdysseyAnimation otherwise in OdysseyAnimation: OdysseyAnimation.h(9): fatal error C1083: Cannot open include file: 'OdysseyRenderingAbility.h': No such file or directory
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    // ... add any modules that your module loads dynamically here ...
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
}
