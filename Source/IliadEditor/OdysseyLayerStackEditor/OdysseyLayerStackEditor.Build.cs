// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyLayerStackEditor : ModuleRules
{
    public OdysseyLayerStackEditor(ReadOnlyTargetRules Target) : base(Target)
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
                Path.Combine(ModuleDirectory, "Public", "Commands"),
                Path.Combine(ModuleDirectory, "Public", "Controllers"),
                Path.Combine(ModuleDirectory, "Public", "DragDropOperations"),
                Path.Combine(ModuleDirectory, "Public", "Shortcuts"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Commands"),
                Path.Combine(ModuleDirectory, "Private", "Controllers"),
                Path.Combine(ModuleDirectory, "Private", "DragDropOperations"),
                Path.Combine(ModuleDirectory, "Private", "Shortcuts"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "OdysseyCore",
                "OdysseyMedia",
                "OdysseyImaging",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "EditorStyle",
                "EditorWidgets",
                "Engine",
                "InputCore",
                "RHI",
                "RenderCore",
                "Slate",
                "SlateCore",
                "UMG",
                "UnrealEd",
                "ToolMenus",
                "ToolWidgets",

                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyMaths",
                "OdysseyStyle",

                "ULIS",
                "ULISLoader"
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
