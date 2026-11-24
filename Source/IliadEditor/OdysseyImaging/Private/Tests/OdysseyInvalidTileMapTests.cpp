// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Misc/AutomationTest.h"

#include "OdysseyInvalidTileMap.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyInvalidTileMapTileIndexesTest, "Odyssey.Core.InvalidTileMap.TileIndexes", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** The tiles a rect touches: Max is exclusive, so a rect ending on a tile boundary does not touch the next tile. */
bool FOdysseyInvalidTileMapTileIndexesTest::RunTest(const FString& Parameters)
{
    // 3 x 2 tiles of 64 pixels, the last column and row partial.
    const FOdysseyInvalidTileMap Map(64, 150, 100);

    TestEqual(TEXT("An aligned rect touches its tile only"), Map.GetTileIndexesForRect(FIntRect(0, 0, 64, 64)).Num(), 1);
    TestEqual(TEXT("An aligned rect of 2 x 1 tiles"), Map.GetTileIndexesForRect(FIntRect(64, 0, 192, 64)).Num(), 2);
    TestEqual(TEXT("A rect crossing a boundary touches both tiles"), Map.GetTileIndexesForRect(FIntRect(60, 10, 70, 20)).Num(), 2);
    TestEqual(TEXT("A one-pixel rect touches one tile"), Map.GetTileIndexesForRect(FIntRect(63, 63, 64, 64)).Num(), 1);
    TestEqual(TEXT("A rect with a negative min is clipped"), Map.GetTileIndexesForRect(FIntRect(-10, -10, 10, 10)).Num(), 1);
    TestEqual(TEXT("An empty rect touches no tile"), Map.GetTileIndexesForRect(FIntRect(10, 10, 10, 20)).Num(), 0);
    TestEqual(TEXT("A rect before the canvas touches no tile"), Map.GetTileIndexesForRect(FIntRect(-20, -20, 0, 0)).Num(), 0);
    TestEqual(TEXT("A rect after the canvas touches no tile"), Map.GetTileIndexesForRect(FIntRect(150, 0, 200, 10)).Num(), 0);
    TestEqual(TEXT("The canvas touches every tile"), Map.GetTileIndexesForRect(FIntRect(0, 0, 150, 100)).Num(), 6);

    const TArray<FIntPoint> Corner = Map.GetTileIndexesForRect(FIntRect(128, 64, 150, 100));
    if (TestEqual(TEXT("The last partial tile"), Corner.Num(), 1))
    {
        TestEqual(TEXT("The last partial tile index"), Corner[0], FIntPoint(2, 1));
    }

    FOdysseyInvalidTileMap Invalid(64, 150, 100);
    Invalid.Invalidate(FIntRect(0, 0, 64, 64));
    const TArray<FIntRect> InvalidRects = Invalid.InvalidRects();
    if (TestEqual(TEXT("Invalidating an aligned tile gives one rect"), InvalidRects.Num(), 1))
    {
        TestEqual(TEXT("Invalidating an aligned tile invalidates this tile only"), InvalidRects[0], FIntRect(0, 0, 64, 64));
    }
    return true;
}

#endif
