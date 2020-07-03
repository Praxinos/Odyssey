// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

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


SOdysseyLayerStackView::~SOdysseyLayerStackView()
{
    //delete LayerStackModelPtr;
}


void SOdysseyLayerStackView::Construct(const FArguments& InArgs)
{
    mLayerStackModelPtr = new FOdysseyLayerStackModel( MakeShareable( this ), InArgs._LayerStackData );

    SAssignNew(mTreeView, SOdysseyLayerStackTreeView, mLayerStackModelPtr->GetNodeTree());

    ChildSlot
    [
        //Here we put the current layer infos
        SNew(SVerticalBox)
        + SVerticalBox::Slot()
        .AutoHeight()
        .Padding( 16.f )
        .Expose( mTreeView->GetPropertyView() )
        [
            SNullWidget::NullWidget
        ]

        + SVerticalBox::Slot()
        [
            SNew(SSplitter)
            .Orientation(Orient_Vertical)
            .PhysicalSplitterHandleSize( 0 )
            .HitDetectionSplitterHandleSize( 0 )

            + SSplitter::Slot()
            .SizeRule( SSplitter::ESizeRule::SizeToContent )
            [
                MakeAddButton()
            ]
            + SSplitter::Slot()
            [
                SNew(SScrollBox)
                .Orientation(Orient_Vertical)
                .ScrollBarAlwaysVisible(false)
                +SScrollBox::Slot()
                [
                    mTreeView.ToSharedRef()
                ]
            ]
        ]
    ];

    if( mTreeView->GetNodeTree()->Update() != -1 ) //Empty stack
        mTreeView->Refresh();
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
    .ButtonStyle(FEditorStyle::Get(), "FlatButton.Success")
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
            .TextStyle(FEditorStyle::Get(), "NormalText.Important")
            .Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
            .Text(FEditorFontGlyphs::Plus)
        ]

        + SHorizontalBox::Slot()
        .AutoWidth()
        .Padding(4, 0, 0, 0)
        [
            SNew(STextBlock)
            .TextStyle(FEditorStyle::Get(), "NormalText.Important")
            .Text(LOCTEXT("Layer", "Layer"))
        ]

        + SHorizontalBox::Slot()
        .VAlign(VAlign_Center)
        .AutoWidth()
        .Padding(4, 0, 0, 0)
        [
            SNew(STextBlock)
            .TextStyle(FEditorStyle::Get(), "NormalText.Important")
            .Font(FEditorStyle::Get().GetFontStyle("FontAwesome.10"))
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


#undef LOCTEXT_NAMESPACE
