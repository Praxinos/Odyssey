// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorColorPaletteTab.h"

#include "OdysseyPalette.h"
#include "SOdysseyPalette.h"
#include "OdysseyPainterEditor.h"

#include "UObject/OdysseyObjectEditorUtils.h"

#define LOCTEXT_NAMESPACE "PainterEditor"

const FName&
FOdysseyPainterEditorPaletteTab::StaticId()
{
    static FName Id = TEXT("OdysseyPainterEditor_Palette");
    return Id;
}

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorPaletteTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorPaletteTab::~FOdysseyPainterEditorPaletteTab()
{
}

FOdysseyPainterEditorPaletteTab::FOdysseyPainterEditorPaletteTab(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorTab(LOCTEXT( "palette-tab.name", "Palette" ), FSlateIcon( "OdysseyStyle", "OdysseyPalette.PaletteTab" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

const FName&
FOdysseyPainterEditorPaletteTab::GetId() const
{
    return StaticId();
}

TSharedPtr<SWidget>
FOdysseyPainterEditorPaletteTab::CreateWidget()
{
    /* return SNew( SOdysseyPalette )
        .Palette(this, &FOdysseyPainterEditorPaletteTab::GetPalette)
        .CurrentColorEntry(this, &FOdysseyPainterEditorPaletteTab::GetPaletteCurrentColorEntry)
        .CurrentSet(this, &FOdysseyPainterEditorPaletteTab::GetPaletteCurrentSet)
        .OnPaletteChanged(this, &FOdysseyPainterEditorPaletteTab::OnPaletteChanged)
        .OnCurrentColorEntrySelected(this, &FOdysseyPainterEditorPaletteTab::OnPaletteCurrentColorEntrySelected)
        .OnCurrentSetSelected(this, &FOdysseyPainterEditorPaletteTab::OnPaletteCurrentSetSelected); */

    return SNullWidget::NullWidget;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Public Getters

/*const FOdysseyPaletteSetEntryColor&
FOdysseyPainterEditorPaletteTab::GetPaletteCurrentColorEntry() const
{
    return mEditor->GetPaletteCurrentColorEntry();
}

void
FOdysseyPainterEditorPaletteTab::OnPaletteChanged(UOdysseyPalette* iPalette) const
{
    return mEditor->SetPalette(iPalette);
}

void
FOdysseyPainterEditorPaletteTab::OnPaletteCurrentColorEntrySelected(UOdysseyPaletteEntryColor* iEntry) const
{
    return mEditor->SetPaletteCurrentColorEntry(iEntry);
}

void
FOdysseyPainterEditorPaletteTab::OnPaletteCurrentSetSelected(int iSet) const
{
    return mEditor->SetPaletteCurrentSet(iSet);
} */

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
