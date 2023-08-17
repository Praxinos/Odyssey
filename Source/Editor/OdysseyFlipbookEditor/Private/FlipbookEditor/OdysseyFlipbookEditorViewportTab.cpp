// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyFlipbookEditorViewportTab.h"
/*
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

UTexture*
FOdysseyFlipbookEditorViewportTab::Texture() const
{
    //TODO: Instead of going through the GUI, make a Player class in the data and get the condition from there
    if (!mEditor->GetGUI()->GetTimelineTab()->Timeline())
        return nullptr;

    if (mEditor->GetGUI()->GetTimelineTab()->Timeline()->IsScrubbing())
        return mEditor->PreviewTexture();

    if (!mEditor->GetSource())
        return nullptr;

    return mEditor->GetSource()->DisplayTexture();
} */

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

#undef LOCTEXT_NAMESPACE
