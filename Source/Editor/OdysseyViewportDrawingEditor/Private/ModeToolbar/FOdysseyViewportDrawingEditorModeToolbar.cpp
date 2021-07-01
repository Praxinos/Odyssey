// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "FOdysseyViewportDrawingEditorModeToolbar.h"
#include "OdysseyViewportDrawingEditorGUI.h"

#include "LevelEditor.h" 
#include "Serialization/BufferArchive.h"
#include "Serialization/MemoryReader.h"
#include "Misc/FileHelper.h"

#define LOCTEXT_NAMESPACE "OdysseyViewportDrawingEditorModeToolbar"


/////////////////////////////////////////////////////
// FOdysseyViewportDrawingEditorModeToolbar
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyViewportDrawingEditorModeToolbar::FOdysseyViewportDrawingEditorModeToolbar( FOdysseyViewportDrawingEditorGUI* iGUI )
    : mGUI( iGUI )
{
    FLevelEditorModule& levelEditorModule = FModuleManager::GetModuleChecked<FLevelEditorModule>(FName("LevelEditor"));
    mLevelEditorTabManager = levelEditorModule.GetLevelEditorTabManager();
}


FOdysseyViewportDrawingEditorModeToolbar::~FOdysseyViewportDrawingEditorModeToolbar()
{
    TSharedPtr< SDockTab > tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetLayerStackTab()->ID()));

    if (tab.IsValid())
        tab->RequestCloseTab();
}

void FOdysseyViewportDrawingEditorModeToolbar::SaveOpenedTabs()
{

    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *FString("IliadEdModeLayout.save"));
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenWrite(*tabsOpenedPath, true);

    FBufferArchive buffer;

    FString str = mGUI->GetLayerStackTab()->ID().ToString();
    buffer << str;

    str = mGUI->GetColorSlidersTab()->ID().ToString();
    buffer << str;

    fileHandle->Seek(0);
    fileHandle->Write(buffer.GetData(), buffer.Num());

    fileHandle->Flush(true);
    delete fileHandle;

}

void FOdysseyViewportDrawingEditorModeToolbar::LoadOpenedTabs()
{
    if( !mLevelEditorTabManager )
        return;

    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *FString("IliadEdModeLayout.save"));
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*tabsOpenedPath, true);

    FBufferArchive buffer;
    buffer.SetNum( fileHandle->Size() );

    fileHandle->Seek(0);
    bool succeed = fileHandle->Read(buffer.GetData(), fileHandle->Size() );

    //FString openedTabs;
    TArray<uint8> openedTabs;
    FFileHelper::LoadFileToArray( openedTabs, *tabsOpenedPath );
    FString openedTabsString;
    int start = 0;
    for( int i = 0; i < openedTabs.Num(); i++)
    {
        if( openedTabs[i] == 0 )
        {
            start = i;
            openedTabsString = FString( (char*) openedTabs.GetData() + start );
            mLevelEditorTabManager->TryInvokeTab( FTabId( FName( openedTabsString ) ) );
        }
    }
    //fileHandle->Read( (uint8*) *openedTabs, fileHandle->Size() );
    // UE_LOG(LogTemp, Display, TEXT("%s"), *openedTabs );

    fileHandle->Flush(true);
    delete fileHandle;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------- Getter / Setter


//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Callbacks / Toogle


void FOdysseyViewportDrawingEditorModeToolbar::ToggleLayerStackTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetLayerStackTab()->ID()));
}

#undef LOCTEXT_NAMESPACE
