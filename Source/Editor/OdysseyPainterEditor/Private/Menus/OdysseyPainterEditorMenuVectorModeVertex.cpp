// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Menus/OdysseyPainterEditorMenuVectorModeVertex.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorMenuVectorModeVertex"

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorSelectionToolVertexContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorMenuVectorModeVertex::CreateWidget( FOdysseyPainterEditor* iEditor )
{
    FMenuBuilder menu( true, nullptr );
    bool hasVector = iEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();

    if( hasVector )
    {
        TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = iEditor->GetCurrentMediaProvider().GetOrCreateMedias<FOdysseyMediaVector>();

        if( mediaVectors.Num() && ( mediaVectors[0]->IsLocked() == false ) )
        {
            FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
            FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

            menu.BeginSection("Context");
            {
            menu.AddMenuEntry(
                  LOCTEXT("DeleteSelection", "Delete Selection")
                , LOCTEXT("DeleteSelection", "Delete Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeletePointSelection, vectorEngine, vectorScene)));
            menu.AddMenuEntry(
                  LOCTEXT("AlignPointSelection", "Align Point Selection")
                , LOCTEXT("AlignPointSelection", "Align Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::AlignPointSelection, vectorScene)));
            menu.AddMenuEntry(
                  LOCTEXT("UnalignPointSelection", "Unalign Point Selection")
                , LOCTEXT("UnalignPointSelection", "Unalign Point Selection")
                , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnalignPointSelection, vectorScene)));
            }
            menu.EndSection();
        }
    }

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
