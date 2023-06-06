// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyPainterEditorVectorContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorContextMenu"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorVectorContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorVectorContextMenu::~FOdysseyPainterEditorVectorContextMenu()
{

}

FOdysseyPainterEditorVectorContextMenu::FOdysseyPainterEditorVectorContextMenu(FOdysseyPainterEditor* iEditor)
    : FOdysseyEditorContextMenu(TEXT("OdysseyPainterEditor_VectorContextMenu"),
                               LOCTEXT( "OdysseyPainterEditorVectorContextMenu", "Context Menu" ),
                               FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ))
    , mEditor(iEditor)
{

}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorContextMenu::CreateWidget()
{
    return mMenu.MakeWidget();
}

void
FOdysseyPainterEditorVectorContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorVectorContextMenu::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

#undef LOCTEXT_NAMESPACE
