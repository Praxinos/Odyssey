// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyStyle : ModuleRules
{
    public OdysseyStyle(ReadOnlyTargetRules Target) : base(Target)
    {

        PrivateIncludePaths.AddRange(
            new string[] {
                "Odyssey/OdysseyEditor/OdysseyStyle/Private"
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
                "Odyssey/OdysseyEditor/OdysseyStyle/Public"
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
                "Core",
                "OdysseyCore",
                "CoreUObject",
                "Slate",
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "SlateCore",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
                "Settings",
            }
        );

        // DesktopPlatform is only available for Editor and Program targets (running on a desktop platform)
        bool IsDesktopPlatformType = Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Win32
            || Target.Platform == UnrealBuildTool.UnrealTargetPlatform.Win64
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
    }
}
