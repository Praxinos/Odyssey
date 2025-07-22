// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyRendering : ModuleRules
{
    public OdysseyRendering(ReadOnlyTargetRules Target) : base(Target)
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

        PublicIncludePaths.AddRange(
            new string[] {
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicIncludePathModuleNames.AddRange(
            new string[] {
                "OdysseyCore",
            }
        );

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
            new string[] {
            }
        );

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "Engine",
                "InputCore",
                "Projects",
                "RawMesh",
                "Renderer",
                "RHI",
                "RenderCore",
                "SlateCore",
                "Slate",

                "OdysseyCore",
             }
        );


        //---
        /* if (Target.Type == TargetType.Editor)
        {
            PublicDependencyModuleNames.AddRange(
                new string[] {
                    "ULIS",//--- Link to ULIS only if TargetType is Editor
                    "ULISLoader"//--- Link to ULIS only if TargetType is Editor
                }
            );

            PrivateDependencyModuleNames.AddRange(
                new string[] {
                    "blend2d",
                    "OdysseyFile", //Needed to load VectorBlock and RasterBlock
                    "OdysseyVector", //Needed to load VectorBlock and RasterBlock
                }
            );
        } */

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
