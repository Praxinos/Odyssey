// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorStrokeOptionsTab.h"

#include "SOdysseyStrokeOptions.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorStrokeOptionsTab"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorStrokeOptionsTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorStrokeOptionsTab::~FOdysseyPainterEditorStrokeOptionsTab()
{
}

FOdysseyPainterEditorStrokeOptionsTab::FOdysseyPainterEditorStrokeOptionsTab(FOdysseyPainterEditor* iEditor)
	: FOdysseyPainterEditorTab(TEXT("OdysseyPainterEditor_StrokeOptions"),
                            LOCTEXT( "OdysseyPainterEditorStrokeOptionsTab", "Stroke Options" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyPainterEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorStrokeOptionsTab::CreateWidget()
{
	return SNew(SOdysseyStrokeOptions)
		.PaintEngine(this, &FOdysseyPainterEditorStrokeOptionsTab::PaintEngine);
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

FOdysseyPaintEngine*
FOdysseyPainterEditorStrokeOptionsTab::PaintEngine() const
{
	return mEditor->PaintEngine();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
