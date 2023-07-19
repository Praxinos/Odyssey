// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolContextMenu.h"
#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGUI.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorPaintBucketToolContextMenu"

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FOdysseyPainterEditorPaintBucketToolContextMenu interface

//static
TSharedPtr<SWidget>
FOdysseyPainterEditorPaintBucketToolContextMenu::CreateWidget( FOdysseyPainterEditorToolContext* iToolContext )
{
    FOdysseyVectorEngine* vectorEngine = iToolContext->GetVectorEngine();
    FOdysseyVectorScene* vectorScene = vectorEngine->GetScene();
    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
        menu.AddMenuEntry(
            LOCTEXT("DeleteBucket", "Delete Bucket")
            , LOCTEXT("DeleteBucket", "Delete Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteBucket, vectorEngine, vectorScene )));
        menu.AddMenuEntry(
            LOCTEXT("PropagateBucket", "Propagate Bucket")
            , LOCTEXT("PropagateBucket", "Propagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PropagateBucket, vectorEngine, vectorScene )));
        menu.AddMenuEntry(
            LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnpropagateBucket, vectorEngine, vectorScene )));
        menu.AddMenuEntry(
            LOCTEXT("BucketProperties", "Bucket properties")
            , LOCTEXT("BucketProperties", "Bucket Properties")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditorPaintBucketToolContextMenu::BucketProperties, iToolContext )));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

//static
void
FOdysseyPainterEditorPaintBucketToolContextMenu::BucketProperties( FOdysseyPainterEditorToolContext* iToolContext )
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    TSharedPtr<IDetailsView> detailsView;
    FDetailsViewArgs DetailsViewArgs;

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    detailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    detailsView->SetObject(nullptr);

    TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
    .Title(FText::FromString(TEXT("Cookbook Window")))
    .ClientSize(FVector2D(800, 400))
    .SupportsMaximize(false)
    .SupportsMinimize(false)
    [
        detailsView.ToSharedRef()
      /*SNew(SVerticalBox)
      +SVerticalBox::Slot()
      .HAlign(HAlign_Center)
      .VAlign(VAlign_Center)
      [
        SNew(STextBlock)
        .Text(FText::FromString(TEXT("Hello from Slate")))
      ]*/
    ];

    FSlateApplication::Get().AddModalWindow
    (
        CookbookWindow,
        iToolContext->GetEditor()->GetGUI()->GetViewportTab().Get()->Widget(),
        false
    );
}

#undef LOCTEXT_NAMESPACE
