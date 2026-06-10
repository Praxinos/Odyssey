// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

        /**
        * Sometimes Unreal can compile Editor modules even if it builds Game only modules
        * It happens when an editor module is defined as a dependency in a non editor module
        * and it is not encapsulated with if (Target.Type == TargetType.Editor)
        * So we make sure here to throw an error if this module is used in a game compilation
        *
        * If you hit this assert, search for this module being a non editor module's dependency.
        * It could also be an indirect dependency.
        */
        if (Target.Type == TargetType.Game)
        {
            throw new InvalidOperationException("ERROR in EposSequenceEditor Module : Target.Type == TargetType.Game");
        }

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

        if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
        {
            // To know when the plugin is build in DebugGame mode
            // Not intended to be overused, it's just to be able to display a note in the sequencer toolbar
            //
            // It has no value (=XX) to not have to also set it in the "else" part
            // And so, in code, use #ifdef
            PrivateDefinitions.Add("ODC_DEBUGGAME");
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
                "OdysseyAnimationTracks",
                "OdysseyAnimation",
                "OdysseyLayerStack",

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
                "OdysseyAnimationEditor",
                "OdysseyPainterEditor",
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
