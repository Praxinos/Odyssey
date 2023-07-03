// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyPalette : ModuleRules
    {
		public OdysseyPalette(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
                new string[] {
                Path.Combine(ModuleDirectory, "Public", "Palette"),
                Path.Combine(ModuleDirectory, "Public", "Widgets"),
                Path.Combine(ModuleDirectory, "Public", "DragDropOperations"),

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
                    "ULIS",
                    "ULISLoader",
					// ... add other public dependencies that you statically link with here ...
				}
				);
                
			PrivateDependencyModuleNames.AddRange(
				new string[]
                {
                    "AppFramework",
                    "AssetTools",
                    "Core",
                    "CoreUObject",
                    "EditorStyle",
                    "EditorWidgets",
                    "Engine",
                    "InputCore",
                    "ToolMenus",
                    "OdysseyCore",
                    "Slate",
                    "SlateCore",
                    "UnrealEd",
                    "OdysseyStyle",
                    "PropertyEditor",
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
