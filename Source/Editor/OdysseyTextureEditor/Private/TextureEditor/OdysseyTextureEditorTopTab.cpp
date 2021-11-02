// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyTextureEditorTopTab.h"
#include "OdysseyTextureEditor.h"

/////////////////////////////////////////////////////
// FOdysseyTextureEditorTopTab
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyTextureEditorTopTab::~FOdysseyTextureEditorTopTab()
{
}

FOdysseyTextureEditorTopTab::FOdysseyTextureEditorTopTab(FOdysseyTextureEditor* iEditor)
	: FOdysseyPainterEditorTopTab(iEditor)
    , mEditor(iEditor)
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Widget Getters

::ULIS::eAlphaMode
FOdysseyTextureEditorTopTab::AlphaMode() const
{
    return mEditor->SelectedAlphaMode();
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Event Listeners


void
FOdysseyTextureEditorTopTab::SetAlphaMode(::ULIS::eAlphaMode iAlphaMode)
{
    mEditor->SelectedAlphaMode( iAlphaMode );
}

#undef LOCTEXT_NAMESPACE
