// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "Editor/TransBuffer.h"
#include "Elements/Framework/TypedElementSelectionSet.h"
#include "Framework/Commands/UICommandList.h"
#include "Misc/AutomationTest.h"
#include "ScopedTransaction.h"
#include "Selection.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"

#include "OdysseyAnimation.h"
#include "OdysseyAnimationLayerImageRaster.h"
#include "OdysseyAnimationLayerStackEditorCommands.h"
#include "OdysseyAnimationLayerStackGlobalShortcuts.h"
#include "OdysseyAnimationCellsSelection.h"
#include "OdysseyAnimationCellImageStagger.h"
#include "OdysseyLayerStackSelection.h"
#include "OdysseyAnimationEditorData.h"

namespace
{
/** Preserves the caller's selection and cursor while tests exercise the shared selection object. */
class FScopedAnimationSelection
{
public:
    ~FScopedAnimationSelection()
    {
        USelection* Selection = OdysseyLayerStackSelection::Get();
        Selection->BeginBatchSelectOperation();
        Selection->DeselectAll();
        for (const TStrongObjectPtr<UObject>& Object : PreviousObjects)
            Selection->Select(Object.Get());
        Selection->EndBatchSelectOperation();
        OdysseyAnimationCellsSelection::SetCellSelectionCursor(PreviousCursor.Get());
    }

    FScopedAnimationSelection()
        : PreviousCursor(OdysseyAnimationCellsSelection::GetCellSelectionCursor())
    {
        TArray<UObject*> Objects;
        OdysseyLayerStackSelection::Get()->GetSelectedObjects(Objects);
        for (UObject* Object : Objects)
            PreviousObjects.Emplace(Object);
        Select({});
    }

