// Copyright Epic Games, Inc. All Rights Reserved.
// This a copy of SPackageReportDialog.cpp private in the source code of UE4
// see: https://udn.unrealengine.com/s/question/0D54z0000775oOGCAY/spackagereportdialog-and-sdiscoveringassetsdialog-are-private


#include "SOdysseyPackageReportDialog.h"
#include "Modules/ModuleManager.h"
#include "Widgets/SWindow.h"
#include "Layout/WidgetPath.h"
#include "SlateOptMacros.h"
#include "Framework/Application/SlateApplication.h"
#include "Widgets/Images/SImage.h"
#include "Widgets/Layout/SUniformGridPanel.h"
#include "Widgets/Input/SCheckBox.h"
#include "Widgets/Input/SButton.h"
#include "EditorStyleSet.h"
#include "Interfaces/IMainFrameModule.h"

#define LOCTEXT_NAMESPACE "PackageReportDialog"

struct FCompareFPackageReportNodeByName
{
    FORCEINLINE bool operator()( TSharedPtr<FPackageReportNode> A, TSharedPtr<FPackageReportNode> B ) const
    {
        return A->NodeName < B->NodeName;
    }
};

FPackageReportNode::FPackageReportNode()
    : bIsChecked(true)
    , bIsActive(true)
    , bShouldExportPackage(nullptr)
    , bIsFolder(false)
    , Parent(nullptr)
{}

FPackageReportNode::FPackageReportNode(const FString& iInNodeName, bool iInIsFolder)
    : NodeName(iInNodeName)
    , bIsChecked(true)
    , bIsActive(true)
    , bShouldExportPackage(nullptr)
    , bIsFolder(iInIsFolder)
    , Parent(nullptr)
{}

void FPackageReportNode::AddPackage(const FString& iPackageName, bool* bInShouldExportFolder)
{
    TArray<FString> pathElements;
    iPackageName.ParseIntoArray(pathElements, TEXT("/"), /*InCullEmpty=*/true);

    return AddPackage_Recursive(pathElements, bInShouldExportFolder);
}

void FPackageReportNode::ExpandChildrenRecursively(const TSharedRef<PackageReportTree>& iTreeview)
{
    for ( auto childIt = Children.CreateConstIterator(); childIt; ++childIt )
    {
        iTreeview->SetItemExpansion(*childIt, true);
        (*childIt)->ExpandChildrenRecursively(iTreeview);
    }
}

void FPackageReportNode::AddPackage_Recursive(TArray<FString>& iPathElements, bool* bInShouldExportFolder)
{
    if ( iPathElements.Num() > 0 )
    {
        // Pop the bottom element
        FString childNodeName = iPathElements[0];
        iPathElements.RemoveAt(0);

        // Try to find a child which uses this folder name
        TSharedPtr<FPackageReportNode> child;
        for ( auto childIt = Children.CreateConstIterator(); childIt; ++childIt )
        {
            if ( (*childIt)->NodeName == childNodeName )
            {
                child = (*childIt);
                break;
            }
        }

        // If one was not found, create it
        if ( !child.IsValid() )
        {
            const bool bIsAFolder = (iPathElements.Num() > 0);
            int32 childIdx = Children.Add( MakeShareable(new FPackageReportNode(childNodeName, bIsAFolder)) );
            child = Children[childIdx];
            child.Get()->Parent = this;
            Children.Sort( FCompareFPackageReportNodeByName() );
        }

        if ( ensure(child.IsValid()) )
        {
            child->AddPackage_Recursive(iPathElements, bInShouldExportFolder);
        }
    }
    else 
    {
        bShouldExportPackage = bInShouldExportFolder;
    }
}

