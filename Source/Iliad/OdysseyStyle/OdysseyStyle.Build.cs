// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyStyle : ModuleRules
{
    public OdysseyStyle(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "CoreUObject",
                "Slate",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "Projects",
                "SlateCore",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
            }
        );

        // DesktopPlatform is only available for Editor and Program targets (running on a desktop platform)
        bool IsDesktopPlatformType = /*Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Win32
            ||*/ Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Win64
            || Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Mac
            || Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Linux;
        if (Target.Type == TargetType.Editor || (Target.Type == TargetType.Program && IsDesktopPlatformType))
        {
            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "DesktopPlatform",
                }
            );
        }

        // I'm told this is to improve compilation performance of this module
        OptimizeCode = CodeOptimization.Never;
                
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
