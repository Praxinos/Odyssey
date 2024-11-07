// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Widgets/Views/STileView.h"

class UOdysseyPalette;

class ODYSSEYPALETTE_API SOdysseyPaletteSetView
    : public STileView<FName>
{

public:
    DECLARE_DELEGATE_OneParam( FOnSetSelected, FName);

public:
    SLATE_BEGIN_ARGS(SOdysseyPaletteSetView)
        {}
        SLATE_ARGUMENT( UOdysseyPalette*, Palette )
        SLATE_EVENT( FOnSetSelected, OnSetSelected )
    SLATE_END_ARGS()

public:
    // Construction / Destruction
    ~SOdysseyPaletteSetView();
    SOdysseyPaletteSetView();
    
    void Construct(const FArguments& InArgs);
    void OnSelectionChanged( FName iSet, ESelectInfo::Type iType );
    void OnPaletteSetsChanged(UOdysseyPalette* iPalette);

public:
    //Setters/Getters
    void SelectSet(FName iSet);

protected:    
    void DeleteSelectedSet();
    
    bool CanDeleteSelectedSet();
    
    void DuplicateSelectedSet();

    bool CanDuplicateSelectedSet();

    /**
     * @brief Extends the context menu
     * Allows us to insert entries wherever we want in the context menu
     * CreateContextMenu() does not allow us to do that
     */
    virtual TArray<TSharedPtr<FExtender>> ExtendContextMenu();

    void MapActionsToCommandList();

    /**
     * @brief Creates and registers palette set context menu
     *
     */
    virtual void CreateContextMenu();

private:
    //Callbacks
    /**
     * @brief Called when the view asks for a contextmenu to be opened (rightclick)
     *
     * @return TSharedPtr<SWidget>
     */
    TSharedPtr<SWidget> OnContextMenuOpening();

    void OnItemClicked(FName iSet);

    TSharedRef<class ITableRow> OnGenerateTile(FName iSet, const TSharedRef< class STableViewBase >& iTable);

private:
    UOdysseyPalette* mPalette;
    FOnSetSelected mOnSetSelected;
    TSharedRef<FUICommandList> mCommandList;
};
