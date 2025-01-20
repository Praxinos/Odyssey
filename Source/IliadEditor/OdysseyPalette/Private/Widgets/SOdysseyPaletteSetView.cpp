// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SOdysseyPaletteSetView.h"

#include "Framework/Application/SlateApplication.h"
#include "SOdysseyPaletteSetTile.h"
#include "Framework/Commands/GenericCommands.h"
#include "OdysseyPalette.h"
#include "ToolMenus.h"

#define LOCTEXT_NAMESPACE "Palette"

static FName contextSetMenuName = "OdysseyPaletteSetContextMenu";

SOdysseyPaletteSetView::~SOdysseyPaletteSetView()
{
    UOdysseyPalette::OnSetsChanged().RemoveAll(this);
}

SOdysseyPaletteSetView::SOdysseyPaletteSetView():
    mCommandList(MakeShared<FUICommandList>())
{
    MapActionsToCommandList();
    UOdysseyPalette::OnSetsChanged().AddRaw(this, &SOdysseyPaletteSetView::OnPaletteSetsChanged);
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
            .OnMouseButtonClick(this, &SOdysseyPaletteSetView::OnItemClicked)
            .ClearSelectionOnClick(false)
            .OnGenerateTile(this, &SOdysseyPaletteSetView::OnGenerateTile)
            .OnSelectionChanged(this, &SOdysseyPaletteSetView::OnSelectionChanged )
            .OnContextMenuOpening(this, &SOdysseyPaletteSetView::OnContextMenuOpening )
        );

    }

    CreateContextMenu();

    if (mPalette)
        SelectSet( mPalette->Sets[mPalette->UsedSet] );
}

void
SOdysseyPaletteSetView::DeleteSelectedSet()
{
    if (!mPalette)
        return;

    mPalette->RemoveSet( mPalette->UsedSet );

    mPalette->UsedSet = 1;
    SelectSet(mPalette->Sets[0]);
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
    SelectSet(mPalette->Sets.Last());
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

void SOdysseyPaletteSetView::OnItemClicked(FName iSet)
{
    if (FSlateApplication::Get().GetPressedMouseButtons().Contains(EKeys::RightMouseButton))
    {
        CreateContextMenu();
    }
    else
    {
        SelectSet(iSet);
    }
}

TSharedRef<class ITableRow>
SOdysseyPaletteSetView::OnGenerateTile(FName iSet, const TSharedRef< class STableViewBase >& iTable)
{
    return SNew(SOdysseyPaletteSetTile, iTable)
        .UsedSet(iSet) //All sets one after the other
        .OnSelected(this, &SOdysseyPaletteSetView::SelectSet, iSet);
}

void
SOdysseyPaletteSetView::OnPaletteSetsChanged(UOdysseyPalette* iPalette)
{
    RequestListRefresh();
    Private_ClearSelection();
    Private_SetItemSelection(iPalette->GetUsedSet(), true, true);
    mOnSetSelected.ExecuteIfBound(iPalette->GetUsedSet());
}

void
SOdysseyPaletteSetView::SelectSet(FName iSet)
{
    RequestListRefresh();
    Private_ClearSelection();
    Private_SetItemSelection(iSet, true, true);
    mOnSetSelected.ExecuteIfBound(iSet);
}

void
SOdysseyPaletteSetView::OnSelectionChanged(FName iSet, ESelectInfo::Type iType)
{
    RequestListRefresh();
    Private_ClearSelection();
    Private_SetItemSelection(iSet, true, true);
    mOnSetSelected.ExecuteIfBound(iSet);
}

#undef LOCTEXT_NAMESPACE
