// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyPainterEditor : ModuleRules
{
    public OdysseyPainterEditor(ReadOnlyTargetRules Target) : base(Target)
    {
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
                "Slate",
                "SlateCore",
                "UnrealEd",
                "OdysseyCore",
                "InputCore",
                "Engine",
                "RHI",
                "RenderCore",
                "RawMesh",
                "EditorStyle",
                "PropertyEditor",
                "OdysseyStyle",
                "OdysseyBrush",
                "OdysseyWidgets",
                "OdysseyImaging",
                "OdysseyPaintEngine",
                "OdysseyStrokePipeline",
                "OdysseyLayer",
                "OdysseyTransaction",
                "OdysseyBase",
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                "Odyssey/OdysseyEditor/OdysseyPainterEditor/Private",
                "Odyssey/OdysseyEditor/OdysseyPainterEditor/Private/Models",
                "Editor/PackagesDialog/Public",
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
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

    }
}
