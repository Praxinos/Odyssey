// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
// Fyi : Original code is here : C:\Program Files\Epic Games\UE_4.27\Engine\Source\Developer\AssetTools\Private\AssetTools.cpp

#include "OdysseyExportFolderExtension.h"
#include "SOdysseyDiscoveringAssetsDialog.h"
#include "AssetRegistry/AssetRegistryModule.h"
#include "AssetToolsModule.h"
#include "FileHelpers.h"
#include "Misc/MessageDialog.h"
#include "Misc/ScopedSlowTask.h"
#include "AssetRegistry/ARFilter.h"
#include "IDesktopPlatform.h"
#include "DesktopPlatformModule.h"
#include "OdysseyScopedTextureSettings.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyPixelFormat.h"
#include "EditorDirectories.h"
#include "ULISLoaderModule.h"
#include "Misc/Paths.h"
#include <string>

#define LOCTEXT_NAMESPACE "OdysseyExportFolderExtension"

/*-----------------------------------------------------------------------------
   FOdysseyExportFolderExtension
-----------------------------------------------------------------------------*/

void
FOdysseyExportFolderExtension::Register( FContentBrowserModule& iContentBrowserModule )
{
    iContentBrowserModule.GetAllPathViewContextMenuExtenders().Add( FContentBrowserMenuExtender_SelectedPaths::CreateLambda( [] ( const TArray<FString>& iSelectedPaths )
    {
        TSharedRef<FExtender> extender = MakeShared<FExtender>();
        extender->AddMenuExtension(
            "PathViewFolderOptions",
            EExtensionHook::First,
            TSharedPtr<FUICommandList>(),
            FMenuExtensionDelegate::CreateLambda( [iSelectedPaths] ( FMenuBuilder& ioMenuBuilder )
            {
                ioMenuBuilder.AddSubMenu(
                  LOCTEXT( "CB_Extension_Texture_IliadActions", "ILIAD Actions" )
                , LOCTEXT( "CB_Extension_Texture_IliadActions_ToolTip", "All actions related to ILIAD" )
                , FNewMenuDelegate::CreateStatic( &FOdysseyExportFolderExtension::PopulateIliadActionsSubMenu, iSelectedPaths )
                , false
                , FSlateIcon( "OdysseyStyle", "OdysseyLogo.Iliad16" )
                );
            }));
        return extender;
    }));
}

void
FOdysseyExportFolderExtension::PopulateIliadActionsSubMenu( FMenuBuilder& ioMenuBuilder, const TArray<FString> iSelectedPaths )
{
    ioMenuBuilder.AddMenuEntry(
        LOCTEXT( "MyActionTitle", "Export all Textures" ),
        LOCTEXT( "MyActionTooltip", "Export all Textures within the selected folder" ),
        FSlateIcon( "OdysseyStyle", "OdysseyTexture.ExportTexture_16" ),
        FUIAction( FExecuteAction::CreateStatic( &FOdysseyExportFolderExtension::ExecuteExportFolder, iSelectedPaths ) )
    );
}

void
FOdysseyExportFolderExtension::ExecuteExportFolder( TArray<FString> iSelectedPaths )
{
    const FString& sourcesPath = iSelectedPaths[0];
    if ( ensure( sourcesPath.Len()) )
    {
        // @todo Make sure the asset registry has completed discovering assets, or else GetAssetsByPath() will not find all the assets in the folder! Add some UI to wait for this with a cancel button
        FAssetRegistryModule& assetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>( TEXT("AssetRegistry") );
        if ( assetRegistryModule.Get().IsLoadingAssets() )
        {
            FMessageDialog::Open( EAppMsgType::Ok, LOCTEXT( "ExportFolderAssetsNotDiscovered", "You must wait until asset discovery is complete to export the folder content" ) );
            return;
        }

        // Get a list of package names for input into ExportFolderTextures
        TArray<FAssetData> assetDataList;
        TArray<FName> packageNames;

        // Creating a filter to get only the UTextures2D, recursively
        FARFilter filter;
        filter.bRecursivePaths = true;
        filter.ClassPaths.Add( UTexture2D::StaticClass()->GetClassPathName() );

        for (int32 pathIdx = 0; pathIdx < iSelectedPaths.Num(); ++pathIdx)
        {
            filter.PackagePaths.Emplace(*iSelectedPaths[pathIdx]);
        }

        // Query for a list of assets in the selected paths
        assetRegistryModule.Get().GetAssets(filter, assetDataList);

        for ( auto assetIt = assetDataList.CreateConstIterator(); assetIt; ++assetIt )
        {
            packageNames.Add( (*assetIt).PackageName );
        }

        // Load all the assets in the selected paths
        ExportFolderTextures( packageNames );
    }
}

