// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyContentBrowserExtensionsModule.h"
#include "OdysseyExportFolderExtension.h"
#include "ContentBrowserModule.h"
#include "Modules/ModuleManager.h"

#define LOCTEXT_NAMESPACE "OdysseyContentBrowserExtensionsModule"

/*-----------------------------------------------------------------------------
   FOdysseyContentBrowserExtensionsModule
-----------------------------------------------------------------------------*/

void
FOdysseyContentBrowserExtensionsModule::StartupModule()
{
    FContentBrowserModule& contentBrowserModule = FModuleManager::LoadModuleChecked<FContentBrowserModule>( TEXT("ContentBrowser") );
    FOdysseyExportFolderExtension::Register( contentBrowserModule );
}

void
FOdysseyContentBrowserExtensionsModule::ShutdownModule()
{

}

IMPLEMENT_MODULE( FOdysseyContentBrowserExtensionsModule, OdysseyContentBrowserExtensions );

#undef LOCTEXT_NAMESPACE

