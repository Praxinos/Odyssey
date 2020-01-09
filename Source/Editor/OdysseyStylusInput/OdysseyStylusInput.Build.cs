// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

namespace UnrealBuildTool.Rules
{
	public class OdysseyStylusInput : ModuleRules
    {
		public OdysseyStylusInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					// ... add other private include paths required here ...
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					"CoreUObject",
					"EditorSubsystem",
					"Engine",
					"UnrealEd",
					"WinTab",
					// ... add other public dependencies that you statically link with here ...
				}
				);

            //We need ApplicationCore for Mac for this module
            if( Target.Platform == UnrealTargetPlatform.Mac )
            {
                PrivateDependencyModuleNames.AddRange(
                new string[]
				{
                    "ApplicationCore"
                }
                );
            }
                
			PrivateDependencyModuleNames.AddRange(
				new string[]
				{
					"Core",
					"MainFrame",
					"SlateCore",
					"Slate",
					"WorkspaceMenuStructure"
					// ... add private dependencies that you statically link with here ...
				}
				);

			DynamicallyLoadedModuleNames.AddRange(
				new string[]
				{
					// ... add any modules that your module loads dynamically here ...
				}
				);
		}
	}
}
