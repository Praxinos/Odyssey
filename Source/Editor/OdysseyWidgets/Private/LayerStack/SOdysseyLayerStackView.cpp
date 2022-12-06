// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "LayerStack/SOdysseyLayerStackView.h"
#include "LayerStack/SOdysseyLayerStackTreeView.h"

#include "Framework/Application/SlateApplication.h"
#include "EditorStyleSet.h"
#include "EditorFontGlyphs.h"

#include "Widgets/Input/SComboButton.h"
#include "Widgets/Layout/SSplitter.h"
#include "Widgets/Layout/SSpacer.h"
#include "Widgets/Layout/SScrollBox.h"


#define LOCTEXT_NAMESPACE "OdysseyLayerStack"

//CONSTRUCTION/DESTRUCTION-----------------------------------------------


//TODO: Make it a SListView, it will be much much easier

SOdysseyLayerStackView::~SOdysseyLayerStackView()
{
    //delete LayerStackModelPtr;
}


void SOdysseyLayerStackView::Construct(const FArguments& InArgs)
{
    mCurrentLayerStack = nullptr;
    mLayerStackModelPtr = new FOdysseyLayerStackModel( MakeShareable( this ), InArgs._LayerStackData );

    //SAssignNew(mTreeView, SOdysseyLayerStackTreeView, mLayerStackModelPtr->GetNodeTree());
	SAssignNew(mTreeView, SOdysseyLayerStackTreeView)
		.NodeTree(mLayerStackModelPtr, &FOdysseyLayerStackModel::GetNodeTree);

	mLayerStackModelPtr->OnUpdated().AddSP(mTreeView.ToSharedRef(), &SOdysseyLayerStackTreeView::Refresh, -1);

    ChildSlot
    [
		SNew(SVerticalBox)
		+ SVerticalBox::Slot()
		.AutoHeight()
		[
			SNew(STextBlock)
			.Visibility(this, &SOdysseyLayerStackView::LayerIsMissingWarningVisibility)
			.Text(LOCTEXT("EmptyLayerStackInstructions", "No Layer Stack can be displayed"))
		]

		+ SVerticalBox::Slot()
		[
			SNew(SBorder)
			.Visibility(this, &SOdysseyLayerStackView::LayerStackVisibility)
			[
				SNew(SVerticalBox)
				//Here we put the current layer infos
				+SVerticalBox::Slot()
				.AutoHeight()
				.Padding(4.f)
				.Expose(mTreeView->GetPropertyView())
				[
					SNullWidget::NullWidget
				]

				+ SVerticalBox::Slot()
				.AutoHeight()
				[
					MakeAddButton()
                ]

                + SVerticalBox::Slot()
				[
                    SNew(SScrollBox)
                    .Orientation(Orient_Vertical)
                    .ScrollBarAlwaysVisible(false)
                    + SScrollBox::Slot()
                    [
                        mTreeView.ToSharedRef()
                    ]
				]
			]
		]
    ];

    if( mTreeView->GetNodeTree()->Update() != -1 ) //Empty stack
        mTreeView->Refresh();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- SWidget overrides

void
SOdysseyLayerStackView::Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime)
{
    FOdysseyLayerStack* layerstack = mLayerStackModelPtr->GetLayerStackData();
    if (layerstack != mCurrentLayerStack)
    {
        mCurrentLayerStack = layerstack;
        mTreeView->Refresh();
    }
}


//PUBLIC API-------------------------------------------------------------

TSharedPtr<SOdysseyLayerStackTreeView> SOdysseyLayerStackView::GetTreeView() const
{
    return mTreeView;
}

void SOdysseyLayerStackView::RefreshView()
{
    mTreeView->Refresh();
}

//PRIVATE API-----------------------------------------------------------

void SOdysseyLayerStackView::GetContextMenuContent(FMenuBuilder& ioMenuBuilder)
{
    ioMenuBuilder.BeginSection("AddLayers");

    if (mLayerStackModelPtr)
    {
        mLayerStackModelPtr->BuildAddLayerMenu(ioMenuBuilder);
    }

    ioMenuBuilder.EndSection();
}

TSharedRef<SWidget> SOdysseyLayerStackView::MakeAddButton()
{
    return SNew(SComboButton)
    .OnGetMenuContent(this, &SOdysseyLayerStackView::MakeAddMenu)
    .ButtonStyle(FAppStyle::Get(), "FlatButton.Success")
    .ContentPadding(FMargin(2.0f, 5.0f))
    .HasDownArrow(false)
    .ButtonContent()
    [
        SNew(SHorizontalBox)
        + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .AutoWidth()
        [
            SNew(STextBlock)
            .TextStyle(FAppStyle::Get(), "NormalText.Important")
            .Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
            .Text(FEditorFontGlyphs::Plus)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(4, 0, 0, 0)
        [
            SNew(STextBlock)
            .TextStyle(FAppStyle::Get(), "NormalText.Important")
            .Text(LOCTEXT("Layer", "Layer"))
        ]

        + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .AutoWidth()
        .Padding(4, 0, 0, 0)
        [
            SNew(STextBlock)
            .TextStyle(FAppStyle::Get(), "NormalText.Important")
            .Font(FAppStyle::Get().GetFontStyle("FontAwesome.10"))
            .Text(FEditorFontGlyphs::Caret_Down)
        ]
     ];
}

TSharedRef<SWidget> SOdysseyLayerStackView::MakeAddMenu()
{
    FMenuBuilder MenuBuilder(true, nullptr, mAddMenuExtender);
    {
        GetContextMenuContent(MenuBuilder);
    }

    return MenuBuilder.MakeWidget();
}

EVisibility
SOdysseyLayerStackView::LayerIsMissingWarningVisibility() const
{
	return mLayerStackModelPtr->GetNodeTree().IsValid() && mLayerStackModelPtr->GetLayerStackData() ? EVisibility::Hidden : EVisibility::Visible;
}

EVisibility
SOdysseyLayerStackView::LayerStackVisibility() const
{
	return mLayerStackModelPtr->GetNodeTree().IsValid() && mLayerStackModelPtr->GetLayerStackData() ? EVisibility::Visible : EVisibility::Hidden;
}


#undef LOCTEXT_NAMESPACE
