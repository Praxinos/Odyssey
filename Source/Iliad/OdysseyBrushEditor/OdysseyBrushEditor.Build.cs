// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using System;
using System.IO;
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