BEGIN_SLATE_FUNCTION_BUILD_OPTIMIZATION
void SOdysseyPackageReportDialog::Construct( const FArguments& iInArgs, const FText& iInReportMessage, TArray<ReportPackageData>& iInPackageNames, const FOnReportConfirmed& iInOnReportConfirmed )
{
    mOnReportConfirmed = iInOnReportConfirmed;
    mFolderOpenBrush = FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderOpen");
    mFolderClosedBrush = FAppStyle::GetBrush("ContentBrowser.AssetTreeFolderClosed");
    mPackageBrush = FAppStyle::GetBrush("ContentBrowser.ColumnViewAssetIcon");

    ConstructNodeTree(iInPackageNames);
    
    ChildSlot
    [
        SNew(SBorder)
        .BorderImage( FAppStyle::GetBrush("Docking.Tab.ContentAreaBrush") )
        .Padding(FMargin(4, 8, 4, 4))
        [
            SNew(SVerticalBox)

            // Report Message
            +SVerticalBox::Slot()
            .AutoHeight()
            .Padding(0, 4)
            [
                SNew(STextBlock)
                .Text(iInReportMessage)
                .TextStyle( FAppStyle::Get(), "PackageMigration.DialogTitle" )
            ]

            // Tree of packages in the report
            +SVerticalBox::Slot()
            .FillHeight(1.f)
            [
                SNew(SBorder)
                .BorderImage( FAppStyle::GetBrush("ToolPanel.GroupBorder") )
                [
                    SAssignNew( mReportTreeView, PackageReportTree )
                    .TreeItemsSource(&mPackageReportRootNode.Children)
                    .ItemHeight(18)
                    .SelectionMode(ESelectionMode::Single)
                    .OnGenerateRow( this, &SOdysseyPackageReportDialog::GenerateTreeRow )
                    .OnGetChildren( this, &SOdysseyPackageReportDialog::GetChildrenForTree )
                ]
            ]

             // Select the export format
            +SVerticalBox::Slot()
            .AutoHeight()
            .Padding( 10, 10 )
            [
                SNew( SHorizontalBox )
                +SHorizontalBox::Slot()
                [
                    SNew( STextBlock )
                    .Text( LOCTEXT( "format-export", "Image(s) Export Format :" ) )
                ]
                +SHorizontalBox::Slot()
                .FillWidth( 2 )
                [
                    SAssignNew( mExportFormatComboBox, SEnumComboBox, StaticEnum<EExportImageFormat>() )
                    .CurrentValue( this, &SOdysseyPackageReportDialog::GetSelectedExportImageFormat )
                    .OnEnumSelectionChanged( this, &SOdysseyPackageReportDialog::HandleOnFormatChanged )
                ]
            ]

            // Ok/Cancel buttons
            +SVerticalBox::Slot()
            .AutoHeight()
            .HAlign(HAlign_Right)
            .Padding(0,4,0,0)
            [
                SNew(SUniformGridPanel)
                .SlotPadding(FAppStyle::GetMargin("StandardDialog.SlotPadding"))
                .MinDesiredSlotWidth(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotWidth"))
                .MinDesiredSlotHeight(FAppStyle::GetFloat("StandardDialog.MinDesiredSlotHeight"))
                +SUniformGridPanel::Slot(0,0)
                [
                    SNew(SButton)
                    .HAlign(HAlign_Center)
                    .ContentPadding( FAppStyle::GetMargin("StandardDialog.ContentPadding") )
                    .OnClicked(this, &SOdysseyPackageReportDialog::OkClicked)
                    .Text(LOCTEXT("OkButton", "OK"))
                ]
                +SUniformGridPanel::Slot(1,0)
                [
                    SNew(SButton)
                    .HAlign(HAlign_Center)
                    .ContentPadding( FAppStyle::GetMargin("StandardDialog.ContentPadding") )
                    .OnClicked(this, &SOdysseyPackageReportDialog::CancelClicked)
                    .Text(LOCTEXT("CancelButton", "Cancel"))
                ]
            ]
        ]
    ];

    if ( ensure(mReportTreeView.IsValid()) )
    {
        mPackageReportRootNode.ExpandChildrenRecursively(mReportTreeView.ToSharedRef());
    }
}
END_SLATE_FUNCTION_BUILD_OPTIMIZATION

void SOdysseyPackageReportDialog::OpenPackageReportDialog(const FText& iReportMessage, TArray<ReportPackageData>& iPackageNames, const FOnReportConfirmed& iInOnReportConfirmed)
{
    TSharedRef<SWindow> reportWindow = SNew(SWindow)
        .Title(LOCTEXT("ReportWindowTitle", "Asset Report"))
        .ClientSize( FVector2D(600, 500) )
        .SupportsMaximize(false)
        .SupportsMinimize(false)
        [
            SNew(SOdysseyPackageReportDialog, iReportMessage, iPackageNames, iInOnReportConfirmed)
        ];
        
    IMainFrameModule& mainFrameModule = FModuleManager::LoadModuleChecked<IMainFrameModule>(TEXT("MainFrame"));
    if ( mainFrameModule.GetParentWindow().IsValid() )
    {
        FSlateApplication::Get().AddWindowAsNativeChild(reportWindow, mainFrameModule.GetParentWindow().ToSharedRef());
    }
    else
    {
        FSlateApplication::Get().AddWindow(reportWindow);
    }
}

void SOdysseyPackageReportDialog::CloseDialog()
{
    TSharedPtr<SWindow> window = FSlateApplication::Get().FindWidgetWindow(AsShared());

    if ( window.IsValid() )
    {
        window->RequestDestroyWindow();
    }
}

TSharedRef<ITableRow> SOdysseyPackageReportDialog::GenerateTreeRow( TSharedPtr<FPackageReportNode> iTreeItem, const TSharedRef<STableViewBase>& iOwnerTable)
{
    check(iTreeItem.IsValid());

    const FSlateBrush* iconBrush = GetNodeIcon(iTreeItem);

    return SNew( STableRow< TSharedPtr<FPackageReportNode> >, iOwnerTable )
        [
            // Icon
            SNew(SHorizontalBox)
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SCheckBox)
                .OnCheckStateChanged(this, &SOdysseyPackageReportDialog::CheckBoxStateChanged, iTreeItem, iOwnerTable)
                .IsChecked(this, &SOdysseyPackageReportDialog::GetEnabledCheckState, iTreeItem)
                .IsEnabled(iTreeItem.Get()->Parent == nullptr ? true : iTreeItem.Get()->Parent->bIsActive)

            ]
            +SHorizontalBox::Slot()
            .AutoWidth()
            [
                SNew(SImage).Image(iconBrush)
            ]
            // Name
            +SHorizontalBox::Slot()
            .FillWidth(1.f)
            [
                SNew(STextBlock).Text(FText::FromString(iTreeItem->NodeName))
                .ColorAndOpacity(iTreeItem.Get()->bIsActive ? FSlateColor::UseForeground() : FSlateColor::UseSubduedForeground())
            ]
        ];
}

