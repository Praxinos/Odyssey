// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
