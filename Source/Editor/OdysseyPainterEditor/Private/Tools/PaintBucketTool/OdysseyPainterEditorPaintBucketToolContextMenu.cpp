// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorPaintBucketToolContextMenu"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorPaintBucketToolContextMenu
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorPaintBucketToolContextMenu::~FOdysseyPainterEditorPaintBucketToolContextMenu()
{

}

FOdysseyPainterEditorPaintBucketToolContextMenu::FOdysseyPainterEditorPaintBucketToolContextMenu( FOdysseyPainterEditor* iEditor )
    : FOdysseyPainterEditorContextMenu( iEditor
                                      , TEXT("OdysseyPainterEditor_PaintBucketToolContextMenu")
                                      , LOCTEXT( "OdysseyPainterEditorPaintBucketToolContextMenu", "Context Menu" )
                                      , FSlateIcon( "OdysseyStyle", "PainterEditor.Tools16" ) )
{
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorPaintBucketToolContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorPaintBucketToolContextMenu::CreateWidget()
{
    return mMenu.MakeWidget();
}

void
FOdysseyPainterEditorPaintBucketToolContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorContextMenu::BindShortcuts(iToolkit);
/*
    const TSharedRef<FUICommandList>& toolkitCommands = iToolkit->GetToolkitCommands();
    const FOdysseyPainterEditorCommands& painterEditorCommands = FOdysseyPainterEditorCommands::Get();

    #define MAP_ACTION(action, ...) toolkitCommands->MapAction( action, FExecuteAction::CreateSP( this, &FOdysseyPainterEditorPaintBucketToolContextMenu::__VA_ARGS__ ), FCanExecuteAction() );

    #undef MAP_ACTION
*/
}

/*
void
FOdysseyPainterEditorPaintBucketToolContextMenu::OnToolkitInitialized( FBaseToolkit* iToolkit )
{
    FOdysseyEditorContextMenu::OnToolkitInitialized( iToolkit );
}

void
FOdysseyPainterEditorPaintBucketToolContextMenu::ExtendMenu( FToolMenuOwner iOwner, FName iMenuName )
{
    FOdysseyEditorContextMenu::ExtendMenu( iOwner, iMenuName );
}
*/
#undef LOCTEXT_NAMESPACE
