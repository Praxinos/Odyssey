// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyViewportDrawingEditor : ModuleRules
{
    public OdysseyViewportDrawingEditor(ReadOnlyTargetRules Target) : base(Target)
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

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "MeshPaint",
                "ULIS",
                "ULISLoader",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "ContentBrowser",
                "Core",
                "CoreUObject",
                "DesktopPlatform",
                "Engine",
                "EditorFramework",
                "Projects",
                "InputCore",
                "RenderCore",
                "RHI",
                "Slate",
                "SlateCore",
                "Sequencer",
                "EditorStyle",
                "UnrealEd",
                "RawMesh",
                "SourceControl",
                "ViewportInteraction",
                "VREditor",
                "ToolMenus",
                "PropertyEditor",
                "MainFrame",
                "MeshPaint",
                "MediaAssets",
                "MediaPlate",
                "MovieScene",
                "OdysseyAnimation",
                "OdysseyAnimationEditor",
                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyWidgets",
                "OdysseyHUD",
                "OdysseyEditor",
                "OdysseyPainterEditor",
                "OdysseyTextureEditor",
                "OdysseyImaging",
                "OdysseyStylusInput",
                "OdysseyStyle",
                "OdysseyAnimationTracks",
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "ViewportDrawingEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
                Path.Combine(ModuleDirectory, "Private", "Painter"),
                Path.Combine(ModuleDirectory, "Private", "Utils"),
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[]
            {
                "AssetTools",
                "LevelEditor"
            });

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools"
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
