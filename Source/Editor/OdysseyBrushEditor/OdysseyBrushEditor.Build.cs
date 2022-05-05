// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyBrushEditor : ModuleRules
{
    public OdysseyBrushEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AssetTools",
                "Core",
                "CoreUObject",
                "EditorStyle",
                "Engine",
                "PropertyEditor",
                "Slate",
                "SlateCore",
                "UnrealEd",

                "OdysseyBrush"
            }
            );

        DynamicallyLoadedModuleNames.AddRange(
            new string[] {
            }
            );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader",
             }
        );
    }
}
