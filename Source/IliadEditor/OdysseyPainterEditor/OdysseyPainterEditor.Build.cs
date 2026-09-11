// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyPainterEditor : ModuleRules
{
    public OdysseyPainterEditor(ReadOnlyTargetRules Target) : base(Target)
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
            throw new InvalidOperationException("ERROR in OdysseyPainterEditor Module : Target.Type == TargetType.Game");
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

        bAddDefaultIncludePaths = true;

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
                "PropertyEditor",
                "AssetTools",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "Engine",
                "EditorStyle",
                "EditorFramework",
                "EditorWidgets",
                "InputCore",
                "Json",
                "LevelEditor",
                "MediaAssets",
                "MediaPlate",
                "MeshPaint",
                "MovieScene",
                "Paper2D",
                "Paper2DEditor",
                "PropertyEditor",
                "RHI",
                "RenderCore",
                "Renderer",
                "RawMesh",
                "Slate",
                "SlateCore",
                "Sequencer",
                "ToolMenus",
                "ToolWidgets",
                "UnrealEd",
                "ViewportInteraction",
                "NamingTokens",

                "OdysseyAnimation",
                "OdysseyAnimationTracks",
                "OdysseyCore",
                "OdysseyCommonWidgets",
                "OdysseyHUD",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyLayerStackEditor",
                "OdysseyRendering",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyTexture",
                "OdysseyVector",
                "OdysseyWidgets",
                "OdysseyTelemetry",

                "blend2d"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyBrush",
                "OdysseyCoreEditor",
                "OdysseyEditor",
                "OdysseyHUD",
                "OdysseyMedia",
                "OdysseyPalette",
                "OdysseyPaletteEditor",
                "OdysseyRendering",
                "OdysseyVector",

                "ULIS",
                "ULISLoader"
             }
        );

        PrivateIncludePaths.AddRange(
            Directory.GetDirectories(ModuleDirectory, "Private/*", SearchOption.AllDirectories)
        );

        PublicIncludePaths.AddRange(
            Directory.GetDirectories(ModuleDirectory, "Public/*", SearchOption.AllDirectories)
        );

        DynamicallyLoadedModuleNames.AddRange(
             new string[] {
                "MainFrame",
                "WorkspaceMenuStructure",
                "AssetTools",
                "PackagesDialog"
             }
        );
    }
}
