// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

// Compile this public interface from a consumer module without OdysseyImaging's private include paths.
#include "OdysseyTiledImage.h"

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"
#include "UObject/StrongObjectPtr.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyTiledImagePublicHeaderTest, "Odyssey.Imaging.TiledImage.PublicInterface",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)
bool FOdysseyTiledImagePublicHeaderTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UOdysseyTiledImage> Image(NewObject<UOdysseyTiledImage>());
    Image->Initialize(32, EOdysseyTiledImageFormat::RGBA16F);
    TestEqual(TEXT("Public image API is usable from another module"), Image->GetPixelFormat(), PF_FloatRGBA);
    TestNotNull(TEXT("Tile identity reflection is exported"), FOdysseyTileId::StaticStruct());
    TestFalse(TEXT("Default tile identity is empty"), FOdysseyTileId().IsValid());
    return true;
}
#endif
