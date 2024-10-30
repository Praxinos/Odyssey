// Copyright � 2018-2019 Praxinos, Inc. All Rights Reserved.
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System;
using System.IO;
using UnrealBuildTool;

public class ULISLoader : ModuleRules
{
    public ULISLoader( ReadOnlyTargetRules Target ) : base( Target )
    {
        PublicDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "OdysseyCore",
                "ULIS"
             }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
                Path.Combine( ModuleDirectory, "Private" )
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
