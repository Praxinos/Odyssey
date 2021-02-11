// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorColorSlidersTab.h"

#include "Color/SOdysseyColorSliders.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorColorSlidersTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorColorSlidersTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorColorSlidersTab::~FOdysseyPainterEditorColorSlidersTab()
{
}

FOdysseyPainterEditorColorSlidersTab::FOdysseyPainterEditorColorSlidersTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_ColorSliders"),
                            LOCTEXT( "OdysseyPainterEditorColorSlidersTab", "Color Sliders" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.ColorSliders_2_16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorColorSlidersTab::CreateWidget()
{
	return SNew( SOdysseyColorSliders )
		.Color_Raw(mEditor, &FOdysseyPainterEditor::PaintColor)
        .OnColorChange_Raw(this, &FOdysseyPainterEditorColorSlidersTab::OnColorChange);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyPainterEditorColorSlidersTab::OnColorChange( eOdysseyEventState::Type iEventState, const ::ul3::FPixelValue& iColor )
{
    mEditor->PaintColor(iColor);

    if (iEventState == eOdysseyEventState::kSet)
    {
	    mEditor->PaintEngine()->SetColor( iColor );
    }
}

#undef LOCTEXT_NAMESPACE
