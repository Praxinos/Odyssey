// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorViewportTab.h"

#include "OdysseyFlipbookEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyFlipbookEditorViewportTab"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorViewportTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorViewportTab::~FOdysseyFlipbookEditorViewportTab()
{
}

FOdysseyFlipbookEditorViewportTab::FOdysseyFlipbookEditorViewportTab(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyPainterEditorViewportTab(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getters

IOdysseySurface*
FOdysseyFlipbookEditorViewportTab::Surface() const
{
    //TODO: Instead of going through the GUI, make a Player class in the data and get the condition from there
    return mEditor->GetGUI()->GetTimelineTab()->Timeline()->IsScrubbing() ? (IOdysseySurface*)mEditor->PreviewSurface() : (IOdysseySurface*)mEditor->DisplaySurface();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
