// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "OdysseyPainterEditorRasterSelection.h"

#if WITH_DEV_AUTOMATION_TESTS

#include "Editor.h"
#include "Misc/AutomationTest.h"
#include "ScopedTransaction.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "OdysseyTiledImage.h"

namespace Odyssey::RasterSelectionTests
{
constexpr EAutomationTestFlags TestFlags = EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter;
constexpr int32 Width = 200;
constexpr int32 Height = 150;

/** Returns the mask value at a pixel. */
float MaskAt(UOdysseyPainterEditorRasterSelection& Selection, int32 X, int32 Y)
{
    TSharedPtr<::ULIS::FBlock> Mask = Selection.GetBlock();
    return Mask ? reinterpret_cast<const float*>(Mask->ScanlineBits(static_cast<uint16>(Y)))[X] : -1.0f;
}

/** Returns the tiled image holding the mask, through its property. */
const UOdysseyTiledImage* MaskImage(UOdysseyPainterEditorRasterSelection& Selection)
{
    const FObjectProperty* Property = FindFProperty<FObjectProperty>(Selection.GetClass(), TEXT("MaskImage"));
    return Property ? Cast<UOdysseyTiledImage>(Property->GetObjectPropertyValue_InContainer(&Selection)) : nullptr;
}
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyRasterSelectionTest, "Odyssey.PainterEditor.RasterSelection", Odyssey::RasterSelectionTests::TestFlags)
/**
 * The raster selection holds its mask in a tiled image: changes are undoable, the derived mask follows undo and redo
 * without changing identity, and unselected areas hold no tile.
 */
bool FOdysseyRasterSelectionTest::RunTest(const FString& Parameters)
{
    using namespace Odyssey::RasterSelectionTests;
    TStrongObjectPtr<UOdysseyPainterEditorRasterSelection> Selection(NewObject<UOdysseyPainterEditorRasterSelection>(GetTransientPackage(), NAME_None, RF_Transactional));
    Selection->Init(Width, Height);
    TestTrue(TEXT("A new selection is empty"), Selection->IsEmpty());
    const TSharedPtr<::ULIS::FBlock> Mask = Selection->GetBlock();
    const UOdysseyTiledImage* Image = MaskImage(*Selection);
    if (!TestTrue(TEXT("The selection has a mask"), Mask.IsValid()) || !TestNotNull(TEXT("The mask is held by a tiled image"), Image))
    {
        return false;
    }

    int32 Notifications = 0;
    Selection->OnChanged().AddLambda([&Notifications]() { ++Notifications; });
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyRasterSelectionTests", "Add", "Select"));
        Selection->Add({ FVector2D(10, 10), FVector2D(50, 10), FVector2D(50, 40), FVector2D(10, 40) });
    }
    TestFalse(TEXT("The selection holds the polygon"), Selection->IsEmpty());
    TestEqual(TEXT("A pixel inside is selected"), MaskAt(*Selection, 30, 25), 1.0f);
    TestEqual(TEXT("A pixel outside is not selected"), MaskAt(*Selection, 120, 100), 0.0f);
    TestTrue(TEXT("Changes are notified"), Notifications > 0);
    TestTrue(TEXT("Unselected areas hold no tile"), Image->GetTileBounds().Max.X <= 64 && Image->GetTileBounds().Max.Y <= 64);

    TestTrue(TEXT("Undo the selection"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo empties the selection"), Selection->IsEmpty());
    TestEqual(TEXT("Undo clears the mask"), MaskAt(*Selection, 30, 25), 0.0f);
    TestTrue(TEXT("The mask keeps its identity, which tools hold"), Selection->GetBlock() == Mask);

    TestTrue(TEXT("Redo the selection"), GEditor->RedoTransaction());
    TestFalse(TEXT("Redo restores the selection"), Selection->IsEmpty());
    TestEqual(TEXT("Redo restores the mask"), MaskAt(*Selection, 30, 25), 1.0f);

    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyRasterSelectionTests", "Invert", "Invert selection"));
        Selection->Invert();
    }
    TestEqual(TEXT("Inverting deselects the polygon"), MaskAt(*Selection, 30, 25), 0.0f);
    TestEqual(TEXT("Inverting selects the rest"), MaskAt(*Selection, 120, 100), 1.0f);
    TestTrue(TEXT("Undo the inversion"), GEditor->UndoTransaction());
    TestEqual(TEXT("Undone inversion"), MaskAt(*Selection, 120, 100), 0.0f);
    return true;
}

#endif
