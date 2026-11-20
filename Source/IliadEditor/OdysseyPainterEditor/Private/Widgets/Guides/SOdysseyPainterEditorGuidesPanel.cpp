// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "SOdysseyPainterEditorGuidesPanel.h"

#include "OdysseyPainterEditor.h"
#include "Shapes/OdysseyPainterEditorEllipseGuide.h"
#include "Shapes/OdysseyPainterEditorLineGuide.h"
#include "Tools/GuideEditTool/OdysseyPainterEditorGuideEditTool.h"
#include "OdysseyAnimation.h"
#include "Sources/OdysseyPainterEditorTextureSource.h"
#include "Engine/Texture2D.h"
#include "Framework/MultiBox/MultiBoxBuilder.h"
#include "Input/DragAndDrop.h"
#include "IDetailsView.h"
#include "PropertyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "OdysseyStyle.h"
#include "ScopedTransaction.h"
#include "Widgets/Input/SButton.h"
#include "Widgets/Input/SEditableTextBox.h"
#include "Widgets/Input/SComboButton.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SBox.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Layout/SSeparator.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Views/SListView.h"
#include "Widgets/Views/STableRow.h"

#define LOCTEXT_NAMESPACE "PainterEditorGuides"

FGuideReorderDragDropOp::FGuideReorderDragDropOp(TWeakObjectPtr<UOdysseyPainterEditorGuide> InGuide)
    : Guide(InGuide)
{
}

void SOdysseyPainterEditorGuidesPanel::Construct(const FArguments& Args)
{
    mEditor = Args._Editor;
    FPropertyEditorModule& PropertyEditor = FModuleManager::GetModuleChecked<FPropertyEditorModule>("PropertyEditor");
    FDetailsViewArgs DetailsArgs;
    DetailsArgs.bUpdatesFromSelection = false;
    DetailsArgs.bLockable = false;
    DetailsArgs.bAllowSearch = false;
    DetailsArgs.NameAreaSettings = FDetailsViewArgs::HideNameArea;
    mDetailsView = PropertyEditor.CreateDetailView(DetailsArgs);

    SAssignNew(mListView, SListView<TWeakObjectPtr<UOdysseyPainterEditorGuide>>)
        .ListItemsSource(&mGuideItems)
        .SelectionMode(ESelectionMode::Single)
        .OnGenerateRow(this, &SOdysseyPainterEditorGuidesPanel::MakeGuideRow)
        .OnSelectionChanged(this, &SOdysseyPainterEditorGuidesPanel::SelectGuide);

    ChildSlot
    [
        SNew(SVerticalBox)
        + SVerticalBox::Slot().AutoHeight().Padding(6.f)
        [
            SNew(SHorizontalBox)
            + SHorizontalBox::Slot().AutoWidth()
            [
                SNew(SComboButton)
                .ButtonContent()[ SNew(STextBlock).Text(LOCTEXT("add-guide", "Add Guide  +")) ]
                .MenuContent()
                [
                    MakeAddMenu()
                ]
            ]
            + SHorizontalBox::Slot().FillWidth(1.f).HAlign(HAlign_Right)
            [
                SNew(SButton)
                .IsEnabled_Lambda([this]()
                {
                    UOdysseyPainterEditorGuide* SelectedGuide = mSelectedGuide.Get();
                    return mEditor && SelectedGuide && SelectedGuide->bVisible && !SelectedGuide->bLocked;
                })
                .OnClicked(this, &SOdysseyPainterEditorGuidesPanel::ToggleViewportEditing)
                [
                    SNew(STextBlock).Text(this, &SOdysseyPainterEditorGuidesPanel::GetEditButtonText)
                ]
            ]
        ]
        + SVerticalBox::Slot().FillHeight(1.f)
        [
            SNew(SSplitter)
            .Orientation(Orient_Vertical)
            + SSplitter::Slot().Value(0.58f)
            [
                SNew(SBorder).Padding(2.f)
                [
                    mListView.ToSharedRef()
                ]
            ]
            + SSplitter::Slot().Value(0.42f)
            [
                SNew(SBorder).Padding(4.f)
                [
                    mDetailsView.ToSharedRef()
                ]
            ]
        ]
    ];

    Refresh();
}

