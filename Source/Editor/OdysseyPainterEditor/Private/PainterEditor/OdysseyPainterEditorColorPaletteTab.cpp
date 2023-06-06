// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorColorPaletteTab.h"

#include "Palette/SOdysseyPalette.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorColorPaletteTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorPaletteTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorPaletteTab::~FOdysseyPainterEditorPaletteTab()
{
}

FOdysseyPainterEditorPaletteTab::FOdysseyPainterEditorPaletteTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_Palette"),
                            LOCTEXT( "OdysseyPainterEditorPaletteTab", "Palette" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.Mesh16" ))
    , mEditor(iEditor)
    , mPaletteWidget(nullptr)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

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
