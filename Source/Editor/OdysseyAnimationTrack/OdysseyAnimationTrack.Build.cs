// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using System.IO;

namespace UnrealBuildTool.Rules
{
	public class OdysseyAnimationTrack : ModuleRules
    {
		public OdysseyAnimationTrack(ReadOnlyTargetRules Target) : base(Target)
        {
            PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

            PublicIncludePaths.AddRange(
				new string[] {
				}
				);

			PrivateIncludePaths.AddRange(
				new string[] {
				}
				);

			PublicDependencyModuleNames.AddRange(
				new string[]
				{
					// ... add other public dependencies that you statically link with here ...
				}
				);
                
			PrivateDependencyModuleNames.AddRange(
				new string[]
                {	
					"OdysseyAnimation",
					"OdysseyAnimationEditor",
					"OdysseyImaging",
					"OdysseyLayerStack",
					"OdysseyLayerStackEditor",
					"OdysseyPainterEditor",
					"OdysseyStyle",
					"OdysseyViewportDrawingEditor",

					"Core",
					"CoreUObject",
					"Engine",
					"InputCore",
					"LevelSequence",
					"MediaAssets",
					"MovieScene",
					"MovieSceneTools",
					"Sequencer",
					"SequencerCore",
					"Slate",
					"SlateCore",
					"UnrealEd"
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
