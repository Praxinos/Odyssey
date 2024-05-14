// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;
using UnrealBuildTool;

public class EposSequence : ModuleRules
{
    public EposSequence(ReadOnlyTargetRules Target) : base(Target)
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
                "LevelSequence",
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
                "MovieSceneTracks",
                "CinematicCamera",
                "MovieRenderPipelineCore",
                "MediaCompositing",
                "Niagara",
                "Paper2D",
                "MediaPlate",
                "UniversalObjectLocator",
                // [RUNTIME] custom plugin dependencies
                "EposMovieScene",
                "EposTracks",
                "EposActors",
                "EposNote",
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
