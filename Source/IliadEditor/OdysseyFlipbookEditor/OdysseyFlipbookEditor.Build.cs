// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using System.IO;
using UnrealBuildTool;

public class OdysseyFlipbookEditor : ModuleRules
{
    public OdysseyFlipbookEditor(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

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
            throw new InvalidOperationException("ERROR in OdysseyFlipbookEditor Module : Target.Type == TargetType.Game");
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

        PrivateIncludePathModuleNames.AddRange(
            new string[] {
            }
        );

        PrivateDependencyModuleNames.AddRange(
            new string[] {
                "AssetTools",
                "Core",
                "CoreUObject",
                "Engine",
                "Paper2D",
                "Paper2DEditor",
                "Settings",
                "Slate",
                "SlateCore",
                "UnrealEd",

                "OdysseyPainterEditor",
                "OdysseyTelemetry",
            }
        );

        PrivateIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicIncludePaths.AddRange(
            new string[] {
            }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
             }
        );

        DynamicallyLoadedModuleNames.AddRange(
             new string[] {
             }
        );
    }
}
