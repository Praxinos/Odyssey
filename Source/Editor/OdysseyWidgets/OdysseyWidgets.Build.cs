// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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
