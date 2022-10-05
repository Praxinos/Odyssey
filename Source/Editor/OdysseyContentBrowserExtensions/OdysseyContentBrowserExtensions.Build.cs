// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyContentBrowserExtensions : ModuleRules
    {
        public OdysseyContentBrowserExtensions(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
            // bAddDefaultIncludePaths = true;

            PublicIncludePaths.AddRange(
                new string[] {
                    // ... add public include paths required here ...
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    // ... add other private include paths required here ...
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "ULIS",
                    "ULISLoader",
                    // ... add other public dependencies that you statically link with here ...
                }
                );
                
            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "AssetTools",
                    "Core",
                    "CoreUObject",
                    "Engine",
                    "Slate",
                    "SlateCore",
                    "DesktopPlatform",
                    "EditorWidgets",
                    "MainFrame",
                    "InputCore",
                    "UnrealEd",
                    "OdysseyImaging",
                    "OdysseyTexture",
                    "EditorStyle"
                    // ... add private dependencies that you statically link with here ...
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    // ... add any modules that your module loads dynamically here ...
                }
                );
        }
    }
}