UOdysseyPainterEditorGuideCollection* SOdysseyPainterEditorGuidesPanel::GetCollection(bool bCreateIfMissing)
{
    FOdysseyPainterEditor* Editor = mEditor;
    if (!Editor)
        return nullptr;

    if (UOdysseyAnimation* Animation = Editor->GetAnimation())
    {
        if (!Animation->GetGuideCollection() && bCreateIfMissing)
        {
            Animation->Modify();
            Animation->SetGuideCollection(NewObject<UOdysseyPainterEditorGuideCollection>(Animation, NAME_None, RF_Transactional));
            Animation->MarkPackageDirty();
        }
        return Animation->GetGuideCollection();
    }

    TSharedPtr<FOdysseyPainterEditorTextureSource> TextureSource = Editor->GetSourceTyped<FOdysseyPainterEditorTextureSource>();
    UTexture2D* Texture = TextureSource.IsValid() ? TextureSource->GetTexture() : nullptr;
    if (!Texture)
        return nullptr;

    UOdysseyPainterEditorGuideCollection* Collection = Cast<UOdysseyPainterEditorGuideCollection>(Texture->GetAssetUserDataOfClass(UOdysseyPainterEditorGuideCollection::StaticClass()));
    if (!Collection && bCreateIfMissing)
    {
        Collection = NewObject<UOdysseyPainterEditorGuideCollection>(Texture, NAME_None, RF_Public | RF_Transactional);
        Texture->Modify();
        Texture->AddAssetUserData(Collection);
        Texture->PostEditChange();
    }
    return Collection;
}

TSharedRef<SWidget> SOdysseyPainterEditorGuidesPanel::MakeAddMenu()
{
    FMenuBuilder MenuBuilder(true, nullptr);
    MenuBuilder.AddMenuEntry(LOCTEXT("add-line-guide", "Line"), FText(), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SOdysseyPainterEditorGuidesPanel::AddGuide, UOdysseyPainterEditorLineGuide::StaticClass())));
    MenuBuilder.AddMenuEntry(LOCTEXT("add-ellipse-guide", "Ellipse"), FText(), FSlateIcon(), FUIAction(FExecuteAction::CreateSP(this, &SOdysseyPainterEditorGuidesPanel::AddGuide, UOdysseyPainterEditorEllipseGuide::StaticClass())));
    return MenuBuilder.MakeWidget();
}

void SOdysseyPainterEditorGuidesPanel::AddGuide(UClass* GuideClass)
{
    FScopedTransaction Transaction(LOCTEXT("add-guide-transaction", "Add Guide"));
    UOdysseyPainterEditorGuideCollection* Collection = GetCollection(true);
    if (!Collection || !GuideClass || !GuideClass->IsChildOf(UOdysseyPainterEditorGuide::StaticClass()))
        return;

    Collection->Modify();
    UOdysseyPainterEditorGuide* Guide = NewObject<UOdysseyPainterEditorGuide>(Collection, GuideClass, NAME_None, RF_Transactional);
    Collection->Guides.Add(Guide);
    Collection->MarkPackageDirty();
    Refresh();
    if (mListView.IsValid())
        mListView->SetSelection(TWeakObjectPtr<UOdysseyPainterEditorGuide>(Guide));
}

