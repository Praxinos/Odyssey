// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteFolderRow.h"
#include "UObject/OdysseyObjectEditorUtils.h"
#include "OdysseyStyleSet.h"
#include "SOdysseyPaletteEntryRow.h"
#include "Widgets/SOdysseyPaletteTreeView.h"

//CONSTRUCTION/DESTRUCTION----------------------------------------------- SMultiColumnTableRow
void SOdysseyPaletteFolderRow::Construct(const FArguments& InArgs, const TSharedRef<SOdysseyPaletteTreeView>& iOwnerTableView, UOdysseyPaletteEntryFolder* iFolderEntry)
{
    ensure(iFolderEntry);
    mFolderEntry = iFolderEntry;

    SOdysseyPaletteEntryRow::Construct(
        SOdysseyPaletteEntryRow::FArguments(),
        iOwnerTableView,
        iFolderEntry
    );

	SignalSelectionMode = ETableRowSignalSelectionMode::Instantaneous;
}

//PRIVATE API-----------------------------------------------------------

TSharedRef<SWidget>
SOdysseyPaletteFolderRow::GenerateHeaderWidget()
{
	TSharedRef<SWidget> defaultWidget = SOdysseyPaletteEntryRow::GenerateHeaderWidget();
	return SNew(SHorizontalBox)
		+ SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
		[
			//LayerName
            SOdysseyPaletteEntryRow::GenerateHeaderWidget()
		];
}
