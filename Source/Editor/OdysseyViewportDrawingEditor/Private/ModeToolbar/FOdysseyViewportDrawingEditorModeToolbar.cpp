// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
    TSharedPtr< SDockTab > tab = nullptr;

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetLayerStackTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetColorSlidersTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetBrushSelectorTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetColorWheelTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetMeshSelectorTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetToolOptionsTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetTextureDetailsTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetToolsTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetTopTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetViewportTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();

    tab = mLevelEditorTabManager->FindExistingLiveTab(FTabId(mGUI->GetHUDTab()->ID()));
    if (tab.IsValid())
        tab->RequestCloseTab();
}

void FOdysseyViewportDrawingEditorModeToolbar::SaveOpenedTabs()
{
    FString tabsOpenedPath = FPaths::Combine(FPaths::EngineSavedDir(), *FString("IliadEdModeLayout.save"));
    IPlatformFile& platformFile = FPlatformFileManager::Get().GetPlatformFile();
    FPlatformFileManager::Get().GetPlatformFile().DeleteFile(*tabsOpenedPath);
    IFileHandle* fileHandle = platformFile.OpenWrite(*tabsOpenedPath);

    if( !fileHandle )
        return;

    FBufferArchive buffer;
    FString str;

    if( mLevelEditorTabManager->FindExistingLiveTab( mGUI->GetLayerStackTab()->ID() ) )
    {
        str = mGUI->GetLayerStackTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetColorSlidersTab()->ID() ) )
    {
        str = mGUI->GetColorSlidersTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetBrushSelectorTab()->ID() ) )
    {
        str = mGUI->GetBrushSelectorTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetColorWheelTab()->ID() ) )
    {
        str = mGUI->GetColorWheelTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetMeshSelectorTab()->ID() ) )
    {
        str = mGUI->GetMeshSelectorTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetToolOptionsTab()->ID() ) )
    {
        str = mGUI->GetToolOptionsTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetTextureDetailsTab()->ID() ) )
    {
        str = mGUI->GetTextureDetailsTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetToolsTab()->ID() ) )
    {
        str = mGUI->GetToolsTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetTopTab()->ID() ) )
    {
        str = mGUI->GetTopTab()->ID().ToString();
        buffer << str;
    }

    if( mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetViewportTab()->ID() ) )
    {
        str = mGUI->GetViewportTab()->ID().ToString();
        buffer << str;
    }

    if (mLevelEditorTabManager->FindExistingLiveTab(mGUI->GetHUDTab()->ID()))
    {
        str = mGUI->GetHUDTab()->ID().ToString();
        buffer << str;
    }

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

    if( !fileHandle )
        return;

    FBufferArchive buffer;
    buffer.SetNum( fileHandle->Size() );

    fileHandle->Seek(0);
    fileHandle->Read(buffer.GetData(), fileHandle->Size() );

    TArray<FString> tabNames;
    int start = 4;
    while (start < fileHandle->Size() )
    {
        tabNames.Add( FString( (char*) buffer.GetData() + start ) );
        start += tabNames[tabNames.Num() - 1].Len() + 5;
    }

    for (int i = 0; i < tabNames.Num(); i++)
    {
        mLevelEditorTabManager->TryInvokeTab(FTabId(FName(tabNames[i])));
    }

    fileHandle->Flush(true);
    delete fileHandle;
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------------------------- Callbacks

void FOdysseyViewportDrawingEditorModeToolbar::OpenLayerStackTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetLayerStackTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenColorSlidersTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetColorSlidersTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenBrushSelectorTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetBrushSelectorTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenColorWheelTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetColorWheelTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenMeshSelectorTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetMeshSelectorTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenToolOptionsTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetToolOptionsTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenTextureDetailsTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetTextureDetailsTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenToolsTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetToolsTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenTopTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetTopTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenViewportTab()
{
    if( !mLevelEditorTabManager )
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetViewportTab()->ID()));
}

void FOdysseyViewportDrawingEditorModeToolbar::OpenHUDTab()
{
    if (!mLevelEditorTabManager)
        return;

    mLevelEditorTabManager->TryInvokeTab(FTabId(mGUI->GetHUDTab()->ID()));
}

#undef LOCTEXT_NAMESPACE

