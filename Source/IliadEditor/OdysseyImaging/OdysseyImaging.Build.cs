// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyImaging : ModuleRules
{
    public OdysseyImaging(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        //Inactivate Unity builds to force devs to include all the necessary include files overywhere it is needed
        //Inactivate Code Optimization in Debug configurations
        //Because Engine modules and Engine Plugin Modules are always optimized by default
        //and we don't want that
        if (Target.Configuration == UnrealTargetConfiguration.Debug)
        {
            bMergeUnityFiles = false;
            bUseUnity = false;
            OptimizeCode = CodeOptimization.Never;
        }

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "DerivedDataCache",
                "ImageCore",
                "Paper2D",
                "Paper2DEditor",
                "RenderCore",
                "RHI",
                "TargetPlatform",
                "TextureCompressor",
                "TextureBuildUtilities",
                "UnrealEd",

                "OdysseyCore",
                "OdysseyFile",
                "OdysseyRendering"
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "Engine",
                "blend2d",
                "ULIS",
                "ULISLoader",

                "OdysseyCoreEditor",
                "OdysseyVector",
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
