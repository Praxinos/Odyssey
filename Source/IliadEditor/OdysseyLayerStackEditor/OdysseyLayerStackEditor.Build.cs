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
                // ... add public include paths required here ...
            }
            );

        PrivateIncludePaths.AddRange(
            new string[] {
                // ... add other private include paths required here ...
            }
            );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public", "Animation"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Cells"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Cells", "CellImageRaster"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Cells", "CellImageStagger"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Cells", "CellImageVector"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Layers"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Layers", "LayerFolder"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Layers", "LayerImageRaster"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Layers", "LayerImageVector"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "Layers", "LayerRoot"),
                Path.Combine(ModuleDirectory, "Public", "Animation", "LightTable"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
                Path.Combine(ModuleDirectory, "Public", "Commands"),
                Path.Combine(ModuleDirectory, "Public", "Controllers"),
                Path.Combine(ModuleDirectory, "Public", "DragDropOperations"),
                Path.Combine(ModuleDirectory, "Public", "Shortcuts"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Animation"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Cells"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Cells", "CellImageRaster"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Cells", "CellImageStagger"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Cells", "CellImageVector"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Layers"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Layers", "LayerFolder"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Layers", "LayerImageRaster"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Layers", "LayerImageVector"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "Layers", "LayerRoot"),
                Path.Combine(ModuleDirectory, "Private", "Animation", "LightTable"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
                Path.Combine(ModuleDirectory, "Private", "Commands"),
                Path.Combine(ModuleDirectory, "Private", "Controllers"),
                Path.Combine(ModuleDirectory, "Private", "DragDropOperations"),
                Path.Combine(ModuleDirectory, "Private", "Shortcuts"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Slate",
                "OdysseyEditor",
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
                "SlateCore",
                "ToolMenus",
                "ToolWidgets",
                "UnrealEd",

                "OdysseyAnimation",
                "OdysseyCore",
                "OdysseyCoreEditor",
                "OdysseyFile",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyMedia",
                "OdysseyRendering",
                "OdysseyStyle",
                "OdysseyVector",

                "blend2d",
                "ULIS",
                "ULISLoader",
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
