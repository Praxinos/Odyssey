// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

using System.IO;
using UnrealBuildTool;

public class OdysseyFlipbookEditor : ModuleRules
{
    public OdysseyFlipbookEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
                "InputCore",
                "Engine",
                "RHI",
                "RenderCore",
                "RawMesh",
                "EditorStyle",
                "PropertyEditor",
                "Projects",
                "OdysseyStyle",
                "OdysseyBrush",
                "OdysseyWidgets",
                "OdysseyImaging",
                "OdysseyPaintEngine",
                "OdysseyPainterEditor",
                "OdysseyStrokePipeline",
                "OdysseyLayer",
                "OdysseyTransaction",
                "OdysseyStylusInput",
                "OdysseyFlipbook",
                "OdysseyTexture",
                "Paper2D",
                "Paper2DEditor",
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Private", "Extensions"),
                Path.Combine(ModuleDirectory, "Private", "Models"),
                Path.Combine(ModuleDirectory, "Private", "FlipbookEditor"),
                Path.Combine(ModuleDirectory, "Private", "Settings"),
                Path.Combine(ModuleDirectory, "Private", "Widgets"),
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                Path.Combine(ModuleDirectory, "Public"),
                Path.Combine(ModuleDirectory, "Public", "Settings"),
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader"
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
