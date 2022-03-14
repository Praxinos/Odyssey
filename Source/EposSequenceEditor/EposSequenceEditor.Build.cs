// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;
using UnrealBuildTool;

public class EposSequenceEditor : ModuleRules
{
    public EposSequenceEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "InputCore",
                "MovieScene",
                "MovieSceneTracks",
                "Projects",
                "CinematicCamera",
                "AppFrameWork",
                "DeveloperSettings",
                "Settings", // Developer
                "LevelSequence",
                "TimeManagement",
                "MovieSceneCapture",
                // [RUNTIME] custom plugin dependencies
                "EposMovieScene",
                "EposSequence",
                "EposTracks",
                "EposActors",
                "EposNote",

                // [EDITOR]
                // [EDITOR] custom engine dependencies
                "MovieSceneTools",
                "Sequencer",
                "UnrealEd",
                "LevelEditor",
                "EditorStyle",
                "MovieSceneCaptureDialog",
                "EditorWidgets",
                "EditorScriptingUtilities",
                "MovieRenderPipelineEditor",
                "MovieRenderPipelineCore",
                // [EDITOR] custom plugin dependencies
                "EposTracksEditor",
                "EposNamingConvention",
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
