// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class EposTracksEditor : ModuleRules
{
    public EposTracksEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
                // [RUNTIME] custom plugin dependencies
                "EposMovieScene",
                "EposSequence",
                "EposNote",
                "EposTracks",
                "EposActors",
                "EposPattern",

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
                "EposNamingConvention"
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
