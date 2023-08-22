// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolVertexContextMenu.h"
#include "OdysseyPainterEditor.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPickToolVertexContextMenu"

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorVectorPickToolVertexContextMenu interface

TSharedPtr<SWidget>
FOdysseyPainterEditorVectorPickToolVertexContextMenu::CreateWidget( FOdysseyPainterEditor* iEditor )
{
    bool hasVector = iEditor->GetCurrentMediaProvider().HasMedia<FOdysseyMediaVector>();
    if (!hasVector)
        return nullptr;

    TArray<TSharedPtr<FOdysseyMediaVector>> mediaVectors = iEditor->GetCurrentMediaProvider().GetMedias<FOdysseyMediaVector>();
    if (mediaVectors.Num() <= 0)
        return nullptr;

    FOdysseyVectorScene* vectorScene = mediaVectors[0]->GetScene();
    FOdysseyVectorEngine* vectorEngine = vectorScene->GetEngine();

    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
    menu.AddMenuEntry(
          LOCTEXT("DeleteSelection", "Delete Selection")
        , LOCTEXT("DeleteSelection", "Delete Selection")
        , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
        , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeletePointSelection, vectorEngine, vectorScene)));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
