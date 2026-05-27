// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;
using System;
using System.IO;

public class libharu : ModuleRules
{
    public libharu( ReadOnlyTargetRules Target ) : base( Target )
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
            throw new InvalidOperationException("ERROR in libharu Module : Target.Type == TargetType.Game");
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
