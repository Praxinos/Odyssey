// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolContextMenu.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGUI.h"

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
    /*
    FOdysseyVectorEngine* vectorEngine = iToolContext->GetVectorEngine();
    FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();

    FMenuBuilder menu(true, nullptr);
    menu.BeginSection("Context");
    {
        menu.AddMenuEntry(
            LOCTEXT("DeleteBucket", "Delete Bucket")
            , LOCTEXT("DeleteBucket", "Delete Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateSP(GetEditor()->GetGUI(), &FOdysseyPainterEditor::DeleteBucket, vectorEngine, vectorScene)));
        menu.AddMenuEntry(
            LOCTEXT("PropagateBucket", "Propagate Bucket")
            , LOCTEXT("PropagateBucket", "Propagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateSP(GetEditor()->GetGUI(), &FOdysseyPainterEditor::PropagateBucket, vectorEngine, vectorScene)));
        menu.AddMenuEntry(
            LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateSP(GetEditor()->GetGUI(), &FOdysseyPainterEditor::UnpropagateBucket, vectorEngine, vectorScene)));
    }
    menu.EndSection();
    return menu.MakeWidget();
    */

    return SNullWidget::NullWidget;
}

void
FOdysseyPainterEditorPaintBucketToolContextMenu::BindShortcuts(FBaseToolkit* iToolkit)
{
    FOdysseyEditorContextMenu::BindShortcuts(iToolkit);
}
#undef LOCTEXT_NAMESPACE