ECheckBoxState SOdysseyPackageReportDialog::GetEnabledCheckState(TSharedPtr<FPackageReportNode> iTreeItem) const
{
    return iTreeItem.Get()->bIsChecked ? ECheckBoxState::Checked : ECheckBoxState::Unchecked;
}

void SOdysseyPackageReportDialog::SetStateRecursive(TSharedPtr<FPackageReportNode> iTreeItem, bool bWasChecked)
{
    if (iTreeItem.Get() == nullptr) return;

    iTreeItem.Get()->bIsActive = bWasChecked && iTreeItem.Get()->bIsChecked;
    if (iTreeItem.Get()->bShouldExportPackage)
    {
        *(iTreeItem.Get()->bShouldExportPackage) = iTreeItem.Get()->bIsActive;
    }

    TArray< TSharedPtr<FPackageReportNode> > children;
    GetChildrenForTree(iTreeItem, children);
    for (int i = 0; i < children.Num(); i++)
    {
        if (children[i].Get() == nullptr) continue;

        SetStateRecursive(children[i], iTreeItem.Get()->bIsActive);
    }
}

void SOdysseyPackageReportDialog::CheckBoxStateChanged(ECheckBoxState iInCheckBoxState, TSharedPtr<FPackageReportNode> iTreeItem, TSharedRef<STableViewBase> iOwnerTable)
{
    iTreeItem.Get()->bIsChecked = iInCheckBoxState == ECheckBoxState::Checked;
    SetStateRecursive(iTreeItem, iInCheckBoxState == ECheckBoxState::Checked);
    iOwnerTable.Get().RebuildList();
}

void SOdysseyPackageReportDialog::GetChildrenForTree( TSharedPtr<FPackageReportNode> iTreeItem, TArray< TSharedPtr<FPackageReportNode> >& iOutChildren )
{
    iOutChildren = iTreeItem->Children;
}

void SOdysseyPackageReportDialog::ConstructNodeTree(TArray<ReportPackageData>& iPackageNames)
{
    for (ReportPackageData& package : iPackageNames)
    {
        mPackageReportRootNode.AddPackage(package.Name, &package.bShouldExportPackage);
    }
}

const FSlateBrush* SOdysseyPackageReportDialog::GetNodeIcon(const TSharedPtr<FPackageReportNode>& iReportNode) const
{
    if ( !iReportNode->bIsFolder )
    {
        return mPackageBrush;
    }
    else if ( mReportTreeView->IsItemExpanded(iReportNode) )
    {
        return mFolderOpenBrush;
    }
    else
    {
        return mFolderClosedBrush;
    }
}

FReply SOdysseyPackageReportDialog::OkClicked()
{
    CloseDialog();
    mOnReportConfirmed.ExecuteIfBound( mExportFormat );

    return FReply::Handled();
}

FReply SOdysseyPackageReportDialog::CancelClicked()
{
    CloseDialog();

    return FReply::Handled();
}

void
SOdysseyPackageReportDialog::HandleOnFormatChanged( int32 iSelection, ESelectInfo::Type iSelectInfo )
{
    mExportFormat = TEnumAsByte<EExportImageFormat>( iSelection ); 
}

int32
SOdysseyPackageReportDialog::GetSelectedExportImageFormat() const
{
    return mExportFormat;
}

::ULIS::eFileFormat
SOdysseyPackageReportDialog::GetUlisExportImageFormat( TEnumAsByte<EExportImageFormat> iExportImageFormat )
{
    switch( iExportImageFormat )
    {
        case ExportImageFormat_PNG:          return ::ULIS::FileFormat_png;
        case ExportImageFormat_BMP:          return ::ULIS::FileFormat_bmp;
        case ExportImageFormat_TGA:          return ::ULIS::FileFormat_tga;
        case ExportImageFormat_JPG:          return ::ULIS::FileFormat_jpg;
        case ExportImageFormat_HDR:          return ::ULIS::FileFormat_hdr;
        default: return ::ULIS::FileFormat_png;
    }
}

#undef LOCTEXT_NAMESPACE
