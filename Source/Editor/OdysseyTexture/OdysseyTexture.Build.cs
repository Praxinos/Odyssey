// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyTexture : ModuleRules
    {
		public OdysseyTexture(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Private", "LayerStack")
					// ... add public include paths required here ...
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public", "LayerStack")
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
					"OdysseyCore",
                    "OdysseyImaging",
					"OdysseyMaths",
					"OdysseyLayerStack",
					"OdysseyStyle",
                    "Slate",
                    "SlateCore",
                    "UnrealEd",
					"TextureEditor", //To use TextureEditorSettings in SOdysseyTextureDetails //we could also copy TextureEditorSettings content
					"RHI", //To have access to GPixelFormats
					"zlib"
                    //"AppFramework", // For SColorPicker inside OdysseyTextureFactory.cpp
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
