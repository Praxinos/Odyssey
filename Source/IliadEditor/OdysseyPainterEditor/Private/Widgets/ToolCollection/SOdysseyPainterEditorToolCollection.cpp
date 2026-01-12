// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolCollection.h"

#include "OdysseyPainterEditor.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"
#include "SOdysseyPainterEditorToolTile.h"
#include "SPositiveActionButton.h"
#include "Misc/PackageName.h"
#include "UObject/Package.h"
#include "UObject/SavePackage.h"


SOdysseyPainterEditorToolCollection::~SOdysseyPainterEditorToolCollection()
{
    if (mToolCollection)
    {
        mToolCollection->OnCollectionChanged.RemoveAll(this);
    }
}

SOdysseyPainterEditorToolCollection::SOdysseyPainterEditorToolCollection()
{}

void SOdysseyPainterEditorToolCollection::Construct(const FArguments& InArgs)
{
    mEditor = InArgs._Editor;
    mToolCollection = InArgs._ToolCollection;
    bIsUnlocked = InArgs._IsUnlocked;

    if (!mEditor || !mToolCollection)
        return;

    mToolCollection->OnCollectionChanged.AddSP(this, &SOdysseyPainterEditorToolCollection::HandleToolsChanged);

    ChildSlot
        [
            SNew(SExpandableArea)
                .InitiallyCollapsed(false)
                .HeaderContent()
                [
                    SNew(SHorizontalBox)
                        // Collection name
                        + SHorizontalBox::Slot()
                        .FillWidth(1.f)
                        .VAlign(VAlign_Center)
                        [
                            SNew(STextBlock)
                                .Text(this, &SOdysseyPainterEditorToolCollection::GetCollectionDisplayName)
                        ]
                        // Dropdown menu
                        + SHorizontalBox::Slot()
                        .AutoWidth()
                        .VAlign(VAlign_Center)
                        .Padding(4.f, 0.f)
                        [
                            mToolCollection->IsCollectionTransient()
                                ? SNullWidget::NullWidget
                                : SNew(SComboButton)
                                .HasDownArrow(true)
                                .OnGetMenuContent(this, &SOdysseyPainterEditorToolCollection::GetCollectionMenuContent)
                        ]
                ]
                .BodyContent()
                [
                    SAssignNew(mToolWrapBox, SWrapBox)
                        .UseAllottedSize(true)
                        .InnerSlotPadding(FVector2D(2.f, 2.f))
                ]
        ];

    HandleToolsChanged();
}


TSharedRef<SWidget> SOdysseyPainterEditorToolCollection::GenerateToolConfigTile(UOdysseyPainterEditorToolConfiguration* iTool)
{
    return SNew(SOdysseyPainterEditorToolTile)
        .ToolConfig(iTool)
        .ToolCollection(mToolCollection)
        .Editor(mEditor)
        .IsUnlocked(bIsUnlocked);
}

FText SOdysseyPainterEditorToolCollection::GetCollectionDisplayName() const
{
    if (!mToolCollection || mToolCollection->IsCollectionTransient())
    {
        return FText::FromString(TEXT("Recent Tools"));
    }

    const bool bIsDirty = mToolCollection->GetOutermost() && mToolCollection->GetOutermost()->IsDirty();

    const FString DisplayName = bIsDirty ? FString::Printf(TEXT("%s*"), *mToolCollection->GetName()) : mToolCollection->GetName();

    return FText::FromString(DisplayName);
}

const FSlateBrush*
SOdysseyPainterEditorToolCollection::GetToolConfigIcon(UOdysseyPainterEditorToolConfiguration* iTool) const
{
    if( !iTool)
        return FAppStyle::GetBrush("ClassIcon.Default");
    else
        return &iTool->mIcon;
}

FReply
SOdysseyPainterEditorToolCollection::OnAddToolClicked()
{
    if ( !mEditor || !mEditor->GetCurrentTool() || !mEditor->GetEditorToolOfClass(mEditor->GetCurrentTool()->GetClass()) )
        return FReply::Unhandled();

    TObjectPtr<UOdysseyPainterEditorTool> toolPropertySnapshot;
    toolPropertySnapshot = DuplicateObject< UOdysseyPainterEditorTool >(mEditor->GetCurrentTool(), mToolCollection);

    mToolCollection->AddToolConfiguration( mEditor->GetCurrentTool()->GetClass(), toolPropertySnapshot, mEditor->GetCurrentTool()->Icon );

    HandleToolsChanged();

    return FReply::Handled();
}

void
SOdysseyPainterEditorToolCollection::HandleToolsChanged()
{
    mDisplayedTools = mToolCollection->GetToolConfigurations();

    RefreshToolsGUI();
}

void SOdysseyPainterEditorToolCollection::RefreshToolsGUI()
{
    if (!mToolWrapBox.IsValid())
        return;

    mToolWrapBox->ClearChildren();

    // Rebuild tool tiles
    for (UOdysseyPainterEditorToolConfiguration* toolConfig : mDisplayedTools)
    {
        mToolWrapBox->AddSlot()
            [
                GenerateToolConfigTile(toolConfig)
            ];
    }

    if( !mToolCollection->IsCollectionTransient() )
    {
        mToolWrapBox->AddSlot()
            [
                SNew(SPositiveActionButton)
                    .OnClicked(this, &SOdysseyPainterEditorToolCollection::OnAddToolClicked)
                    .ToolTipText(FText::FromString("Add the current tool to the collection"))
                    .Visibility(this, &SOdysseyPainterEditorToolCollection::GetAddButtonVisibility)
            ];
    }
}

EVisibility SOdysseyPainterEditorToolCollection::GetAddButtonVisibility() const
{
    return bIsUnlocked.Get() ? EVisibility::Visible : EVisibility::Collapsed;
}

TSharedRef<SWidget> SOdysseyPainterEditorToolCollection::GetCollectionMenuContent()
{
    FMenuBuilder menuBuilder(true, nullptr);

    menuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Save Collection")),
        FText::FromString(TEXT("Save this tool collection")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolCollection::HandleSaveCollection)
        )
    );

    /*MenuBuilder.AddMenuEntry(
        FText::FromString(TEXT("Remove Collection")),
        FText::FromString(TEXT("Remove this tool collection from the editor")),
        FSlateIcon(),
        FUIAction(
            FExecuteAction::CreateSP(this, &SOdysseyPainterEditorToolCollection::HandleRemoveCollection)
        )
    );*/

    return menuBuilder.MakeWidget();
}

void SOdysseyPainterEditorToolCollection::HandleSaveCollection()
{
    if (!mToolCollection || mToolCollection->IsCollectionTransient())
    {
        return;
    }

    UPackage* package = mToolCollection->GetOutermost();
    if (!package)
    {
        return;
    }

    if (!package->IsDirty())
    {
        return;
    }

    const FString packageFileName = FPackageName::LongPackageNameToFilename( package->GetName(), FPackageName::GetAssetPackageExtension() );

    FSavePackageArgs saveArgs;
    saveArgs.TopLevelFlags = RF_Public | RF_Standalone;
    saveArgs.SaveFlags = SAVE_None;
    saveArgs.Error = GError;

    UPackage::SavePackage(
        package,
        nullptr,
        *packageFileName,
        saveArgs
    );
}
