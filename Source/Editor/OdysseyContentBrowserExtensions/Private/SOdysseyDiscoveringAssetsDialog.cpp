// Copyright Epic Games, Inc. All Rights Reserved.
// This a copy of SDiscoveringAssetsDialog.cpp private in the source code of UE4
// see: https://udn.unrealengine.com/s/question/0D54z0000775oOGCAY/spackagereportdialog-and-sdiscoveringassetsdialog-are-private

#include "SOdysseyDiscoveringAssetsDialog.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SBoxPanel.h"
#include "Widgets/SWindow.h"
#include "Layout/WidgetPath.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Layout/SBorder.h"
#include "Widgets/Text/STextBlock.h"
#include "Widgets/Input/SButton.h"
#include "EditorStyleSet.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "EditorWidgetsModule.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "DiscoveringAssetsDialog"

SOdysseyDiscoveringAssetsDialog::~SOdysseyDiscoveringAssetsDialog()
{
    if ( FModuleManager::Get().IsModuleLoaded(TEXT("AssetRegistry")) )
    {
        FAssetRegistryModule& assetRegistryModule = FModuleManager::GetModuleChecked<FAssetRegistryModule>("AssetRegistry");
        assetRegistryModule.Get().OnFilesLoaded().RemoveAll(this);
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SOdysseyDiscoveringAssetsDialog::Construct( const FArguments& iInArgs )
{
    mOnAssetsDiscovered = iInArgs._mOnAssetsDiscovered;
    
    FEditorWidgetsModule& editorWidgetsModule = FModuleManager::LoadModuleChecked<FEditorWidgetsModule>("EditorWidgets");
    
    ChildSlot
    [
        SNew(SBorder)
        .BorderImage( FAppStyle::GetBrush("ToolPanel.GroupBorder") )
        .Padding(FMargin(4, 8, 4, 4))
        [
            SNew(SVerticalBox)

            // "Discovering Assets" UI
            +SVerticalBox::Slot()
            .Padding(16, 0)
            .FillHeight(1.f)
            .VAlign(VAlign_Center)
            [
                SNew(SVerticalBox)

                +SVerticalBox::Slot()
                .AutoHeight()
                .Padding(0, 0, 0, 8)
                .HAlign(HAlign_Center)
                [
                    SNew(STextBlock)
                    .Text(LOCTEXT("DiscoveringAssets", "Please wait while assets are being discovered."))
                ]

                +SVerticalBox::Slot()
                .AutoHeight()
                .HAlign(HAlign_Fill)
                [
                    editorWidgetsModule.CreateAssetDiscoveryIndicator(EAssetDiscoveryIndicatorScaleMode::Scale_None, FMargin(0), false)
                ]
            ]

            // Cancel button
            +SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 4)
            .HAlign(HAlign_Right)
            [
                SNew(SButton)
                .OnClicked(this, &SOdysseyDiscoveringAssetsDialog::CancelClicked)
                .Text(LOCTEXT("CancelButton", "Cancel"))
            ]
        ]
    ];

    FAssetRegistryModule& assetRegistryModule = FModuleManager::LoadModuleChecked<FAssetRegistryModule>("AssetRegistry");
    if ( assetRegistryModule.Get().IsLoadingAssets() )
    {
        assetRegistryModule.Get().OnFilesLoaded().AddSP(this, &SOdysseyDiscoveringAssetsDialog::AssetRegistryLoadComplete);
    }
    else
    {
        mOnAssetsDiscovered.ExecuteIfBound();
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SOdysseyDiscoveringAssetsDialog::OpenDiscoveringAssetsDialog(const FOnAssetsDiscovered& iInOnAssetsDiscovered)
{
    TSharedRef<SWindow> renameWindow = SNew(SWindow)
        .Title(LOCTEXT("DiscoveringAssetsDialog", "Discovering Assets..."))
        .SizingRule( ESizingRule::Autosized )
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        [
            SNew(SOdysseyDiscoveringAssetsDialog)
            .mOnAssetsDiscovered(iInOnAssetsDiscovered)
        ];

    IMainFrameModule& mainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));

    if ( mainFrameModule.GetParentWindow().IsValid() )
    {
        FSlateApplication::Get().AddWindowAsNativeChild(renameWindow, mainFrameModule.GetParentWindow().ToSharedRef());
    }
    else
    {
        FSlateApplication::Get().AddWindow(renameWindow);
    }
}

FReply SOdysseyDiscoveringAssetsDialog::CancelClicked()
{
    CloseDialog();

    return FReply::Handled();
}

void SOdysseyDiscoveringAssetsDialog::AssetRegistryLoadComplete()
{
    mOnAssetsDiscovered.ExecuteIfBound();

    CloseDialog();
}

void SOdysseyDiscoveringAssetsDialog::CloseDialog()
{
    TSharedPtr<SWindow> window = FSlateApplication::Get().FindWidgetWindow(AsShared());

    if ( window.IsValid() )
    {
        window->RequestDestroyWindow();
    }
}

#undef LOCTEXT_NAMESPACE
