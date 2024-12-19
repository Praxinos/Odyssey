// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyCore : ModuleRules
{
    public OdysseyCore(ReadOnlyTargetRules Target) : base(Target)
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
                "Engine",
                "InputCore",
                "SlateCore",
                "Slate"
             }
        );

        //TODO: this should be only temporary (or maybe not)
        //
        // Needed for OdysseyMutator:
        // - as it uses GEditor->IsTransactionActive() (inside #if WITH_EDITOR)
        // - so UnrealEd module must be linked (to not have unresolved symbol error)
        // - but OdysseyCore module must be a runtime module
        // Something must be done to OdysseyMutator, maybe move it to an editor module,
        // but it must wait that OdysseyAnimation will be split into 2 runtime and editor modules
        // and also UOdysseyAnimationLayerImageRaster::AutoCreateCell() will be moved in an editor module
        //
        // Needed for FOdysseyUndoDelegates:
        // - as it uses GEditor and UTransBuffer (inside #if WITH_EDITOR)
        // - so UnrealEd module must be linked (to not have unresolved symbol error)
        // - but OdysseyCore module must be a runtime module
        // Something should be done to maybe split FOdysseyUndoDelegates into 2 classes: 1 interface (runtime) and 1 implementation (editor)
        // but the problem is with the static Get() which creates the singleton which can't be overrided in the implementation
        if (Target.bBuildEditor == true)
        {
            PrivateDependencyModuleNames.AddRange(
                 new string[] {
                     "UnrealEd",
                 }
            );

        }

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
