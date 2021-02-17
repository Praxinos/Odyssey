// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyFlipbookEditorTopTab.h"

#include "OdysseyFlipbookEditor.h"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorTopTab::~FOdysseyFlipbookEditorTopTab()
{
}

FOdysseyFlipbookEditorTopTab::FOdysseyFlipbookEditorTopTab(FOdysseyFlipbookEditor* iEditor)
	: FOdysseyPainterEditorTopTab(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ul3::eAlphaMode
FOdysseyFlipbookEditorTopTab::AlphaMode() const
{
    return mEditor->SelectedAlphaMode();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners

void
FOdysseyFlipbookEditorTopTab::OnAlphaModeChanged( int32 iValue )
{
    mEditor->SelectedAlphaMode(static_cast<::ul3::eAlphaMode>(iValue));
}

#undef LOCTEXT_NAMESPACE
