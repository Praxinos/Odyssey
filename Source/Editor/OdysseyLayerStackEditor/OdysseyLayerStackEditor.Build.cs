// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyLayerStackEditor : ModuleRules
{
    public OdysseyLayerStackEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "EditorStyle",
                "Engine",
                "InputCore",
                "Slate",
                "SlateCore",
                "UMG",
                "UnrealEd",
                "ToolMenus",
                "ToolWidgets",

                "OdysseyCore",
                "OdysseyBrush",
                "OdysseyEditor",
                "OdysseyImaging",
                "OdysseyLayerStack",
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
