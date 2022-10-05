// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once
#include "CoreMinimal.h"
#include "ContentBrowserModule.h"
#include "SOdysseyPackageReportDialog.h"
#include <ULIS>

class FOdysseyExportFolderExtension
{
public:
    static void Register( FContentBrowserModule& iContentBrowserModule );

private:
    static void ExecuteExportFolder( TArray<FString> iSelectedPaths );
    static void PopulateIliadActionsSubMenu( FMenuBuilder& ioMenuBuilder, const TArray<FString> iSelectedPaths );
    static void ExportFolderTextures( const TArray<FName>& iPackageNamesToExport );
    static void PerformExportFolder( TArray<FName> iPackageNamesToExport );
    static void ExportFolder_ReportConfirmed( TEnumAsByte<EExportImageFormat> iExportImageFormat, TSharedPtr<TArray<ReportPackageData>> iPackageDataToExport, FString iDestinationFolder );
    static void ExportFile( UTexture2D* iCurrentTexture, FString iSystemPathNameExt, ::ULIS::eFileFormat iExportFormat );
    static void RecursiveGetDependencies(const FName& iPackageName, TSet<FName>& ioAllDependencies, const FString& iOriginalRoot);
};

