// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyEditorModule.h"
#include "Modules/ModuleManager.h"
#include "OdysseyClipboard.h"

IMPLEMENT_MODULE(FOdysseyEditorModule, OdysseyEditor );

void
FOdysseyEditorModule::StartupModule()
{
    mClipboard = MakeShared<FOdysseyClipboard>();
    FCoreDelegates::OnEnginePreExit.AddRaw(this, &FOdysseyEditorModule::OnEnginePreExit);
}

void
FOdysseyEditorModule::OnEnginePreExit()
{
    //Clearing the clipboard in OnEnginePreExit instead of ShutdownModule()
    //Avoids a crash caused by IBulkDataRegistry::Shutdown() being called before ShutdownModule()
    //Which leads to rasterblocks contained in the clipboard to not being able to retrieve IBulkDataRegistry on destruction
    mClipboard = nullptr;
}

void
FOdysseyEditorModule::ShutdownModule()
{
    FCoreDelegates::OnEnginePreExit.RemoveAll(this);
	for (const auto& element : mOpenedTabIds)
    {
        const FName& editorName = element.Key;
        SaveOpenedTabIds(editorName);
    }
}

TSharedPtr<FOdysseyClipboard>
FOdysseyEditorModule::GetClipboard() const
{
    return mClipboard;
}

void
FOdysseyEditorModule::SetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iTabIds)
{
	TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);
    tabIds = iTabIds;
}

const TArray<FName>&
FOdysseyEditorModule::GetOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds)
{
    if (!mOpenedTabIds.Contains(iEditorName))
        LoadOpenedTabIds(iEditorName, iDefaultOpenedTabIds);

	return mOpenedTabIds[iEditorName];
}

FString
FOdysseyEditorModule::GetOpenedTabIdsSavedPath() const
{
    FString filename = FApp::GetProjectName() + FString("OdysseyLayout.ini");
	return FPaths::Combine(FPlatformProcess::UserSettingsDir(), FApp::GetEpicProductIdentifier(), TEXT("Editor"), TEXT("Iliad"), filename);
}

FString
FOdysseyEditorModule::GetOpenedTabIdsProjectPath() const
{
    FString filename = "OdysseyLayout.ini";
    return FPaths::Combine(FPaths::ProjectConfigDir(), filename);
}

void
FOdysseyEditorModule::LoadOpenedTabIds(const FName& iEditorName, const TArray<FName>& iDefaultOpenedTabIds)
{    
	FString savedPath = GetOpenedTabIdsSavedPath();
    FString projectPath = GetOpenedTabIdsProjectPath();

    TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);

    FConfigFile* configFile = GConfig->Find(savedPath);
    if ( !configFile || !configFile->Contains(iEditorName.ToString()) )
    {
        configFile = GConfig->Find(projectPath);

        if ( !configFile || !configFile->Contains(iEditorName.ToString()) )
        {
            tabIds = iDefaultOpenedTabIds;
            return;
        }
    }

    TArray<FString> tabStringIds;
    configFile->GetArray(
        *iEditorName.ToString(),
        TEXT("OpenedTabs"),
        tabStringIds);

    tabIds.Empty();
    for ( const FString& tabId : tabStringIds )
    {
        tabIds.Add(FName(tabId));
    }


    /* IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*tabsOpenedPath, true);

    if( !fileHandle )
    {
        tabIds = iDefaultOpenedTabIds;
		return;
    }

    FBufferArchive buffer;
    buffer.SetNum( fileHandle->Size() );

    fileHandle->Seek(0);
    fileHandle->Read(buffer.GetData(), fileHandle->Size() );
    fileHandle->Flush(true);

    FMemoryReader bufferReader(buffer, false);
    bufferReader << tabIds;
    delete fileHandle; */
}

void
FOdysseyEditorModule::SaveOpenedTabIds(const FName& iEditorName)
{
    FString savedPath = GetOpenedTabIdsSavedPath();
    TArray<FName>& tabIds = mOpenedTabIds.FindOrAdd(iEditorName);

    TArray<FString> tabStringIds;
    for ( const FName& tabId : tabIds )
    {
        tabStringIds.Add(tabId.ToString());
    }

    GConfig->SetArray(
        *iEditorName.ToString(),
        TEXT("OpenedTabs"),
        tabStringIds,
        savedPath);

    /* IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*tabsOpenedPath);
    
    FString folderPath = FPaths::GetPath(tabsOpenedPath);
    if ( !platformFile.CreateDirectoryTree(*folderPath) )
        return;
            
    IFileHandle* fileHandle = platformFile.OpenWrite(*tabsOpenedPath);

    if( !fileHandle )
        return;

    FBufferArchive buffer;
    uint32 numTabs = tabIds.Num();
	buffer << tabIds;

    fileHandle->Seek(0);
    fileHandle->Write(buffer.GetData(), buffer.Num());
    fileHandle->Flush(true);
	
    delete fileHandle; */
}