TSharedRef<ITableRow> SOdysseyPainterEditorGuidesPanel::MakeGuideRow(TWeakObjectPtr<UOdysseyPainterEditorGuide> GuideReference, const TSharedRef<STableViewBase>& OwnerTable)
{
    UOdysseyPainterEditorGuide* Guide = GuideReference.Get();
    check(Guide);
    return SNew(STableRow<TWeakObjectPtr<UOdysseyPainterEditorGuide>>, OwnerTable)
    .OnCanAcceptDrop(this, &SOdysseyPainterEditorGuidesPanel::CanDropGuide)
    .OnAcceptDrop(this, &SOdysseyPainterEditorGuidesPanel::DropGuide)
    .OnDragDetected(this, &SOdysseyPainterEditorGuidesPanel::BeginGuideDrag, GuideReference)
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot().AutoWidth().Padding(2.f)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("SimpleButton.NoPadding"))
            .ToolTipText(Guide->bVisible ? LOCTEXT("hide-guide-tooltip", "Hide guide") : LOCTEXT("show-guide-tooltip", "Show guide"))
            .OnClicked(this, &SOdysseyPainterEditorGuidesPanel::ToggleVisibility, Guide)
            [
                SNew(SImage)
                .Image(this, &SOdysseyPainterEditorGuidesPanel::GetVisibilityBrush, Guide)
                .ColorAndOpacity(FSlateColor::UseForeground())
            ]
        ]
        + SHorizontalBox::Slot().AutoWidth().Padding(2.f)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("SimpleButton.NoPadding"))
            .ToolTipText(Guide->bLocked ? LOCTEXT("unlock-guide-tooltip", "Unlock guide") : LOCTEXT("lock-guide-tooltip", "Lock guide"))
            .OnClicked(this, &SOdysseyPainterEditorGuidesPanel::ToggleLock, Guide)
            [
                SNew(SImage)
                .Image(this, &SOdysseyPainterEditorGuidesPanel::GetLockBrush, Guide)
                .ColorAndOpacity(FSlateColor::UseForeground())
            ]
        ]
        + SHorizontalBox::Slot().AutoWidth().Padding(4.f)
        [
            SNew(SImage)
            .Image(this, &SOdysseyPainterEditorGuidesPanel::GetTypeBrush, Guide)
            .ColorAndOpacity(FSlateColor::UseForeground())
        ]
        + SHorizontalBox::Slot().FillWidth(1.f).Padding(2.f)
        [
            SNew(SEditableTextBox)
            .Text(this, &SOdysseyPainterEditorGuidesPanel::GetGuideName, Guide)
            .OnTextCommitted(this, &SOdysseyPainterEditorGuidesPanel::RenameGuide, Guide)
        ]
        + SHorizontalBox::Slot().AutoWidth().Padding(2.f)
        [
            SNew(SButton)
            .ButtonStyle(&FOdysseyStyle::GetWidgetStyle<FButtonStyle>("SimpleButton.NoPadding"))
            .ToolTipText(LOCTEXT("delete-guide-tooltip", "Delete guide"))
            .OnClicked(this, &SOdysseyPainterEditorGuidesPanel::DeleteGuide, Guide)
            [
                SNew(SImage)
                .Image(FOdysseyStyle::GetBrush("PainterEditor.Clear16"))
                .ColorAndOpacity(FSlateColor::UseForeground())
            ]
        ]
    ];
}

TOptional<EItemDropZone> SOdysseyPainterEditorGuidesPanel::CanDropGuide(const FDragDropEvent& Event, EItemDropZone Zone, TWeakObjectPtr<UOdysseyPainterEditorGuide>)
{
    const TSharedPtr<FGuideReorderDragDropOp> Operation = Event.GetOperationAs<FGuideReorderDragDropOp>();
    if (Operation.IsValid() && Operation->Guide.IsValid() && Zone != EItemDropZone::OntoItem)
        return Zone;
    return TOptional<EItemDropZone>();
}

