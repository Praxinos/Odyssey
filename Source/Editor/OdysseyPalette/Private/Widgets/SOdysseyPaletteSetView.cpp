// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteSetView.h"
#include "SOdysseyPaletteSetTile.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPalette.h"

#define LOCTEXT_NAMESPACE "Palette"

static FName contextSetMenuName = "OdysseyPaletteSetContextMenu";

SOdysseyPaletteSetView::~SOdysseyPaletteSetView()
{
}

SOdysseyPaletteSetView::SOdysseyPaletteSetView():
    mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
}

//CONSTRUCTION/DESTRUCTION-----------------------------------------------
void
SOdysseyPaletteSetView::Construct(const FArguments& InArgs)
{
    mPalette = InArgs._Palette;
    mOnSetSelected = InArgs._OnSetSelected;

    if (mPalette)
    {
        STileView<FName>::Construct(
            STileView<FName>::FArguments()
            .ItemWidth(32)
            .ItemHeight(32)
            .ListItemsSource(&(mPalette->Sets))
            .ItemAlignment(EListItemAlignment::LeftAligned)
            .SelectionMode(ESelectionMode::Single)
            .OnGenerateTile(this, &SOdysseyPaletteSetView::OnGenerateTile)
            .OnContextMenuOpening(this, &SOdysseyPaletteSetView::OnContextMenuOpening)
        );
    }

    CreateContextMenu();

    if (mPalette)
        OnSetSelected(mPalette->Sets[mPalette->UsedSet] );
}

void
SOdysseyPaletteSetView::DeleteSelectedSet()
{
    if (!mPalette)
        return;

    mPalette->RemoveSet( mPalette->UsedSet );

    mPalette->UsedSet = 1;
    OnSetSelected(mPalette->Sets[0]);
}

bool
SOdysseyPaletteSetView::CanDeleteSelectedSet()
{
    if (!mPalette)
        return false;

    if( mPalette->Sets.Num() != 1 )
        return true;

    return false;
}

void
SOdysseyPaletteSetView::DuplicateSelectedSet()
{
    if (!mPalette)
        return;

    mPalette->DuplicateSet();
    OnSetSelected(mPalette->Sets.Last());
}

bool
SOdysseyPaletteSetView::CanDuplicateSelectedSet()
{
    if( !mPalette )
        return false;

    if (mPalette->Sets.Num() < 8)
        return true;

    return false;
}


TArray<TSharedPtr<FExtender>> SOdysseyPaletteSetView::ExtendContextMenu()
{
    return TArray< TSharedPtr<FExtender> >();
}


void
SOdysseyPaletteSetView::MapActionsToCommandList()
{
    mCommandList->MapAction(
        FGenericCommands::Get().Delete,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteSetView::DeleteSelectedSet),
        FCanExecuteAction::CreateRaw(this, &SOdysseyPaletteSetView::CanDeleteSelectedSet)
    );

    mCommandList->MapAction(
        FGenericCommands::Get().Duplicate,
        FExecuteAction::CreateRaw(this, &SOdysseyPaletteSetView::DuplicateSelectedSet),
        FCanExecuteAction::CreateRaw(this, &SOdysseyPaletteSetView::CanDuplicateSelectedSet)
    );
}


void SOdysseyPaletteSetView::CreateContextMenu()
{
    UToolMenus* ToolMenus = UToolMenus::Get();
    if (!ensure(ToolMenus))
        return;

    if (ToolMenus->IsMenuRegistered(contextSetMenuName))
        return;

    UToolMenu* Menu = ToolMenus->RegisterMenu(contextSetMenuName);

    FToolMenuSection& commonSection = Menu->AddSection("Common", LOCTEXT("set-view.context-menu.common-section.name", "Common"));
    {
        commonSection.AddMenuEntry(FGenericCommands::Get().Delete);
        commonSection.AddMenuEntry(FGenericCommands::Get().Duplicate);
    }
}

TSharedPtr<SWidget> SOdysseyPaletteSetView::OnContextMenuOpening()
{
    //Create a new command, so that we can add context menu specific entries 
    TSharedRef<FUICommandList> commandList = MakeShared<FUICommandList>();
    commandList->Append(mCommandList);

    //Allows us to extend the menu context by inserting entries everywhere we want
    //Overriding CreateContextMenu does not allow that
    TArray<TSharedPtr<FExtender>> extenders = ExtendContextMenu();
    TSharedPtr<FExtender> extender = FExtender::Combine(extenders);

    //Build menu
    FToolMenuContext menuContext(commandList, extender);
    return UToolMenus::Get()->GenerateWidget(contextSetMenuName, menuContext);
}

TSharedRef<class ITableRow>
SOdysseyPaletteSetView::OnGenerateTile(FName iSet, const TSharedRef< class STableViewBase >& iTable)
{
    return SNew(SOdysseyPaletteSetTile, iTable)
        .UsedSet(iSet) //All sets one after the other
        .OnSelected(this, &SOdysseyPaletteSetView::OnSetSelected, iSet);
}

void
SOdysseyPaletteSetView::OnSetSelected(FName iSet)
{
    RequestListRefresh();
    Private_ClearSelection();
    Private_SetItemSelection(iSet, true, true);
    mOnSetSelected.ExecuteIfBound(iSet);
}

#undef LOCTEXT_NAMESPACE
