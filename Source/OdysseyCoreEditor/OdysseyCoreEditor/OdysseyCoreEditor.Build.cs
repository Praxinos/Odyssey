// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

using System;
using UnrealBuildTool;

public class OdysseyCoreEditor : ModuleRules
{
    public OdysseyCoreEditor(ReadOnlyTargetRules Target) : base(Target)
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
            throw new InvalidOperationException("ERROR in OdysseyCoreEditor Module : Target.Type == TargetType.Game");
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

        //Module's own include paths
        PrivateIncludePaths.AddRange
        (
            new string[]
            {
            }
        );

        //Dependencies
        //List of modules name (no path needed) with header files that our module's private code files needs access to, but we don't need to "import" or link against.
        //First add your module's private dependencies here, and if it doesn't work, with symbol not found errors, add it to PrivateDependencyModuleNames
        PrivateIncludePathModuleNames.AddRange
        (
            new string[]
            {
            }
        );

        //List of private dependency module names (no path needed) (automatically does the private/public include). These are modules that are required by our public source files.
        PrivateDependencyModuleNames.AddRange
        (
            new string[]
            {
                // [RUNTIME] engine dependencies
                "Core",
                "CoreUObject",
                "Slate",
                "SlateCore",
                "ToolMenus",

                // [RUNTIME] plugin dependencies
                "Projects",

                // [EDITOR] engine dependencies
                "EditorSubsystem",
                "UnrealEd",

                // [EDITOR] plugin dependencies
                "OdysseyCommonWidgets",
                "OdysseyStyle",

                // [DEVELOPER] engine dependencies
                "DerivedDataCache",
            }
        );

        //List of modules names (no path needed) with header files that our module's public headers needs access to, but we don't need to "import" or link against.
        //First add your module's public dependencies here, and if it doesn't work, with symbol not found errors, add it to PublicDependencyModuleNames
        PublicIncludePathModuleNames.AddRange
        (
            new string[]
            {
            }
        );

        //List of public dependency module names (no path needed) (automatically does the private/public include). These are modules that are required by our public source files.
        PublicDependencyModuleNames.AddRange
        (
            new string[]
            {
                "Engine",
            }
        );
    }
}