    /** Replaces the selection and sends the normal batched selection notification. */
    static void Select(const TArray<UObject*>& Objects)
    {
        USelection* Selection = OdysseyLayerStackSelection::Get();
        Selection->BeginBatchSelectOperation();
        Selection->DeselectAll();
        for (UObject* Object : Objects)
            Selection->Select(Object);
        Selection->EndBatchSelectOperation();
    }

private:
    TStrongObjectPtr<UOdysseyAnimationCell> PreviousCursor;
    TArray<TStrongObjectPtr<UObject>> PreviousObjects;
};
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationSelectionTest, "Odyssey.Animation.Editor.CellSelection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Verifies selection callbacks and undo through the animation editor and common layer selection. */
bool FOdysseyAnimationSelectionTest::RunTest(const FString& Parameters)
{
    FScopedAnimationSelection RestoreSelection;
    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    UOdysseyAnimationLayerStack* Stack = CastChecked<UOdysseyAnimationLayerStack>(OdysseyAnimationEditor::GetLayerStack(Animation.Get()));
    UOdysseyAnimationLayer* FirstLayer = CastChecked<UOdysseyAnimationLayer>(Stack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationLayer* SecondLayer = CastChecked<UOdysseyAnimationLayer>(Stack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationCell* First = FirstLayer->AddCell(UOdysseyAnimationCellImageStagger::StaticClass());
    UOdysseyAnimationCell* Second = SecondLayer->AddCell(UOdysseyAnimationCellImageStagger::StaticClass());
    Stack->SetCurrentLayer(FirstLayer);
    FScopedAnimationSelection::Select({First});
    TestTrue(TEXT("Cell editor selection uses the shared object"), First->IsSelected());
    TestEqual(TEXT("Selected cell becomes cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor(), First);
    TestEqual(TEXT("Layer filter includes its cells"), OdysseyAnimationCellsSelection::GetSelectedCells(FirstLayer).Num(), 1);
    TestTrue(TEXT("Layer filter excludes other cells"), OdysseyAnimationCellsSelection::GetSelectedCells(SecondLayer).IsEmpty());
    TestTrue(TEXT("Cell selection contains no selected layers"), OdysseyLayerStackSelection::GetSelectedLayers(nullptr, false).IsEmpty());

    /** Keep the user's transaction history intact by exercising a separate transaction buffer. */
    TStrongObjectPtr<UTransBuffer> TestBuffer(NewObject<UTransBuffer>(GetTransientPackage()));
    TestBuffer->Initialize(1024 * 1024);
    TStrongObjectPtr<UTransactor> OriginalBuffer(GEditor->Trans.Get());
    TGuardValue<TObjectPtr<UTransactor>> TransactionBufferGuard(GEditor->Trans, TestBuffer.Get());
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyAnimationSelectionTests", "Selection", "Change cell selection"));
        FScopedAnimationSelection::Select({Second});
        OdysseyAnimationCellsSelection::RegisterUndo({First}, {Second});
    }
    TestTrue(TEXT("Undo cell selection"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo restores the first cell"), OdysseyAnimationCellsSelection::GetSelectedCells(nullptr) == TArray<UOdysseyAnimationCell*>({First}));
    TestEqual(TEXT("Undo restores the cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor(), First);
    TestTrue(TEXT("Redo cell selection"), GEditor->RedoTransaction());
    TestEqual(TEXT("Redo restores the cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor(), Second);

    Stack->SetCurrentLayer(SecondLayer);
    TestTrue(TEXT("Current layer changes clear cells"), OdysseyAnimationCellsSelection::GetSelectedCells(nullptr).IsEmpty());
    TestNull(TEXT("Current layer changes clear cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor());
    FScopedAnimationSelection::Select({Second});
    SecondLayer->RemoveCell(Second);
    TestTrue(TEXT("Removed cells leave selection"), OdysseyAnimationCellsSelection::GetSelectedCells(nullptr).IsEmpty());
    TestNull(TEXT("Removed cells leave cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor());
    TestEqual(TEXT("Removed garbage cells release their selection element"), OdysseyLayerStackSelection::Get()->GetElementSelectionSet()->GetNumSelectedElements(), 0);
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationGarbageSelectionTest, "Odyssey.Animation.Editor.GarbageSelection",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Collects a selected object marked as garbage, which USelection can neither report nor deselect, and checks the selection releases it first. */
bool FOdysseyAnimationGarbageSelectionTest::RunTest(const FString& Parameters)
{
    FScopedAnimationSelection RestoreSelection;
    UOdysseyAnimationCell* Cell = NewObject<UOdysseyAnimationCellImageStagger>(GetTransientPackage());
    FScopedAnimationSelection::Select({Cell});
    TestEqual(TEXT("Selected cell becomes cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor(), Cell);

    UTypedElementSelectionSet* SelectionSet = OdysseyLayerStackSelection::Get()->GetElementSelectionSet();
    Cell->MarkAsGarbage();
    TestTrue(TEXT("Garbage cell is hidden from USelection"), OdysseyAnimationCellsSelection::GetSelectedCells(nullptr).IsEmpty());
    TestEqual(TEXT("Garbage cell is still held by the selection set"), SelectionSet->GetNumSelectedElements(), 1);

    /** The engine destroys the element of the collected cell at the end of the frame; it must not be referenced anymore by then. */
    CollectGarbage(GARBAGE_COLLECTION_KEEPFLAGS);
    TestEqual(TEXT("Garbage collection releases the garbage cell selection"), SelectionSet->GetNumSelectedElements(), 0);
    TestNull(TEXT("Garbage collection clears the cell cursor"), OdysseyAnimationCellsSelection::GetCellSelectionCursor());
    return true;
}

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyAnimationLayerShortcutsTest, "Odyssey.Animation.Editor.LayerShortcuts",
    EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Exercises command mapping, selected layers, and a focused layer outside the selection. */
bool FOdysseyAnimationLayerShortcutsTest::RunTest(const FString& Parameters)
{
    FScopedAnimationSelection RestoreSelection;
    TStrongObjectPtr<UOdysseyAnimation> Animation(NewObject<UOdysseyAnimation>(GetTransientPackage()));
    Animation->Init(16, 16, EOdysseyAnimationFormat::BGRA8, 24.f);
    UOdysseyAnimationLayerStack* Stack = CastChecked<UOdysseyAnimationLayerStack>(OdysseyAnimationEditor::GetLayerStack(Animation.Get()));
    UOdysseyAnimationLayer* First = CastChecked<UOdysseyAnimationLayer>(Stack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationLayer* Second = CastChecked<UOdysseyAnimationLayer>(Stack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    UOdysseyAnimationLayer* Focused = CastChecked<UOdysseyAnimationLayer>(Stack->AddLayer(UOdysseyAnimationLayerImageRaster::StaticClass()));
    Stack->SetCurrentLayer(First);
    FScopedAnimationSelection::Select({First, Second});
    FOdysseyAnimationLayerStackGlobalShortcuts Shortcuts{TAttribute<UOdysseyLayerStack*>(Stack)};
    TSharedRef<FUICommandList> Commands = MakeShared<FUICommandList>();
    Shortcuts.MapActionsToCommandList(Commands);
    const FOdysseyAnimationLayerStackEditorCommands& AnimationCommands = FOdysseyAnimationLayerStackEditorCommands::Get();
    TestTrue(TEXT("Execute mapped lighttable activation"), Commands->ExecuteAction(AnimationCommands.ActivateLayerLighttable.ToSharedRef()));
    TestTrue(TEXT("Activation affects selected layers"), First->GetLighttable().bIsActivated && Second->GetLighttable().bIsActivated);
    TestFalse(TEXT("Unselected layer stays unchanged"), Focused->GetLighttable().bIsActivated);
    TestTrue(TEXT("Execute mapped lighttable toggle"), Commands->ExecuteAction(AnimationCommands.ToggleLayerLighttable.ToSharedRef()));
    TestFalse(TEXT("Toggle follows the current layer"), First->GetLighttable().bIsActivated || Second->GetLighttable().bIsActivated);

    TAttribute<UOdysseyLayer*> FocusedAttribute = TAttribute<UOdysseyLayer*>::CreateLambda([Focused]() -> UOdysseyLayer* { return Focused; });
    FOdysseyAnimationLayerStackGlobalShortcuts FocusedShortcuts(TAttribute<UOdysseyLayerStack*>(Stack), FocusedAttribute);
    FocusedShortcuts.Action_ActivateLayerLighttable();
    TestTrue(TEXT("Focused layer outside selection is activated"), Focused->GetLighttable().bIsActivated);
    TestFalse(TEXT("Focus preserves other layers"), First->GetLighttable().bIsActivated || Second->GetLighttable().bIsActivated);
    FocusedShortcuts.Action_InactivateLayerLighttable();
    TestFalse(TEXT("Focused layer is deactivated"), Focused->GetLighttable().bIsActivated);
    FOdysseyAnimationLayerStackGlobalShortcuts EmptyShortcuts{TAttribute<UOdysseyLayerStack*>(nullptr)};
    EmptyShortcuts.Action_ActivateLayerLighttable();
    EmptyShortcuts.Action_ToggleLayerLighttable();
    TestFalse(TEXT("Missing stack changes no layer"), Focused->GetLighttable().bIsActivated);
    return true;
}
#endif
