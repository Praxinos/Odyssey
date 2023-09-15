// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;
using System.IO;

public class libharu : ModuleRules
{
    public libharu( ReadOnlyTargetRules Target ) : base( Target )
    {
        Type = ModuleType.External;

        string RootPath = ModuleDirectory;

        //---

        AddEngineThirdPartyPrivateStaticDependencies( Target, "zlib" );
        AddEngineThirdPartyPrivateStaticDependencies( Target, "UElibPNG" );

        //---

        if( Target.Platform.IsInGroup( UnrealPlatformGroup.Windows ) )
        {
            // string LibPath = RootPath + "/install-win64-vs2022-release/lib" + Target.WindowsPlatform.GetVisualStudioCompilerVersionName();
            string LibPath = Path.Combine( RootPath, "lib", "win64", "vs2022" );

            string LibFileName = "libhpdfs.lib";
            PublicAdditionalLibraries.Add( Path.Combine( LibPath, LibFileName ) );

            //---

            string IncludePath = Path.Combine( RootPath, "include", "win64", "vs2022" );
            PublicIncludePaths.Add( IncludePath );
        }
        else if( Target.Platform == UnrealTargetPlatform.Mac )
        {
            string LibPath = Path.Combine( RootPath, "lib", "macosx" );

            string LibFileName = "libhpdfs.a";
            PublicAdditionalLibraries.Add( Path.Combine( LibPath, LibFileName ) );

            //---

            string IncludePath = Path.Combine( RootPath, "include", "macosx" );
            PublicIncludePaths.Add( IncludePath );
        }
    }
}
