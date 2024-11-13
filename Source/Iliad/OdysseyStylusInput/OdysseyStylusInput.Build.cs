// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;

namespace UnrealBuildTool.Rules
{
    public class OdysseyStylusInput : ModuleRules
    {
        public OdysseyStylusInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            //Inactivate Unity builds to force devs to include all the necessary include files overywhere it is needed
            //Inactivate Code Optimization in Debug and DebugGame configurations
            //Because Engine modules and Engine Plugin Modules are always optimized by default
            //and we don't want that
            if (Target.Configuration == UnrealTargetConfiguration.Debug
                || Target.Configuration == UnrealTargetConfiguration.DebugGame)
            {
                bMergeUnityFiles = false;
                bUseUnity = false;
                OptimizeCode = CodeOptimization.Never;
            }
            // bAddDefaultIncludePaths = true;


            PublicIncludePaths.AddRange(
                new string[] {
                    Path.Combine(ModuleDirectory, "Public", "Settings")
                    // ... add public include paths required here ...
                }
                );

            PrivateIncludePaths.AddRange(
                new string[] {
                    // ... add other private include paths required here ...
                }
                );

            PublicDependencyModuleNames.AddRange(
                new string[]
                {
                    "CoreUObject",
                    "EditorSubsystem",
                    "Engine",
                    "UnrealEd",
                    "WinTab",
                    // ... add other public dependencies that you statically link with here ...
                }
            );

            if (Target.Platform == UnrealTargetPlatform.Win64)
            {
                PrivateDefinitions.AddRange(
                    new string[]
                    {
                        "WINVER=0x0602" //Minimal version Windows 8, needed to get access to WM_POINTER events
                    }
                );
            }

            if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                //We need ApplicationCore for Mac for this module
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "ApplicationCore"
                    }
                );
            }

            PrivateDependencyModuleNames.AddRange(
                new string[]
                {
                    "Core",
                    "MainFrame",
                    "SlateCore",
                    "Slate",
                    "WorkspaceMenuStructure"
                    // ... add private dependencies that you statically link with here ...
                }
                );

            DynamicallyLoadedModuleNames.AddRange(
                new string[]
                {
                    // ... add any modules that your module loads dynamically here ...
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
}
