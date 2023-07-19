// Copyright © 2018-2022 Praxinos, Inc. All Rights Reserved.
// IDDN.FR.001.250001.006.S.P.2019.000.00000
/*
*   blend2d
*__________________
* @file         blend2d.Build.cs
* @author       Gary GABRIEL / Clement Berthaud
* @brief        Module description file for UnrealEngine4
* @copyright    Copyright 2018-2022 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
using System;
using System.IO;
using UnrealBuildTool;

public class blend2d : ModuleRules
{
    public string GetRootPath()
    {
        //Assuming Source/ThirdParty/blend2d/
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

    public blend2d( ReadOnlyTargetRules iTarget ) : base( iTarget )
    {
        Type = ModuleType.External;

        string includePath  = Path.GetFullPath( Path.Combine( ModuleDirectory, "include" ) );
        string libPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "lib" ) );
        string binPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "bin" ) );
        string baseName = "blend2d";

        if( iTarget.Platform == UnrealTargetPlatform.Win64 /*||
            iTarget.Platform == UnrealTargetPlatform.Win32*/ )
        {
            string libName = baseName + ".lib";
            string binName = baseName + ".dll";

            PublicSystemIncludePaths.Add( includePath );
            PublicAdditionalLibraries.Add( Path.Combine( libPath, libName ) );

            string binariesPath = CopyToBinaries( Path.Combine( binPath, binName ), iTarget );
            RuntimeDependencies.Add( "$(BinaryOutputDir)/" + binName, "$(ModuleDir)/bin/" + binName );

            System.Console.WriteLine( "Using " + baseName +" DLL: " + binariesPath );
        }
        else if ( iTarget.Platform == UnrealTargetPlatform.Mac )
        {
            string binName = baseName + ".dylib";;

            PublicSystemIncludePaths.Add( includePath );
            PublicAdditionalLibraries.Add( Path.Combine( binPath, binName ) );

            string binariesPath = CopyToBinaries( Path.Combine( binPath, binName ), iTarget );
            RuntimeDependencies.Add( "$(BinaryOutputDir)/" + binName, "$(ModuleDir)/bin/" + binName );

            System.Console.WriteLine( "Using " + baseName +" DYLIB: " + binariesPath );
        }
    }
}

