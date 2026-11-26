// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
/*
*   earcut
*__________________
* @file         earcut.Build.cs
* @author       Gary GABRIEL
* @brief        Module description file for UnrealEngine4
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
using System;
using System.IO;
using UnrealBuildTool;

public class earcut : ModuleRules
{
    public string GetRootPath()
    {
        //Assuming Source/ThirdParty/earcut/
        return Path.GetFullPath( Path.Combine( ModuleDirectory, "../../.." ) );
    }

    private string CopyToBinaries( string iSrcFilepath, ReadOnlyTargetRules iTarget )
    {
        string binariesDir  = Path.GetFullPath( Path.Combine( GetRootPath(), "Binaries", iTarget.Platform.ToString() ) );
        string filename     = Path.GetFileName( iSrcFilepath );
        string dstFilepath = Path.Combine( binariesDir, filename );

        if( !Directory.Exists( binariesDir ) )
            Directory.CreateDirectory( binariesDir );

        if( !File.Exists( dstFilepath ) )
            File.Copy( iSrcFilepath, dstFilepath, true);

        return dstFilepath;
    }

    public earcut( ReadOnlyTargetRules iTarget ) : base( iTarget )
    {
        // Allowed in game targets: the Ariane runtime module depends on it (header-only library).

        //Inactivate Unity builds to force devs to include all the necessary include files overywhere it is needed
        //Inactivate Code Optimization in Debug configurations
        //Because Engine modules and Engine Plugin Modules are always optimized by default
        //and we don't want that
        if (Target.Configuration == UnrealTargetConfiguration.Debug || Target.Configuration == UnrealTargetConfiguration.DebugGame)
        {
            bMergeUnityFiles = false;
            PCHUsage = ModuleRules.PCHUsageMode.NoPCHs;
            bUseUnity = false;
            OptimizeCode = CodeOptimization.Never;
        }

        Type = ModuleType.External;

        string includePath  = Path.GetFullPath( Path.Combine( ModuleDirectory, "include" ) );
        string baseName = "earcut";

        PublicSystemIncludePaths.Add( includePath );
    }
}
