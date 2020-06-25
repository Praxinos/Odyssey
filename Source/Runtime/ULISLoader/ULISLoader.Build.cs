// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

using System.IO;
using UnrealBuildTool;

public class ULISLoader : ModuleRules
{
    public ULISLoader( ReadOnlyTargetRules Target ) : base( Target )
    {
        PublicDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "ULIS"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine( ModuleDirectory, "Private" )
            }
        );
    }
}
