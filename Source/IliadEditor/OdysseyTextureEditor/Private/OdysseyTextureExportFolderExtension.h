// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once
#include "CoreMinimal.h"
#include "ContentBrowserModule.h"
#include "Widgets/SOdysseyPackageReportDialog.h"
#include <ULIS>

class FOdysseyTextureExportFolderExtension
{
public:
    static void Register( FContentBrowserModule& iContentBrowserModule );

private:
    static void ExecuteExportFolder( TArray<FString> iSelectedPaths );
    static void PopulateOdysseyActionsSubMenu( FMenuBuilder& ioMenuBuilder, const TArray<FString> iSelectedPaths );
    static void ExportFolderTextures( const TArray<FName>& iPackageNamesToExport );
    static void PerformExportFolder( TArray<FName> iPackageNamesToExport );
    static void ExportFolder_ReportConfirmed( TEnumAsByte<EExportImageFormat> iExportImageFormat, TSharedPtr<TArray<ReportPackageData>> iPackageDataToExport, FString iDestinationFolder );
    static void ExportFile( UTexture2D* iCurrentTexture, FString iSystemPathNameExt, ::ULIS::eFileFormat iExportFormat );
    static void RecursiveGetDependencies(const FName& iPackageName, TSet<FName>& ioAllDependencies, const FString& iOriginalRoot);
};
