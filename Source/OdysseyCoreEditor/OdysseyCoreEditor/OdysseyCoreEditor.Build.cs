// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;

public class OdysseyCoreEditor : ModuleRules
{
	public OdysseyCoreEditor(ReadOnlyTargetRules Target) : base(Target)
	{
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
                "Engine",
                "ToolMenus",

                // [RUNTIME] plugin dependencies

                // [EDITOR] engine dependencies
                "EditorSubsystem",
                "UnrealEd",

                // [EDITOR] plugin dependencies
                "OdysseyCommonWidgets",
                "OdysseyStyle",
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
			}
        );
	}
}
