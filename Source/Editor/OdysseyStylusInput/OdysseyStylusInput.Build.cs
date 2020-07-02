// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyStylusInput : ModuleRules
    {
		public OdysseyStylusInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
                    Path.Combine(ModuleDirectory, "Public", "Settings")
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



            if (Target.Platform == UnrealTargetPlatform.Win64 || Target.Platform == UnrealTargetPlatform.Win32)
            {
                PublicIncludePaths.AddRange(
                    new string[] {
                        Path.Combine(ModuleDirectory, "Public", "Settings", "Windows" )
                    }
                );
            }
            else if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                PublicIncludePaths.AddRange(
                    new string[] {
                        Path.Combine(ModuleDirectory, "Public", "Settings", "Mac" )
                    }
                );

                //We need ApplicationCore for Mac for this module
                PrivateDependencyModuleNames.AddRange(
                    new string[]
                    {
                        "ApplicationCore"
                    }       
                );
            }
            else
            {
                PublicIncludePaths.AddRange(
                    new string[] {
                    Path.Combine(ModuleDirectory, "Public", "Settings", "Generic" )
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
