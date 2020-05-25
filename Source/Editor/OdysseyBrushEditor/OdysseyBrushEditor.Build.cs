// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyBrushEditor : ModuleRules
{
    public OdysseyBrushEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "AssetRegistry",
                "AssetTools",
                "BlueprintRuntime",
                "ClassViewer",
                "Analytics",
                "DerivedDataCache",
                "LevelEditor",
                "GameProjectGeneration",
                "Sequencer",
            }
            );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "ApplicationCore",
                "Slate",
                "SlateCore",
                "EditorStyle",
                "EditorWidgets",
                "Engine",
                "Json",
                "Merge",
                "MessageLog",
                "UnrealEd",
                "GraphEditor",
                "Kismet",
                "KismetWidgets",
                "KismetCompiler",
                "BlueprintGraph",
                "AnimGraph",
                "PropertyEditor",
                "SourceControl",
                "SharedSettingsWidgets",
                "InputCore",
                "EngineSettings",
                "Projects",
                "JsonUtilities",
                "DerivedDataCache",
                "DesktopPlatform",
                "HotReload",
                "BlueprintNativeCodeGen",
                "UMG", // for SBlueprintDiff
                "WorkspaceMenuStructure",
                "OdysseyBrush",
                "ULIS",
                "ULISLoader",
            }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
                "BlueprintRuntime",
                "ClassViewer",
                "Documentation",
                "GameProjectGeneration",
                "BlueprintCompilerCppBackend",
            }
            );
    }
}