FReply SOdysseyPainterEditorGuidesPanel::DropGuide(const FDragDropEvent& Event, EItemDropZone Zone, TWeakObjectPtr<UOdysseyPainterEditorGuide> TargetGuideReference)
{
    const TSharedPtr<FGuideReorderDragDropOp> Operation = Event.GetOperationAs<FGuideReorderDragDropOp>();
    UOdysseyPainterEditorGuideCollection* Collection = GetCollection(false);
    UOdysseyPainterEditorGuide* SourceGuide = Operation.IsValid() ? Operation->Guide.Get() : nullptr;
    UOdysseyPainterEditorGuide* TargetGuide = TargetGuideReference.Get();
    if (!SourceGuide || !TargetGuide || !Collection || SourceGuide == TargetGuide)
        return FReply::Unhandled();

    const int32 SourceIndex = Collection->Guides.IndexOfByKey(SourceGuide);
    int32 TargetIndex = Collection->Guides.IndexOfByKey(TargetGuide);
    if (SourceIndex == INDEX_NONE || TargetIndex == INDEX_NONE)
        return FReply::Unhandled();
    if (Zone == EItemDropZone::BelowItem)
        ++TargetIndex;
    FScopedTransaction Transaction(LOCTEXT("reorder-guides-transaction", "Reorder Guides"));
    Collection->Modify();
    Collection->Guides.RemoveAt(SourceIndex);
    if (SourceIndex < TargetIndex)
        --TargetIndex;
    Collection->Guides.Insert(SourceGuide, FMath::Clamp(TargetIndex, 0, Collection->Guides.Num()));
    Collection->MarkPackageDirty();
    Refresh();
    return FReply::Handled();
}

FReply SOdysseyPainterEditorGuidesPanel::BeginGuideDrag(const FGeometry&, const FPointerEvent& Event, TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide)
{
    if (!Event.IsMouseButtonDown(EKeys::LeftMouseButton) || !Guide.IsValid())
        return FReply::Unhandled();
    TSharedRef<FGuideReorderDragDropOp> Operation = MakeShared<FGuideReorderDragDropOp>(Guide);
    Operation->Construct();
    return FReply::Handled().BeginDragDrop(Operation);
}

void SOdysseyPainterEditorGuidesPanel::SelectGuide(TWeakObjectPtr<UOdysseyPainterEditorGuide> Guide, ESelectInfo::Type)
{
    mSelectedGuide = Guide;
    if (mDetailsView.IsValid())
        mDetailsView->SetObject(Guide.Get());
    FOdysseyPainterEditor* Editor = mEditor;
    if (Editor && Editor->GetCurrentTemporaryTool() == Editor->GetGuideEditTool())
        Editor->GetGuideEditTool()->SetGuide(Guide.Get());
}

FReply SOdysseyPainterEditorGuidesPanel::ToggleVisibility(UOdysseyPainterEditorGuide* Guide)
{
    FOdysseyPainterEditor* Editor = mEditor;
    FScopedTransaction Transaction(LOCTEXT("toggle-guide-visibility-transaction", "Toggle Guide Visibility"));
    Guide->Modify();
    Guide->bVisible = !Guide->bVisible;
    Guide->MarkPackageDirty();
    Refresh();
    if (!Guide->bVisible && Editor && Editor->GetCurrentTemporaryTool() == Editor->GetGuideEditTool())
        Editor->InactivateTemporaryTool();
    return FReply::Handled();
}

FReply SOdysseyPainterEditorGuidesPanel::ToggleLock(UOdysseyPainterEditorGuide* Guide)
{
    FOdysseyPainterEditor* Editor = mEditor;
    FScopedTransaction Transaction(LOCTEXT("toggle-guide-lock-transaction", "Toggle Guide Lock"));
    Guide->Modify();
    Guide->bLocked = !Guide->bLocked;
    Guide->MarkPackageDirty();
    Refresh();
    if (Guide->bLocked && Editor && Editor->GetCurrentTemporaryTool() == Editor->GetGuideEditTool())
        Editor->InactivateTemporaryTool();
    return FReply::Handled();
}

FReply SOdysseyPainterEditorGuidesPanel::DeleteGuide(UOdysseyPainterEditorGuide* Guide)
{
    FOdysseyPainterEditor* Editor = mEditor;
    if (UOdysseyPainterEditorGuideCollection* Collection = GetCollection(false))
    {
        FScopedTransaction Transaction(LOCTEXT("delete-guide-transaction", "Delete Guide"));
        if (Editor && Editor->GetCurrentTemporaryTool() == Editor->GetGuideEditTool() && Editor->GetGuideEditTool()->GetGuide() == Guide)
            Editor->InactivateTemporaryTool();
        Collection->Modify();
        Collection->Guides.Remove(Guide);
        Collection->MarkPackageDirty();
    }
    if (mSelectedGuide.Get() == Guide)
    {
        mSelectedGuide = nullptr;
        if (mDetailsView.IsValid())
            mDetailsView->SetObject(nullptr);
    }
    Refresh();
    return FReply::Handled();
}

