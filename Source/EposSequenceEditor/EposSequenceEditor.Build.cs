// IDDN.FR.001.220036.002.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using EpicGames.Core;
using System.IO;
using UnrealBuildBase;
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
                Path.Combine(Unreal.EngineDirectory.FullName, "Source/Editor/UnrealEd/Private"), // TODO: Fix this, for now it's needed for the fbx exporter
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
                "AppFramework",
                "DeveloperSettings",
                "Settings", // Developer
                "LevelSequence",
                "TimeManagement",
                "MovieSceneCapture",
                "MovieRenderPipelineCore",
                "ImageWrapper",
                "ImageWriteQueue",
                "RenderCore",
                "RHI",
                "Niagara",
                "Paper2D",
                // [RUNTIME] custom plugin dependencies
                "EposMovieScene",
                "EposSequence",
                "EposTracks",
                "EposActors",
                "EposNote",
                "EposPattern",

                // [EDITOR]
                // [EDITOR] custom engine dependencies
                "MovieSceneTools",
                "SequencerCore",
                "Sequencer",
                "UnrealEd",
                "LevelEditor",
                "EditorStyle",
                "MovieSceneCaptureDialog",
                "EditorWidgets",
                "EditorFramework",
                "ToolWidgets",
                "EditorScriptingUtilities",
                "MovieRenderPipelineEditor",
                "LevelSequenceEditor",
                "AssetDefinition",
                "UMG",
                "SequencerScripting",
                "CurveEditor",
                "LevelSequenceEditor",
                "EditorSubsystem",
                // [EDITOR] custom plugin dependencies
                "EposTracksEditor",
                "EposNamingConvention",
                "libharu",
            }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[]
            {
                // ... add any modules that your module loads dynamically here ...
            }
            );

        AddEngineThirdPartyPrivateStaticDependencies(Target,
            "FBX"
        );

    }
}
