// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyAnimation : ModuleRules
    {
		public OdysseyAnimation(ReadOnlyTargetRules Target) : base(Target)
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
                    "ULIS",
                    "ULISLoader",
					// ... add other public dependencies that you statically link with here ...
				}
				);
                
			PrivateDependencyModuleNames.AddRange(
				new string[]
                {
                    "AssetTools",
                    "Core",
                    "CoreUObject",
                    "EditorStyle",
					"EditorWidgets",
					"Engine",
                    "InputCore",
					"Media",
					"MediaAssets",
					"OdysseyImaging",
                    "OdysseyLayer",
                    "OdysseyLayerStack",
                    "OdysseyPsdOperations",
                    "Slate",
                    "SlateCore",
                    "UnrealEd",
					"RHI" //To have access to GPixelFormats
                    //"AppFramework", // For SColorPicker inside OdysseyAnimationFactory.cpp
					// "Core",
					// "MainFrame",
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