void FOdysseyExportFolderExtension::ExportFolderTextures(const TArray<FName>& iPackageNamesToExport)
{
    // Packages must be saved for the migration to work
    const bool bPromptUserToSave = true;
    const bool bSaveMapPackages = true;
    const bool bSaveContentPackages = true;
    if ( FEditorFileUtils::SaveDirtyPackages( bPromptUserToSave, bSaveMapPackages, bSaveContentPackages ) )
    {
        FAssetRegistryModule& assetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
        if ( assetRegistryModule.Get().IsLoadingAssets() )
        {
            // Open a dialog asking the user to wait while assets are being discovered
            SOdysseyDiscoveringAssetsDialog::OpenDiscoveringAssetsDialog(
                SOdysseyDiscoveringAssetsDialog::FOnAssetsDiscovered::CreateStatic(&FOdysseyExportFolderExtension::PerformExportFolder, iPackageNamesToExport)
            );
        }
        else
        {
            // Assets are already discovered, perform the migration now
            PerformExportFolder( iPackageNamesToExport );
        }
    }
}

void FOdysseyExportFolderExtension::PerformExportFolder(TArray<FName> iPackageNamesToExport)
{
    // Choose a destination folder
    IDesktopPlatform* desktopPlatform = FDesktopPlatformModule::Get();
    FString destinationFolder;
    if ( ensure(desktopPlatform) )
    {
        const void* parentWindowWindowHandle = FSlateApplication::Get().FindBestParentWindowHandleForDialogs(nullptr);

        const FString title = LOCTEXT("ExportToFolderTitle", "Choose a destination Export folder").ToString();

        const bool bFolderSelected = desktopPlatform->OpenDirectoryDialog(
            parentWindowWindowHandle,
            title,
            FEditorDirectories::Get().GetLastDirectory(ELastDirectory::GENERIC_EXPORT),
            destinationFolder
            );

        if ( !bFolderSelected )
        {
            // User canceled, return
            return;
        }

        FEditorDirectories::Get().SetLastDirectory(ELastDirectory::GENERIC_EXPORT, destinationFolder);
        FPaths::NormalizeFilename(destinationFolder);
        if ( !destinationFolder.EndsWith(TEXT("/")) )
        {
            destinationFolder += TEXT("/");
        }
    }
    else
    {
        // Not on a platform that supports desktop functionality
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("NoDesktopPlatform", "Error: This platform does not support a file dialog."));
        return;
    }

    // Form a full list of packages to move by including the dependencies of the supplied packages
    TSet<FName> allPackageNamesToMove;
    {
        FScopedSlowTask slowTask( iPackageNamesToExport.Num(), LOCTEXT( "ExportFolders_GatheringDependencies", "Gathering Dependencies..." ) );
        slowTask.MakeDialog();

        for ( auto packageIt = iPackageNamesToExport.CreateConstIterator(); packageIt; ++packageIt )
        {
            slowTask.EnterProgressFrame();

            if ( !allPackageNamesToMove.Contains(*packageIt) )
            {
                allPackageNamesToMove.Add(*packageIt);
                FString path = (*packageIt).ToString();
                FString originalRootString;
                path.RemoveFromStart(TEXT("/"));
                path.Split("/", &originalRootString, &path, ESearchCase::IgnoreCase, ESearchDir::FromStart);
                originalRootString = TEXT("/") + originalRootString;
                RecursiveGetDependencies(*packageIt, allPackageNamesToMove, originalRootString);
            }
        }
    }

    // Confirm that there is at least one package to move 
    if ( allPackageNamesToMove.Num() == 0 )
    {
        FMessageDialog::Open(EAppMsgType::Ok, LOCTEXT("ExportFolder_NoFilesFound", "No files were found to export"));
        return;
    }

    // Prompt the user displaying all assets that are going to be exported
    {
        const FText reportMessage = LOCTEXT("ExportFolderReportTitle", "The following assets will be exported to your system.");
        TSharedPtr<TArray<ReportPackageData>> reportPackages = MakeShareable(new TArray<ReportPackageData>);
        for( auto packageIt = allPackageNamesToMove.CreateConstIterator(); packageIt; ++packageIt )
        {
            reportPackages.Get()->Add({ (*packageIt).ToString(), true });
        }
        SOdysseyPackageReportDialog::FOnReportConfirmed onReportConfirmed = SOdysseyPackageReportDialog::FOnReportConfirmed::CreateStatic(&FOdysseyExportFolderExtension::ExportFolder_ReportConfirmed, reportPackages, destinationFolder);
        SOdysseyPackageReportDialog::OpenPackageReportDialog(reportMessage, *reportPackages.Get(), onReportConfirmed);
    }
}

