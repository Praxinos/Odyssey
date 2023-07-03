// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorContextMenu.h"
#include "OdysseyPainterEditor.h"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorContextMenu::~FOdysseyPainterEditorContextMenu()
{

}

FOdysseyPainterEditorContextMenu::FOdysseyPainterEditorContextMenu( FOdysseyPainterEditor* iEditor
                                                                  , FName iID
                                                                  , FText iDisplayName
                                                                  , FSlateIcon iIcon )
    : FOdysseyEditorContextMenu( iID, iDisplayName, iIcon )
    , mEditor(iEditor)
{
}
