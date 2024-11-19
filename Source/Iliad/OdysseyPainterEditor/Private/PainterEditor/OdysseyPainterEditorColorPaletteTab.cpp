// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyPainterEditorColorPaletteTab.h"

#include "SOdysseyPalette.h"
#include "OdysseyPainterEditor.h"

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
    , mPaletteWidget(nullptr)
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
    mPaletteWidget = SNew( SOdysseyPalette );

    return mPaletteWidget;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Public Getters

TSharedPtr<SOdysseyPalette>
FOdysseyPainterEditorPaletteTab::PaletteWidget()
{
    return mPaletteWidget;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners



#undef LOCTEXT_NAMESPACE
