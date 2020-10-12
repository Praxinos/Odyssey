// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyViewportDrawingEditor : ModuleRules
{
    public OdysseyViewportDrawingEditor(ReadOnlyTargetRules Target) : base(Target)
    {
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
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "DesktopPlatform",
                "Engine",
                "Projects",
                "InputCore",
                "RenderCore",
                "RHI",
                "Slate",
				"SlateCore",
                "EditorStyle",
                "UnrealEd",
                "RawMesh",
                "SourceControl",
                "ViewportInteraction",
                "VREditor",
                "PropertyEditor",
                "MainFrame",
				"MeshPaint",
                "OdysseyBrush",
                "OdysseyMeshPaintRendering",
                "OdysseyWidgets",
                "OdysseyLayer",
                "OdysseyTexture",
                "OdysseyImaging",
                "OdysseyPaintEngine",
                "OdysseyStrokePipeline",
                "OdysseyStylusInput",
                "OdysseyStyle"
            }
        );

        /*CircularlyReferencedDependentModules.AddRange(
            new string[]
            {
                "ViewportInteraction",
                "VREditor"
            }
        );*/

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
    }
}
