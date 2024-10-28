// Copyright Epic Games, Inc. All Rights Reserved.
// This a copy of SPackageReportDialog.h private in the source code of UE4
// see: https://udn.unrealengine.com/s/question/0D54z0000775oOGCAY/spackagereportdialog-and-sdiscoveringassetsdialog-are-private

#pragma once

#include "CoreMinimal.h"
#include "Input/Reply.h"
#include "Widgets/DeclarativeSyntaxSupport.h"
#include "Widgets/SCompoundWidget.h"
#include "Widgets/Views/STableViewBase.h"
#include "Widgets/Views/STableRow.h"
#include "Widgets/Views/STreeView.h"
#include "SEnumCombo.h"
#include <ULIS>
#include "SOdysseyPackageReportDialog.generated.h"

UENUM()
enum EExportImageFormat
{
    ExportImageFormat_PNG UMETA(DisplayName="PNG       Portable Network Graphics"),
    ExportImageFormat_BMP UMETA(DisplayName="BMP       Windows bitmap"),
    ExportImageFormat_TGA UMETA(DisplayName="TGA       Truevision Targa"),
    ExportImageFormat_JPG UMETA(DisplayName="JPG       Joint Photographic Experts Group"),
    ExportImageFormat_HDR UMETA(DisplayName="HDR       High Dynamic Range", Hidden),
    ExportImageFormat_MAX UMETA(DisplayName="MAX"),
};

struct FPackageReportNode;

typedef STreeView< TSharedPtr<struct FPackageReportNode> > PackageReportTree;

struct ReportPackageData
{
    FString Name;
    bool bShouldExportPackage;
};

struct FPackageReportNode
{
    /** The name of the tree node without the path */
    FString NodeName; 
    /** A user-exposed flag determining whether the content of this node and its children should be exported or not. */
    bool bIsChecked;
    /** A flag determining whether this node should be exported or not. This node is active as long as bIsChecked is true and if all the parent nodes are also checked. */
    bool bIsActive;
    /** A pointer to an external bool describing whether this node ultimately should be exported or not. Is only non-null for leaf nodes.*/
    bool* bShouldExportPackage;
    /** If true, this node is a folder instead of a package */
    bool bIsFolder;
    
    /** The parent of this node */
    FPackageReportNode* Parent;

    /** The children of this node */
    TArray< TSharedPtr<FPackageReportNode> > Children;

    /** Constructor */
    FPackageReportNode();
    FPackageReportNode(const FString& iInNodeName, bool iInIsFolder);

    /** Adds the path to the tree relative to this node, creating nodes as needed. */
    void AddPackage(const FString& iPackageName, bool* bInIsPackageIncluded);

    /** Expands this node and all its children */
    void ExpandChildrenRecursively(const TSharedRef<PackageReportTree>& iTreeview);

private:
    /** Helper function for AddPackage. PathElements is the tokenized path delimited by "/" */
    void AddPackage_Recursive(TArray<FString>& iPathElements, bool* bInIsPackageIncluded);
};

class SOdysseyPackageReportDialog : public SCompoundWidget
{
public:
    DECLARE_DELEGATE_OneParam( FOnReportConfirmed, TEnumAsByte<EExportImageFormat> )

    SLATE_BEGIN_ARGS( SOdysseyPackageReportDialog ){}

    SLATE_END_ARGS()

    /** Constructs this widget with InArgs */
    void Construct( const FArguments& iInArgs, const FText& iInReportMessage, TArray<ReportPackageData>& iInPackageNames, const FOnReportConfirmed& iInOnReportConfirmed );

    /** Opens the dialog in a new window */
    static void OpenPackageReportDialog(const FText& iReportMessage, TArray<ReportPackageData>& iPackageNames, const FOnReportConfirmed& iInOnReportConfirmed);

    /** Closes the dialog. */
    void CloseDialog();

    static ::ULIS::eFileFormat GetUlisExportImageFormat( TEnumAsByte<EExportImageFormat> iExportImageFormat );

private:
    /** Recursively sets the checked/active state of every child of this node in the tree when a checkbox is toggled. */
    void SetStateRecursive(TSharedPtr<FPackageReportNode> iTreeItem, bool bWasChecked);

    /** Callback to check whether a checkbox is checked or not. */
    ECheckBoxState GetEnabledCheckState(TSharedPtr<FPackageReportNode> iTreeItem) const;

    /** Callback called whenever a checkbox is toggled. */
    void CheckBoxStateChanged(ECheckBoxState iInCheckBoxState, TSharedPtr<FPackageReportNode> iTreeItem, TSharedRef<STableViewBase> iOwnerTable);

    /** Constructs the node tree given the list of package names */
    void ConstructNodeTree(TArray<ReportPackageData>& iPackageNames);

    /** Handler to generate a row in the report tree */
    TSharedRef<ITableRow> GenerateTreeRow( TSharedPtr<FPackageReportNode> iTreeItem, const TSharedRef<STableViewBase>& iOwnerTable );

    /** Gets the children for the specified tree item */
    void GetChildrenForTree( TSharedPtr<FPackageReportNode> iTreeItem, TArray< TSharedPtr<FPackageReportNode> >& iOutChildren );

    /** Determines which image to display next to a node */
    const FSlateBrush* GetNodeIcon(const TSharedPtr<FPackageReportNode>& iReportNode) const;

    /** Handler for when "Ok" is clicked */
    FReply OkClicked();

    /** Handler for when "Cancel" is clicked */
    FReply CancelClicked();

    void                HandleOnFormatChanged( int32 iSelection, ESelectInfo::Type iSelectInfo );
    int32               GetSelectedExportImageFormat() const;

private:
    FOnReportConfirmed                                          mOnReportConfirmed;
    FPackageReportNode                                          mPackageReportRootNode;
    TSharedPtr<PackageReportTree>                               mReportTreeView;
    TEnumAsByte<EExportImageFormat>                             mExportFormat;
    TSharedPtr<SEnumComboBox>                                   mExportFormatComboBox;

    /** Brushes for the different node states */
    const FSlateBrush*                                          mFolderOpenBrush;
    const FSlateBrush*                                          mFolderClosedBrush;
    const FSlateBrush*                                          mPackageBrush;

};
