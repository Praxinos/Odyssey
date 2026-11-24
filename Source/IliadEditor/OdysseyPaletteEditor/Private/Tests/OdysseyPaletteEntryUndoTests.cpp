// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#if WITH_DEV_AUTOMATION_TESTS
#include "Editor.h"
#include "Misc/AutomationTest.h"
#include "ScopedTransaction.h"
#include "UObject/Package.h"
#include "UObject/StrongObjectPtr.h"
#include "UObject/UnrealType.h"

#include "Palette/OdysseyPaletteEntryColor.h"

IMPLEMENT_SIMPLE_AUTOMATION_TEST(FOdysseyPaletteEntryColorUndoNotificationsTest, "Odyssey.Palette.EntryColor.UndoNotifications", EAutomationTestFlags::EditorContext | EAutomationTestFlags::EngineFilter)

/** Renaming a color entry, then undoing and redoing the rename, notifies the name change once each time. */
bool FOdysseyPaletteEntryColorUndoNotificationsTest::RunTest(const FString& Parameters)
{
    TStrongObjectPtr<UOdysseyPaletteEntryColor> Entry(NewObject<UOdysseyPaletteEntryColor>(GetTransientPackage(), NAME_None, RF_Transactional));
    FProperty* NameProperty = FindFProperty<FProperty>(UOdysseyPaletteEntry::StaticClass(), GET_MEMBER_NAME_CHECKED(UOdysseyPaletteEntry, EntryName));
    if (!TestNotNull(TEXT("The entry has a name property"), NameProperty))
    {
        return false;
    }

    int32 Notifications = 0;
    const FDelegateHandle Handle = UOdysseyPaletteEntry::OnNameChanged().AddLambda([&Notifications, &Entry](UOdysseyPaletteEntry* InEntry)
    {
        if (InEntry == Entry.Get())
        {
            ++Notifications;
        }
    });

    const FText OriginalName = Entry->EntryName;
    {
        FScopedTransaction Transaction(NSLOCTEXT("OdysseyPaletteEntryUndoTests", "Rename", "Rename entry"));
        Entry->Modify();
        Entry->EntryName = FText::FromString(TEXT("Renamed"));
        FPropertyChangedEvent Event(NameProperty);
        Entry->PostEditChangeProperty(Event);
    }
    TestEqual(TEXT("The rename is notified once"), Notifications, 1);

    Notifications = 0;
    TestTrue(TEXT("Undo the rename"), GEditor->UndoTransaction());
    TestTrue(TEXT("Undo restores the name"), Entry->EntryName.EqualTo(OriginalName));
    TestEqual(TEXT("The undo is notified once"), Notifications, 1);

    Notifications = 0;
    TestTrue(TEXT("Redo the rename"), GEditor->RedoTransaction());
    TestTrue(TEXT("Redo renames the entry again"), Entry->EntryName.ToString() == TEXT("Renamed"));
    TestEqual(TEXT("The redo is notified once"), Notifications, 1);

    UOdysseyPaletteEntry::OnNameChanged().Remove(Handle);
    return true;
}

#endif
