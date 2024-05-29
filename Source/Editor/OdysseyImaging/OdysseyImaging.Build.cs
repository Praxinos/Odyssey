// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

using UnrealBuildTool;

public class OdysseyImaging : ModuleRules
{
    public OdysseyImaging(ReadOnlyTargetRules Target) : base(Target)
    {
        PCHUsage = ModuleRules.PCHUsageMode.UseExplicitOrSharedPCHs;

        PrivateDependencyModuleNames.AddRange(
             new string[] {
                "Core",
                "CoreUObject",
                "DerivedDataCache",
                "Engine",
                "ImageCore",
                "RenderCore",
                "RHI",
                "TargetPlatform",
                "TextureCompressor",
                "TextureBuildUtilities",
                "UnrealEd",

                "OdysseyCore",
                "OdysseyFile",
                "OdysseyMaths",
                "OdysseyVector",
                "OdysseyHUDSystem",
                "blend2d",
             }
        );

        PublicDependencyModuleNames.AddRange(
             new string[] {
                "ULIS",
                "ULISLoader"
             }
        );

    }
}
