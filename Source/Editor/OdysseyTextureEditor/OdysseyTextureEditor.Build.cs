// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyTextureEditor : ModuleRules
    {
		public OdysseyTextureEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public"),
					Path.Combine(ModuleDirectory, "Public", "Settings"),
					Path.Combine(ModuleDirectory, "Public", "TextureEditor"),
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Private", "Settings"),
					Path.Combine(ModuleDirectory, "Private", "TextureEditor"),
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
					"EditorWidgets",
					"Engine",
					"InputCore",
					"TextureEditor",
					"ToolMenus",
					"OdysseyBrush",
					"OdysseyCore",
					"OdysseyPaintEngine",
					"OdysseyImaging",
					"OdysseyMedia",
					"OdysseyLayerStack",
					"OdysseyLayerStackEditor",
					"OdysseyEditor",
					"OdysseyPainterEditor",
					"OdysseyStyle",
					"OdysseyTexture",
                    "OdysseyVector",
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
