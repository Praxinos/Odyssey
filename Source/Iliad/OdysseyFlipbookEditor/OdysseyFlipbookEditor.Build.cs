// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
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
                "KismetWidgets",
                "PropertyEditor",
                "Projects",
                "OdysseyCore",
                "OdysseyStyle",
                "OdysseyBrush",
                "OdysseyWidgets",
                "OdysseyImaging",
                "OdysseyLayerStack",
                "OdysseyEditor",
                "OdysseyMedia",
                "OdysseyPainterEditor",
                "OdysseyPaintEngine",
                "OdysseyStylusInput",
                "OdysseyFlipbook",
                "OdysseyTexture",
                "OdysseyTextureEditor",
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
                Path.Combine(ModuleDirectory, "Public", "FlipbookEditor"),
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
