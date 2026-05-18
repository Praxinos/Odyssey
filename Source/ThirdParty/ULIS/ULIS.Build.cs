// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
/*
*   ULIS
*__________________
* @file         ULIS.Build.cs
* @author       Clement Berthaud
* @brief        Module description file for UnrealEngine4
* @copyright    Copyright 2018-2021 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
using System;
using System.IO;
using UnrealBuildTool;

public class ULIS : ModuleRules
{
    public string GetRootPath()
    {
        //Assuming Source/ThirdParty/ULIS/
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

    public ULIS( ReadOnlyTargetRules iTarget ) : base( iTarget )
    {
        /**
        * Sometimes Unreal can compile Editor modules even if it builds Game only modules
        * It happens when an editor module is defined as a dependency in a non editor module
        * and it is not encapsulated with if (Target.Type == TargetType.Editor)
        * So we make sure here to throw an error if this module is used in a game compilation
        *
        * If you hit this assert, search for this module being a non editor module's dependency.
        * It could also be an indirect dependency.
        */
        if (Target.Type == TargetType.Game)
        {
            throw new InvalidOperationException("ERROR in ULIS Module : Target.Type == TargetType.Game");
        }

        Type = ModuleType.External;

        string includePath  = Path.GetFullPath( Path.Combine( ModuleDirectory, "include" ) );
        string libPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "lib" ) );
        string binPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "bin" ) );
        string baseName = "ULIS4";

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
            string binName = baseName + ".dylib";

            PublicSystemIncludePaths.Add( includePath );
            PublicAdditionalLibraries.Add( Path.Combine( binPath, binName ) );

            string binariesPath = CopyToBinaries( Path.Combine( binPath, binName ), iTarget );
            RuntimeDependencies.Add( "$(BinaryOutputDir)/" + binName, ModuleDirectory + "/bin/" + binName );

            System.Console.WriteLine( "Using " + baseName +" DYLIB: " + binariesPath );
        }
    }
}
