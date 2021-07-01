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

    UE_LOG(LogTemp, Display, TEXT("%s"), *str );

    buffer << str;
    str = mGUI->GetColorSlidersTab()->ID().ToString();

    UE_LOG(LogTemp, Display, TEXT("%s"), *str);

    buffer << str;

    fileHandle->Seek(0);
    fileHandle->Write(buffer.GetData(), buffer.Num());
    fileHandle->Flush(true);

    delete fileHandle;

}

void FOdysseyViewportDrawingEditorModeToolbar::LoadOpenedTabs()
{
/*
    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *FString("IliadEdModeLayout.save"));
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    IFileHandle* fileHandle = platformFile.OpenRead(*tabsOpenedPath, true);

    FBufferArchive buffer;

    fileHandle->Seek(0);
    fileHandle->Read(buffer.GetData(), fileHandle->Size() );
    fileHandle->Flush(true);

    FString openedTabs;
    buffer << openedTabs;

    int i = 0;*/
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
