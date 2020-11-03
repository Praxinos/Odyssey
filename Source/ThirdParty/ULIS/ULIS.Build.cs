// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
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

    private string CopyToBinaries(string Filepath, ReadOnlyTargetRules Target)
    {
        string BinariesDir      = Path.Combine( GetRootPath(), "Binaries", Target.Platform.ToString() );
        string Filename         = Path.GetFileName( Filepath );
        string FullBinariesDir  = Path.GetFullPath( BinariesDir );

        if( !Directory.Exists( FullBinariesDir ) ) {
            Directory.CreateDirectory( FullBinariesDir );
        }

        string FullFilePath = Path.Combine( FullBinariesDir, Filename );

        if( File.Exists( FullFilePath ) )
            return FullFilePath;

        File.Copy( Filepath, Path.Combine( FullBinariesDir, Filename ), true);
        return FullFilePath;
    }

    public ULIS( ReadOnlyTargetRules Target ) : base( Target )
    {
        Type = ModuleType.External;
        if (Target.Platform == UnrealTargetPlatform.Win64 ||
            Target.Platform == UnrealTargetPlatform.Win32 )
        {
            string IncludeBase  = Path.GetFullPath( Path.Combine( ModuleDirectory, "redist", "include" ) );
            string LibBase      = Path.GetFullPath( Path.Combine( ModuleDirectory, "redist", "lib" ) );
            string BinBase      = Path.GetFullPath( Path.Combine( ModuleDirectory, "redist", "bin" ) );
            string ULIS_Name = "ULIS3";
            string ULIS_LibName = ULIS_Name + ".lib";
            string ULIS_DLLName = ULIS_Name + ".dll";

            PublicSystemIncludePaths.Add( Path.Combine( IncludeBase, ULIS_Name) );
            PublicAdditionalLibraries.Add( Path.Combine( LibBase, ULIS_LibName ) );

            PublicDefinitions.Add("ULIS3_DYNAMIC_LIBRARY");

            string pluginDLLPath = Path.Combine( BinBase, ULIS_DLLName );
            string binariesPath = CopyToBinaries( pluginDLLPath, Target );
            System.Console.WriteLine( "Using " + ULIS_Name +" DLL: " + binariesPath );

            RuntimeDependencies.Add( "$(BinaryOutputDir)/ULIS3.dll", "$(ModuleDir)/redist/bin/ULIS3.dll" );
        }
        else if ( Target.Platform == UnrealTargetPlatform.Mac )
        {
            string IncludeBase  = Path.GetFullPath( Path.Combine( ModuleDirectory, "redist", "include" ) );
            string BinBase      = Path.GetFullPath( Path.Combine( ModuleDirectory, "redist", "bin" ) );
            string ULIS_Name = "ULIS3";
            string ULIS_DylibName = ULIS_Name + ".dylib";
            string pluginDylibPath = Path.Combine( BinBase, ULIS_DylibName );

            PublicSystemIncludePaths.Add( Path.Combine( IncludeBase, ULIS_Name) );
            PublicAdditionalLibraries.Add( pluginDylibPath );

            PublicDefinitions.Add("ULIS3_DYNAMIC_LIBRARY");

            string binariesPath = CopyToBinaries( pluginDylibPath, Target );
            System.Console.WriteLine( "Using " + ULIS_Name +" DYLIB: " + binariesPath );

            RuntimeDependencies.Add( "$(BinaryOutputDir)/ULIS3.dylib", "$(ModuleDir)/redist/bin/ULIS3.dylib" );
        }
    }
}

