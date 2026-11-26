// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "CoreMinimal.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"

#include "OdysseyLegacyAssetGroup.h"

class SWindow;

/** What the user chose in the window. */
enum class EOdysseyTiledImageMigrationDecision : uint8
{
    /** Convert and save the chosen groups. */
    Convert,
    /** Close the window; the conversion is offered again at the next startup. */
    Later,
    /** Close the window; the conversion is no longer offered at startup. */
    DoNotAskAgain,
};

/** What the user chose in the window, and which groups to convert. */
struct FOdysseyTiledImageMigrationChoice
{
    /** Later when the window is closed without a button. */
    EOdysseyTiledImageMigrationDecision Decision = EOdysseyTiledImageMigrationDecision::Later;

    /** The indexes of the groups to convert, when the decision is Convert. */
    TArray<int32> GroupIndexes;
};

/** Lets the user choose the groups of legacy assets to convert. */
class SOdysseyTiledImageMigrationWindow
    : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTiledImageMigrationWindow) {}
        /** The groups of legacy assets. */
        SLATE_ARGUMENT(TArray<FOdysseyLegacyAssetGroup>, Groups)
        /** The window holding the widget, closed once the user chose. */
        SLATE_ARGUMENT(TWeakPtr<SWindow>, ParentWindow)
    SLATE_END_ARGS()

    /** Builds one row per group: the project group starts checked when it can be converted, and the groups of engine plugins cannot be checked. */
    void Construct(const FArguments& InArgs);

    /** Shows the window as a modal window, and returns what the user chose. */
    static FOdysseyTiledImageMigrationChoice ShowModal(TArray<FOdysseyLegacyAssetGroup> InGroups);

private:
    /** Returns the row of the group at GroupIndex, whose check box chooses the group for the conversion. */
    [[nodiscard]] TSharedRef<SWidget> CreateGroupRow(int32 GroupIndex);

    /** Returns the buttons of the window. */
    [[nodiscard]] TSharedRef<SWidget> CreateButtons();

    /** Records the choice and closes the window. */
    FReply Close(EOdysseyTiledImageMigrationDecision Decision);

    /** Returns true when at least one group is checked. */
    bool IsAnyGroupChecked() const;

private:
    /** The groups shown, one row each; CheckedGroups and Choice.GroupIndexes index into it. */
    TArray<FOdysseyLegacyAssetGroup> Groups;

    /** Whether each group is checked, by group index. */
    TArray<bool> CheckedGroups;

    /** The window holding the widget. */
    TWeakPtr<SWindow> ParentWindow;

    /** Set by Close(); returned by ShowModal(). */
    FOdysseyTiledImageMigrationChoice Choice;
};
