// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyTexture2DEditor : ModuleRules
    {
		public OdysseyTexture2DEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public"),
					Path.Combine(ModuleDirectory, "Public", "Settings"),
					Path.Combine(ModuleDirectory, "Public", "Texture2DEditor"),
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Private", "Models"),
					Path.Combine(ModuleDirectory, "Private", "Texture2DEditor"),
					Path.Combine(ModuleDirectory, "Private", "Settings"),
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
                    "DesktopPlatform",
                	"EditorStyle",
					"Engine",
					"TextureEditor",
					"OdysseyBrush",
					"OdysseyPaintEngine",
					"OdysseyImaging",
					"OdysseyLayer",
					"OdysseyEditor",
					"OdysseyPainterEditor",
					"OdysseyTexture",
					"OdysseyTextureEditor",
					"OdysseyWidgets",
                	"Slate",
					"SlateCore",
					"UnrealEd",
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
