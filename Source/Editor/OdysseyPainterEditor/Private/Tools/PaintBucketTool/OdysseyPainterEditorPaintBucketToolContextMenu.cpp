// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketToolContextMenu.h"
#include "OdysseyPainterEditorVectorBucketView.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorPaintBucketToolContextMenu"

//static
TSharedPtr<SWidget>
FOdysseyPainterEditorPaintBucketToolContextMenu::CreateWidget( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FMenuBuilder menu( true, nullptr );

    menu.BeginSection("Context");
    {
        menu.AddMenuEntry(
            LOCTEXT("DeleteBucket", "Delete Bucket")
            , LOCTEXT("DeleteBucket", "Delete Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::DeleteBucket, iBucket )));
        menu.AddMenuEntry(
            LOCTEXT("PropagateBucket", "Propagate Bucket")
            , LOCTEXT("PropagateBucket", "Propagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::PropagateBucket, iBucket )));
        menu.AddMenuEntry(
            LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , LOCTEXT("UnpropagateBucket", "Unpropagate Bucket")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditor::UnpropagateBucket, iBucket )));
        menu.AddMenuEntry(
            LOCTEXT("BucketProperties", "Bucket properties")
            , LOCTEXT("BucketProperties", "Bucket Properties")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditorPaintBucketToolContextMenu::BucketProperties, iEditor, iBucket )));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

//static
void
FOdysseyPainterEditorPaintBucketToolContextMenu::BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
{
    FPropertyEditorModule& PropertyEditorModule = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    TSharedPtr<IDetailsView> detailsView;
    FDetailsViewArgs DetailsViewArgs;
    UOdysseyPainterEditorVectorBucketView* bucketView = NewObject<UOdysseyPainterEditorVectorBucketView>();

    bucketView->Update( iBucket );

    DetailsViewArgs.bUpdatesFromSelection = false;
    DetailsViewArgs.bLockable = false;
    DetailsViewArgs.bAllowSearch = false;
    DetailsViewArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;

    detailsView = PropertyEditorModule.CreateDetailView(DetailsViewArgs);
    detailsView->SetObject(bucketView);

    TSharedRef<SWindow> CookbookWindow = SNew(SWindow)
    .Title(FText::FromString(TEXT("Bucket Properties")))
    //.ClientSize(FVector2D(800, 400))
    .SizingRule(ESizingRule::Autosized)
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
        iEditor->GetGUI()->GetViewportTab().Get()->Widget(),
        false
    );

    bucketView->ConditionalBeginDestroy();
}

#undef LOCTEXT_NAMESPACE
