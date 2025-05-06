// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;
using UnrealBuildTool;

public class EposTracksEditor : ModuleRules
{
    public EposTracksEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "Core",
                // ... add other public dependencies that you statically link with here ...
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[]
            {
                // [RUNTIME]
                "CoreUObject",
                "Engine",
                "Slate",
                "SlateCore",
                // [RUNTIME] custom engine dependencies
                "MovieScene",
                "InputCore",
                "RenderCore",
                "MovieSceneTracks",
                "Projects",
                "CinematicCamera",
                "AppFramework",
                "DeveloperSettings",
                "LevelSequence",
                "MediaAssets", //TODO: remove once OdysseyAnimation is runtime
                // [RUNTIME] custom plugin dependencies
                "EposMovieScene",
                "EposSequence",
                "EposNote",
                "EposTracks",
                "EposActors",
                "EposPattern",
                "OdysseyAnimationTracks",
                "ULIS",
                "ULISLoader",

                // [EDITOR]
                // [EDITOR] custom engine dependencies
                "SequencerCore",
                "Sequencer",
                "MovieSceneTools",
                "UnrealEd",
                "EditorStyle",
                "EditorWidgets",
                "SceneOutliner",
                "MaterialEditor",
                "EditorScriptingUtilities",
                "LevelEditor",
                // [EDITOR] custom plugin dependencies
                "EposNamingConvention",
                "OdysseyAnimation", //TODO: move to runtime part above
                "OdysseyImaging", //TODO: remove once OdysseyAnimation is runtime
                "OdysseyLayerStackEditor", //TODO: remove it ? ot keep it ? (will be layer stack full editor ?)
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
