// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyAnimationEditor : ModuleRules
    {
		public OdysseyAnimationEditor(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Public"),
					Path.Combine(ModuleDirectory, "Public", "AnimationEditor"),
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
					Path.Combine(ModuleDirectory, "Private", "AnimationEditor"),
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
					"AppFramework",
					"Core",
                	"CoreUObject",
                    "DesktopPlatform",
                	"EditorStyle",
					"EditorWidgets",
					"Engine",
					"InputCore",
					"ToolMenus",
					"Media",
					"MediaAssets",
					"OdysseyAnimation",
					"OdysseyAnimationPlayer",
					"OdysseyAnimationTexture",
					"OdysseyBrush",
					"OdysseyCore",
					"OdysseyFlipbookEditor",
					"OdysseyPaintEngine",
					"OdysseyImaging",
					"OdysseyMedia",
					"OdysseyLayerStack",
					"OdysseyLayerStackEditor",
					"OdysseyEditor",
					"OdysseyPainterEditor",
					"OdysseyStyle",
					"OdysseyTexture",
					"OdysseyTextureEditor",
					"OdysseyWidgets",
					"OdysseyHUDSystem",
					"OdysseyVector",
					"Paper2D",
					"Paper2DEditor",
					"RenderCore",
                	"Slate",
					"SlateCore",
					"ToolWidgets",
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
