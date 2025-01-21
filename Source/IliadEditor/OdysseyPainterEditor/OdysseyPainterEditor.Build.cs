// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyPainterEditor : ModuleRules
{
    public OdysseyPainterEditor(ReadOnlyTargetRules Target) : base(Target)
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
                "Settings",
                "PropertyEditor",
                "AssetTools",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "blend2d",
                "AppFramework",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "UnrealEd",
                "InputCore",
                "LevelEditor",
                "Engine",
                "RHI",
                "RenderCore",
                "RawMesh",
                "ToolMenus",
                "EditorStyle",
                "EditorFramework",
                "EditorWidgets",
                "PropertyEditor",
                "Projects",
                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyHUDSystem",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyLayerStackEditor",
                "OdysseyStyle",
                "OdysseyStylusInput",
                "OdysseyVector",
                "OdysseyWidgets"
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "OdysseyPaintEngine",
                "OdysseyShapes",
                "OdysseyVector",
                "OdysseyPalette",
                "OdysseyMedia",
                "OdysseyHUDSystem",
                "ULIS",
                "ULISLoader"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
                Path.Combine(ModuleDirectory, "Private", "Tools"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "PainterEditor"),
                Path.Combine(ModuleDirectory, "Public", "Settings"),
                Path.Combine(ModuleDirectory, "Public", "Tools"),
            }
        );

        DynamicallyLoadedModuleNames.AddRange(
             new string[] {
                "MainFrame",
                "WorkspaceMenuStructure",
                "AssetTools",
                "PackagesDialog"
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