void FOdysseyExportFolderExtension::ExportFolder_ReportConfirmed( TEnumAsByte<EExportImageFormat> iExportImageFormat, TSharedPtr<TArray<ReportPackageData>> iPackageDataToExport, FString iDestinationFolder )
{
    // Convert the Uenum into Ulis Enum
    ::ULIS::eFileFormat ulisExportImageFormat = SOdysseyPackageReportDialog::GetUlisExportImageFormat( iExportImageFormat );

    // Check if destination directory is empty
    TArray<FString> foundFiles;
    IFileManager::Get().FindFilesRecursive( foundFiles, GetData( FPaths::GetPath( iDestinationFolder ) ), *FString( "*" ), true, true, false );
    if ( ( foundFiles.Num() > 0 ) == true )
    {
        EAppReturnType::Type answer;
        const FText notEmptyFolder = FText::Format( LOCTEXT("ExportFolder_NotEmpty", "The Folder {0} is not empty, continue anyway ?"), FText::FromString( iDestinationFolder ) );
        answer = FMessageDialog::Open( EAppMsgType::YesNo, notEmptyFolder );
        if ( answer == EAppReturnType::No )
        {
            // The user chose to cancel because the destination folder is not empty, Break out
            return;
        }
    }

    bool bUserCanceled = false;

    // Copy all specified assets and their dependencies to the destination folder
    {
        FScopedSlowTask slowTask( 1, LOCTEXT( "ExportFolder_CopyingFiles", "Copying Files..." ) );
        slowTask.MakeDialog();

        EAppReturnType::Type lastResponse = EAppReturnType::Yes;

        slowTask.EnterProgressFrame();
        {
            FScopedSlowTask loopProgress(iPackageDataToExport.Get()->Num());
            for ( auto packageDataIt = iPackageDataToExport.Get()->CreateConstIterator(); packageDataIt; ++packageDataIt)
            {
                // Avoid to export the unchecked packages
                loopProgress.EnterProgressFrame();
                if (!packageDataIt->bShouldExportPackage)
                {
                    continue;
                }
                const FString& packageName = packageDataIt->Name;
                FString srcFilename;
            
                // Check if the Content Browser file exists
                if (!FPackageName::DoesPackageExist(packageName, &srcFilename))
                {
                    const FText errorMessage = FText::Format(LOCTEXT("ExportFolder_PackageMissing", "{0} does not exist on disk."), FText::FromString(packageName));
                }
                // Check if the Content Browser file is not in the Game folder
                else if (srcFilename.Contains(FPaths::EngineContentDir()))
                {
                    const FString leafName = srcFilename.Replace(*FPaths::EngineContentDir(), TEXT("Engine/"));
                }
                // Check what is on the operating system
                else
                {
                    bool bFileOKToCopy = true;

                    FString destFilename = iDestinationFolder;
                    const char* ext = ::ULIS::kwImageFormat[ulisExportImageFormat];

                    FString subFolder;
                    if ( srcFilename.Split( TEXT("/Content/"), nullptr, &subFolder ) )
                    {
                        destFilename = FPaths::GetPath( destFilename + *subFolder ) + TEXT("/") + FPaths::GetBaseFilename( srcFilename ) + TEXT(".") + ext;

                        if ( IFileManager::Get().FileSize(*destFilename) > 0 )
                        {
                            // The destination file already exists! Ask the user what to do
                            EAppReturnType::Type response;
                            if ( lastResponse == EAppReturnType::YesAll || lastResponse == EAppReturnType::NoAll )
                            {
                                response = lastResponse;
                            }
                            else
                            {
                                const FText message = FText::Format( LOCTEXT("ExportFolder_AlreadyExists", "An asset already exists at location {0} would you like to overwrite it?"), FText::FromString(destFilename) );
                                response = FMessageDialog::Open( EAppMsgType::YesNoYesAllNoAllCancel, message );
                                if ( response == EAppReturnType::Cancel )
                                {
                                    // The user chose to cancel mid-operation, Break out
                                    bUserCanceled = true;
                                    break;
                                }
                                lastResponse = response;
                            }

                            const bool bWantOverwrite = response == EAppReturnType::Yes || response == EAppReturnType::YesAll;
                            if( !bWantOverwrite )
                            {
                                // User chose not to replace the package
                                bFileOKToCopy = false;
                            }
                        }
                    }
                    else
                    {
                        // Couldn't find Content folder in source path
                        bFileOKToCopy = false;
                    }
                    // Perform the export operation
                    if ( bFileOKToCopy )
                    {
                        UTexture2D* textureToSave = LoadObject<UTexture2D>( nullptr, GetData( packageName ), nullptr, ELoadFlags::LOAD_None, nullptr );
                        ExportFile( textureToSave, destFilename, ulisExportImageFormat );
                    }
                }
            }
        }
    }

    const FText exportFinished = FText::Format( LOCTEXT("ExportFolder_Finished", "The export operation is finished "), FText::FromString( iDestinationFolder ) );
    FMessageDialog::Open( EAppMsgType::Ok, exportFinished );
}

