// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorStrokeOptionsTab.h"

#include "Tools/Widgets/SOdysseyToolOptions.h"
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
	: FOdysseyEditorTab(TEXT("OdysseyPainterEditor_StrokeOptions"),
                            LOCTEXT( "OdysseyPainterEditorStrokeOptionsTab", "Stroke Options" ),
                            FSlateIcon( "OdysseyStyle", "PainterEditor.StrokeOptions16" ))
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------- FOdysseyEditorTab interface

TSharedPtr<SWidget>
FOdysseyPainterEditorStrokeOptionsTab::CreateWidget()
{
    return SNew( SVerticalBox )
        + SVerticalBox::Slot()
        .FillHeight(1.0f)
        [
            SNew(SOdysseyToolOptions)
		    .Tool(mEditor->GetSelectedTool())
        ];
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
