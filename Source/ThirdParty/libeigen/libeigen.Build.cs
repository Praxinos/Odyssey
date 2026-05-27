// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
/*
*   libeigen
*__________________
* @file         libeigen.Build.cs
* @author       Gary GABRIEL
* @brief        Module description file for UnrealEngine4
* @copyright    Copyright 2018-2024 Praxinos, Inc. All Rights Reserved.
* @license      Please refer to LICENSE.md
*/
using System;
using System.IO;
using UnrealBuildTool;

public class libeigen : ModuleRules
{
    public string GetRootPath()
    {
        //Assuming Source/ThirdParty/libeigen/
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

    public libeigen( ReadOnlyTargetRules iTarget ) : base( iTarget )
    {
        Type = ModuleType.External;

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
            throw new InvalidOperationException("ERROR in libeigen Module : Target.Type == TargetType.Game");
        }

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

        string includePath  = Path.GetFullPath( Path.Combine( ModuleDirectory, "include/eigen3" ) );
//        string libPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "lib" ) );
//        string binPath      = Path.GetFullPath( Path.Combine( ModuleDirectory, "bin" ) );
//        string baseName = "libeigen";

        if( iTarget.Platform == UnrealTargetPlatform.Win64 /*||
            iTarget.Platform == UnrealTargetPlatform.Win32*/ )
        {
//            string libName = baseName + ".lib";
//            string binName = baseName + ".dll";

            PublicSystemIncludePaths.Add( includePath );
//            PublicAdditionalLibraries.Add( Path.Combine( libPath, libName ) );

//            string binariesPath = CopyToBinaries( Path.Combine( binPath, binName ), iTarget );
//            RuntimeDependencies.Add( "$(BinaryOutputDir)/" + binName, "$(ModuleDir)/bin/" + binName );

//            System.Console.WriteLine( "Using " + baseName +" DLL: " + binariesPath );
        }
        else if ( iTarget.Platform == UnrealTargetPlatform.Mac )
        {
//            baseName = "libeigen";
//            string binName = baseName + ".dylib";;

            PublicSystemIncludePaths.Add( includePath );
//            PublicAdditionalLibraries.Add( Path.Combine( binPath, binName ) );

//            string binariesPath = CopyToBinaries( Path.Combine( binPath, binName ), iTarget );
 //           RuntimeDependencies.Add( "$(BinaryOutputDir)/" + binName, ModuleDirectory + "/bin/" + binName );

 //           System.Console.WriteLine( "Using " + baseName +" DYLIB: " + binariesPath );
        }
    }
}
