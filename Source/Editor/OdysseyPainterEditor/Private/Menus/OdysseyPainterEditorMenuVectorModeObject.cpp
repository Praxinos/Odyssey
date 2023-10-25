// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Menus/OdysseyPainterEditorMenuVectorModeObject.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyMediaVector.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorMenuVectorModeObject"

// static
TSharedPtr<SWidget>
FOdysseyPainterEditorMenuVectorModeObject::CreateWidget( FOdysseyPainterEditor* iEditor )
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
                      LOCTEXT("ResetView", "Reset View")
                    , LOCTEXT("ResetView", "Reset View")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ResetView, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("GroupPaint", "Make Paint Group")
                    , LOCTEXT("GroupPaint", "Make Paint Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::GroupPaint, vectorEngine, vectorScene)));
                /*menu.AddMenuEntry(
                      LOCTEXT("Trim", "Trim")
                    , LOCTEXT("Trim", "Trim")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Trim, vectorScene)));*/
                menu.AddMenuEntry(
                      LOCTEXT("Group", "Group")
                    , LOCTEXT("Group", "Group")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Group, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("Ungroup", "Ungroup")
                    , LOCTEXT("Ungroup", "Ungroup")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::Ungroup, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("BringForward", "Bring forward")
                    , LOCTEXT("BringForward", "Bring forward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::BringForward, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("SendBackward", "Send backward")
                    , LOCTEXT("SendBackward", "Send backward")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::SendBackward, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("DeleteSelection","Delete Selection")
                    , LOCTEXT("DeleteSelection","Delete Selection")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteObjectSelection, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("FlipHorizontal","Flip Horizontal")
                    , LOCTEXT("FlipHorizontal","Flip Horizontal")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::FlipHorizontal, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                      LOCTEXT("FlipVertical","Flip Vertical")
                    , LOCTEXT("FlipVertical","Flip Vertical")
                    , FSlateIcon("OdysseyStyle","OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::FlipVertical, vectorEngine, vectorScene)));
                menu.AddMenuEntry(
                    LOCTEXT("ClearColoring", "Clear Coloring")
                    , LOCTEXT("ClearColoring", "Clear Coloring")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ClearColoring, vectorEngine, vectorScene )));
                menu.AddMenuEntry(
                    LOCTEXT("ApplyTransformations", "Apply Transformations")
                    , LOCTEXT("ApplyTransformations", "Apply Transformations")
                    , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
                    , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::ApplyTransformations, vectorScene )));
            }
            menu.EndSection();
        }
    }

    return menu.MakeWidget();
}

#undef LOCTEXT_NAMESPACE
