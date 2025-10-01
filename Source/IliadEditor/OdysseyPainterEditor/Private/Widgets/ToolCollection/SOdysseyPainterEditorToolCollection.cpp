// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Widgets/ToolCollection/SOdysseyPainterEditorToolCollection.h"

#include "OdysseyPainterEditor.h"
#include "ToolCollection/OdysseyToolCollection.h"
#include "Widgets/Layout/SExpandableArea.h"
#include "ToolCollection/OdysseyToolCollectionDragDropOp.h"
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


TSharedRef<SWidget> SOdysseyPainterEditorToolCollection::GenerateToolTile(UOdysseyPainterEditorTool* Tool)
{
    return SNew(SOdysseyPainterEditorToolTile)
        .Tool(Tool)
        .ToolCollection(mToolCollection)
        .Editor(mEditor);
}

FText
SOdysseyPainterEditorToolCollection::GetCollectionDisplayName() const
{
    return mToolCollection && !mToolCollection->IsCollectionTransient() ? FText::FromString(mToolCollection->GetName())
                                 : FText::FromString(TEXT("Recent Tools"));
}

const FSlateBrush*
SOdysseyPainterEditorToolCollection::GetToolIcon(UOdysseyPainterEditorTool* iTool) const
{
    if( !iTool )
        return FAppStyle::GetBrush("ClassIcon.Default");
    else
        return &iTool->Icon;
}

FReply
SOdysseyPainterEditorToolCollection::OnAddToolClicked()
{
    mToolCollection->AddTool( mEditor->GetCurrentTool() );
    HandleToolsChanged();

    return FReply::Handled();
}

void
SOdysseyPainterEditorToolCollection::HandleToolsChanged()
{
    mDisplayedTools = mToolCollection->GetTools();

    RefreshToolsGUI();
}

void SOdysseyPainterEditorToolCollection::RefreshToolsGUI()
{
    UE_LOG(LogTemp, Display, TEXT("REFRESH"));
    if (!mToolWrapBox.IsValid())
        return;

    mToolWrapBox->ClearChildren();

    // Rebuild tool tiles
    for (UOdysseyPainterEditorTool* Tool : mDisplayedTools)
    {
        mToolWrapBox->AddSlot()
            [
                GenerateToolTile(Tool)
            ];
    }

    if( !mToolCollection->IsCollectionTransient() )
    {
        mToolWrapBox->AddSlot()
            [
                SNew(SButton)
                    .ContentPadding(0)
                    .OnClicked(this, &SOdysseyPainterEditorToolCollection::OnAddToolClicked)
                    [
                        SNew(SBox)
                            .WidthOverride(32)
                            .HeightOverride(32)
                            [
                                SNew(STextBlock)
                                    .Text(FText::FromString(TEXT("+")))
                                    .Justification(ETextJustify::Center)
                            ]
                    ]
            ];
    }
}
