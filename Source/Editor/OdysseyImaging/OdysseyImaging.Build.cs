// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

using UnrealBuildTool;

public class OdysseyImaging : ModuleRules
{
    public OdysseyImaging(ReadOnlyTargetRules Target) : base(Target)
    {
        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "OdysseyCore",
                "OdysseyBase",
                "CoreUObject",
                "Engine",
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
             }
        );

    }
}