FReply SOdysseyPainterEditorGuidesPanel::ToggleViewportEditing()
{
    FOdysseyPainterEditor* Editor = mEditor;
    UOdysseyPainterEditorGuide* SelectedGuide = mSelectedGuide.Get();
    if (!Editor || !SelectedGuide)
        return FReply::Unhandled();
    UOdysseyPainterEditorGuideEditTool* Tool = Editor->GetGuideEditTool();
    if (!Tool)
        return FReply::Unhandled();
    if (Editor->GetCurrentTemporaryTool() == Tool)
    {
        Editor->InactivateTemporaryTool();
    }
    else
    {
        Tool->SetGuide(SelectedGuide);
        Editor->ActivateTemporaryTool(Tool);
    }
    return FReply::Handled();
}

FText SOdysseyPainterEditorGuidesPanel::GetGuideName(UOdysseyPainterEditorGuide* Guide) const
{
    return Guide ? Guide->Name : FText::GetEmpty();
}

const FSlateBrush* SOdysseyPainterEditorGuidesPanel::GetVisibilityBrush(UOdysseyPainterEditorGuide* Guide) const
{
    return FOdysseyStyle::GetBrush(Guide && Guide->bVisible ? "OdysseyLayerStack.Visible16" : "OdysseyLayerStack.NotVisible16");
}

const FSlateBrush* SOdysseyPainterEditorGuidesPanel::GetLockBrush(UOdysseyPainterEditorGuide* Guide) const
{
    return FOdysseyStyle::GetBrush(Guide && Guide->bLocked ? "OdysseyLayerStack.Locked16" : "OdysseyLayerStack.Unlocked16");
}

const FSlateBrush* SOdysseyPainterEditorGuidesPanel::GetTypeBrush(UOdysseyPainterEditorGuide* Guide) const
{
    if (!Guide)
        return FOdysseyStyle::GetNoBrush();

    if( Guide->GetGuideType() == EOdysseyPainterEditorGuideType::Line )
        return FOdysseyStyle::GetBrush("Shapes.Line");
    else if ( Guide->GetGuideType() == EOdysseyPainterEditorGuideType::Ellipse )
        return FOdysseyStyle::GetBrush("Shapes.Ellipse_Filled");
    else
        return FOdysseyStyle::GetNoBrush();
}

void SOdysseyPainterEditorGuidesPanel::RenameGuide(const FText& NewName, ETextCommit::Type, UOdysseyPainterEditorGuide* Guide)
{
    if (Guide && Guide->Name.ToString() != NewName.ToString())
    {
        FScopedTransaction Transaction(LOCTEXT("rename-guide-transaction", "Rename Guide"));
        Guide->Modify(); Guide->Name = NewName; Guide->MarkPackageDirty();
        if (mListView.IsValid()) mListView->RequestListRefresh();
    }
}

FText SOdysseyPainterEditorGuidesPanel::GetEditButtonText() const
{
    FOdysseyPainterEditor* Editor = mEditor;
    return Editor && Editor->GetCurrentTemporaryTool() == Editor->GetGuideEditTool()
        ? LOCTEXT("stop-editing-in-viewport", "Stop editing")
        : LOCTEXT("edit-in-viewport", "Edit in viewport");
}

void SOdysseyPainterEditorGuidesPanel::Refresh()
{
    UOdysseyPainterEditorGuideCollection* Collection = GetCollection(false);
    mCollection = Collection;
    mGuideItems.Reset();
    if (Collection)
    {
        for (UOdysseyPainterEditorGuide* Guide : Collection->Guides)
        {
            if (Guide)
            {
                mGuideItems.Add(Guide);
            }
        }
    }
    if (mListView.IsValid())
        mListView->RequestListRefresh();
}

#undef LOCTEXT_NAMESPACE
