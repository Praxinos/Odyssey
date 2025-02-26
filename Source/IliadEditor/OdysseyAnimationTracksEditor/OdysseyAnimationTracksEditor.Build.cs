// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyAnimationTracksEditor : ModuleRules
    {
        public OdysseyAnimationTracksEditor(ReadOnlyTargetRules Target) : base(Target)
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
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "ULIS",
                    "ULISLoader",
                }
                );

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "OdysseyAnimation",
                    "OdysseyAnimationTracks",
                    "OdysseyImaging",
                    "OdysseyLayerStackEditor",
                    "OdysseyPainterEditor",
                    "OdysseyStyle",
                    "OdysseyViewportDrawingEditor",

                    "Core",
                    "CoreUObject",
                    "Engine",
                    "InputCore",
                    "LevelSequence",
                    "MediaAssets",
                    "MovieScene",
                    "MovieSceneTools",
                    "Sequencer",
                    "SequencerCore",
                    "Slate",
                    "SlateCore",
                    "UnrealEd"
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
