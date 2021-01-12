// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyStylusInput : ModuleRules
    {
		public OdysseyStylusInput(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;
			// bAddDefaultIncludePaths = true;


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

            if (Target.Platform == UnrealTargetPlatform.Mac)
            {
                //We need ApplicationCore for Mac for this module
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
