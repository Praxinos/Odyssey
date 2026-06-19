// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "SOdysseyPainterEditorToolCollection.h"

#include "SPositiveActionButton.h"
#include "UObject/Package.h"
#include "Widgets/Layout/SExpandableArea.h"

#include "OdysseyPainterEditor.h"
#include "OdysseyToolCollection.h"
#include "OdysseyToolCollectionDragDropOp.h"
#include "SOdysseyPainterEditorToolTile.h"


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
        return iTool->mIconToolConfiguration.MakeIconBrush();
}

FReply
SOdysseyPainterEditorToolCollection::OnAddToolClicked()
{


    if ( !mEditor || !mEditor->GetCurrentMainTool() || !mEditor->GetCurrentMainTool()->IsActivated() )
        return FReply::Unhandled();

    FIconToolConfiguration iconToolConfig;
    iconToolConfig.mIconSource = EToolIconSource::Style;
    iconToolConfig.mIconStyleSet = mEditor->GetCurrentTool()->mIconStyleSet;

    mToolCollection->AddToolConfiguration( mEditor->GetCurrentTool()->GetClass(), mEditor->GetCurrentTool(), iconToolConfig );

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