// TODO : check is the OdysseyBlock format is correct according to the export file format
void FOdysseyExportFolderExtension::ExportFile( UTexture2D* iCurrentTexture, FString iSystemPathNameExt, ::ULIS::eFileFormat iExportFormat )
{
    std::string stringSystemPathNameExt = TCHAR_TO_UTF8( *iSystemPathNameExt );
    // The OdysseyBLock is required to be used with Ulis export function
    FTexturePlatformData* platformData = iCurrentTexture->GetPlatformData();
    ::ULIS::FBlock* odysseyBlockToSave = new ::ULIS::FBlock( platformData->SizeX, platformData->SizeY, ULISFormatForTextureSourceFormat( iCurrentTexture->Source.GetFormat() ) );
    FOdysseyScopedTextureSettings settingsGuard = FOdysseyScopedTextureSettings::MakeUncompressedNoMipMaps( iCurrentTexture );
    CopyUTextureSourceDataIntoBlock( odysseyBlockToSave, iCurrentTexture );

    // ::ul3::SaveToFile doesn't recreate directories
    if ( !IFileManager::Get().DirectoryExists( GetData( FPaths::GetPath( iSystemPathNameExt ) ) ) )
        {
            IFileManager::Get().MakeDirectory( GetData( FPaths::GetPath( iSystemPathNameExt ) ), true );
        }

    // Exporting the actual image
    IULISLoaderModule& ULISModule = IULISLoaderModule::Get();
    ::ULIS::FContext& ctx = ULISModule.FindOrAddContext(odysseyBlockToSave->Format());


    bool canSaveDirectly = false;
    ::ULIS::FContext::SaveBlockToDiskMetrics(*odysseyBlockToSave, iExportFormat, &canSaveDirectly);
    if (canSaveDirectly)
    {
        ctx.SaveBlockToDisk(
            *odysseyBlockToSave
            , stringSystemPathNameExt
            , iExportFormat
            , 100
        );

        ctx.Finish();
    }
    else
    {
        ::ULIS::eFormat format = odysseyBlockToSave->Model() == ::ULIS::ColorModel_GREY ? ::ULIS::Format_GA8 : ::ULIS::Format_RGBA8;
        if (iExportFormat == ::ULIS::FileFormat_hdr)
        {
            format = ::ULIS::Format_RGBAF;
        }

        ::ULIS::FBlock blockProxy(odysseyBlockToSave->Width(), odysseyBlockToSave->Height(), format);

        ::ULIS::FEvent eventConvert;
        ctx.ConvertFormat(
            *odysseyBlockToSave
            , blockProxy
            , ULIS::FRectI::Auto
            , ULIS::FVec2I(0)
            , ULIS::FSchedulePolicy::CacheEfficient
            , 0
            , nullptr
            , &eventConvert
        );

        ctx.SaveBlockToDisk(
            blockProxy
            , stringSystemPathNameExt
            , iExportFormat
            , 100
        );

        ctx.Finish();
    }

    delete odysseyBlockToSave;
}

void FOdysseyExportFolderExtension::RecursiveGetDependencies(const FName& iPackageName, TSet<FName>& ioAllDependencies, const FString& iOriginalRoot)
{
    FAssetRegistryModule& assetRegistryModule = FModuleManager::Get().LoadModuleChecked<FAssetRegistryModule>(TEXT("AssetRegistry"));
    TArray<FName> dependencies;
    assetRegistryModule.Get().GetDependencies(iPackageName, dependencies);
    
    for ( auto dependsIt = dependencies.CreateConstIterator(); dependsIt; ++dependsIt )
    {
        if ( !ioAllDependencies.Contains(*dependsIt) )
        {
            const bool bIsEnginePackage = (*dependsIt).ToString().StartsWith(TEXT("/Engine"));
            const bool bIsScriptPackage = (*dependsIt).ToString().StartsWith(TEXT("/Script"));
            // Skip all packages whose root is different than the source package list root
            const bool bIsInSamePackage = (*dependsIt).ToString().StartsWith(iOriginalRoot);
            if ( !bIsEnginePackage && !bIsScriptPackage && bIsInSamePackage )
            {
                ioAllDependencies.Add(*dependsIt);
                FOdysseyExportFolderExtension::RecursiveGetDependencies(*dependsIt, ioAllDependencies, iOriginalRoot);
            }
        }
    }
}

#undef LOCTEXT_NAMESPACE

