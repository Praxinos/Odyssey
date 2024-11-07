// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyEditorBrushBlueprints : ModuleRules
{
    public OdysseyEditorBrushBlueprints(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "InputCore",
                "Engine",
                "MediaAssets",

                "OdysseyAnimation",
                "OdysseyBrush",
                "OdysseyLayerStack",
                "OdysseyLayerStackEditor",
                "OdysseyPainterEditor",
                "OdysseyTexture",
                "OdysseyImaging",

                "ULIS",
                "ULISLoader",
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
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
