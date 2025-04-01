// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using EpicGames.Core;
using System;
using System.IO;
using UnrealBuildBase;
using UnrealBuildTool;

public class EposSequenceEditor : ModuleRules
{
    public EposSequenceEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "UniversalObjectLocator", //PATCH: used for UEposSequenceEditorBlueprintLibrary::LocateBoundObjectsPATCH542() until the UMovieSceneSequenceExtensions::LocateBoundObjects() is patch in >=5.4.3, then remove it
                "ApplicationCore",
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
                "SequencerScriptingEditor",
                "CurveEditor",
                "LevelSequenceEditor",
                "EditorSubsystem",
                "PropertyEditor",
                "SceneOutliner",
                "ActorPickerMode",
                "ToolMenus",
                "ControlRigEditor",
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

        //--- WIBU

        /*
        PCHUsage = PCHUsageMode.NoPCHs;

        if (Target.Platform.IsInGroup(UnrealPlatformGroup.Windows))
        {
            //string AxProtectorSDKPath = Environment.GetEnvironmentVariable("AXPROTECTOR_SDK");

            //PublicSystemIncludePaths.Add(Path.Combine(AxProtectorSDKPath, "bin", "ctp", "pass", "include"));
        }
        else if (Target.Platform == UnrealTargetPlatform.Mac)
        {
            //string AxProtectorSDKPath = "/Applications/WIBU-SYSTEMS Devkit/AxProtector";

            //PublicSystemIncludePaths.Add(Path.Combine(AxProtectorSDKPath, "ctp", "pass", "include"));

            PublicDefinitions.Add("USE_WIBU_CTP");
        }

        string pathfile_to_protection_specification = Path.Combine(PluginDirectory, "Wibu", "ProtectionSpecification.yaml");
        // WIBU_CTP_YAML_PATH is only valid directly on command line or in environment variable
        // As compilation is done via .rsp, we can't use PublicDefinitions because it adds WIBU_CTP_YAML_PATH in the .rsp
        // So we must use the environment variable
        // (See Wibu Ticket T-165355)
        Environment.SetEnvironmentVariable("WIBU_CTP_YAML_PATH", pathfile_to_protection_specification);
        //PublicDefinitions.Add($"WIBU_CTP_YAML_PATH=\"{pathfile_to_protection_specification}\"");
        */

        string enable_wibu_encryption = Environment.GetEnvironmentVariable("ENABLE_WIBU_ENCRYPTION");
        if( enable_wibu_encryption != null )
        {
            PCHUsage = PCHUsageMode.NoPCHs;
            PublicDefinitions.Add("USE_WIBU_CTP");

            // Should be used in CLangToolChain.cs, but doesn't work: "-l/usr/local/lib/libcpsrt.dylib: 'linker' input unused [-Werror,-Wunused-command-line-argument]"
            // Post to UDN soon
            PublicAdditionalLibraries.Add("/usr/local/lib/libcpsrt.dylib");
        }

        // For XCode -> To generate environment variable at build time:
        // Right click the project on the left
        // Add new config file
        // Write the environment variables inside said config file
        // Left click on project, info, Configurations -> Add config file to wanted deployment target
    }
}
