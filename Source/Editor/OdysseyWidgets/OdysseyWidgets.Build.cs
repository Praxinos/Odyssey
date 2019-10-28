// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyWidgets : ModuleRules
{
    public OdysseyWidgets(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AppFramework",
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "InputCore",
                "EditorStyle",
                "Engine",
                "UnrealEd",
                "Layers",
                "MainFrame",
                "GraphEditor",
                "PropertyEditor",
                "OdysseyBrush",
                "OdysseyImaging",
                "OdysseyLayer",
                "OdysseyStyle",
                "OdysseyStrokePipeline",
                "OdysseyTransaction",
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
             }
        );

    }
}
