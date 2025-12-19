// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolCollection.h"

#include "OdysseyPainterEditor.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"
#include "SOdysseyPainterEditorToolTile.h"
#include "SPositiveActionButton.h"

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
                .AreaTitle(this, &SOdysseyPainterEditorToolCollection::GetCollectionDisplayName)
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

FText
SOdysseyPainterEditorToolCollection::GetCollectionDisplayName() const
{
    return mToolCollection && !mToolCollection->IsCollectionTransient() ? FText::FromString(mToolCollection->GetName())
                                 : FText::FromString(TEXT("Recent Tools"));
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

    FToolPropertySnapshot toolPropertySnapshot;
    mEditor->SaveToolPropertySnapshot( mEditor->GetCurrentTool(), toolPropertySnapshot );

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
