// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketToolContextMenu.h"
#include "OdysseyPainterEditorVectorBucketView.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorVectorPaintBucketToolContextMenu"

//static
TSharedPtr<SWidget>
FOdysseyPainterEditorVectorPaintBucketToolContextMenu::CreateWidget( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
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
            LOCTEXT("CopyBucketParam", "Copy Bucket Param")
            , LOCTEXT("CopyBucketParam", "Copy Bucket Param")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditorVectorPaintBucketToolContextMenu::CopyBucketParam, iBucket )));
        menu.AddMenuEntry(
            LOCTEXT("PasteBucketParam", "Paste Bucket Param")
            , LOCTEXT("PasteBucketParam", "Paste Bucket Param")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditorVectorPaintBucketToolContextMenu::PasteBucketParam, iBucket )));
        menu.AddMenuEntry(
            LOCTEXT("BucketProperties", "Bucket properties")
            , LOCTEXT("BucketProperties", "Bucket Properties")
            , FSlateIcon("OdysseyStyle", "OdysseyLogo.Iliad16")
            , FUIAction(FExecuteAction::CreateStatic(&FOdysseyPainterEditorVectorPaintBucketToolContextMenu::BucketProperties, iEditor, iBucket )));
    }
    menu.EndSection();

    return menu.MakeWidget();
}

//static
void
FOdysseyPainterEditorVectorPaintBucketToolContextMenu::BucketProperties( FOdysseyPainterEditor* iEditor, FOdysseyVectorBucket* iBucket )
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

    TSharedPtr<FOdysseyPainterEditorViewportTab> viewportTab = iEditor->FindTab<FOdysseyPainterEditorViewportTab>();

    FSlateApplication::Get().AddModalWindow
    (
        CookbookWindow,
        viewportTab->Widget(),
        false
    );

    bucketView->ConditionalBeginDestroy();
}

static FOdysseyVectorBucket&
GetCopiedBucket()
{
    static FOdysseyVectorBucket copiedBucket( nullptr, 0, 0, false );

    return copiedBucket;
}

// static
void
FOdysseyPainterEditorVectorPaintBucketToolContextMenu::CopyBucketParam( FOdysseyVectorBucket* iSourceBucket )
{
    FOdysseyVectorBucket& destinationBucket = GetCopiedBucket();

    iSourceBucket->Copy( &destinationBucket );
}

// static
void
FOdysseyPainterEditorVectorPaintBucketToolContextMenu::PasteBucketParam( FOdysseyVectorBucket* iDestinationBucket )
{
    FOdysseyVectorBucket& sourceBucket = GetCopiedBucket();
    ::ULIS::FVec2D destinationBucketCoords = iDestinationBucket->GetCoords();

    sourceBucket.Copy( iDestinationBucket );

    // we only keep the coords
    iDestinationBucket->SetCoords( destinationBucketCoords.x, destinationBucketCoords.y, 0.0f );

    iDestinationBucket->Invalidate();
}


#undef LOCTEXT_NAMESPACE
