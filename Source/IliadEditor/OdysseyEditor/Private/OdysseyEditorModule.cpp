// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyEditorModule.h"

#include "Modules/ModuleManager.h"
#include "OdysseyClipboard.h"

IMPLEMENT_MODULE(FOdysseyEditorModule, OdysseyEditor );

void
FOdysseyEditorModule::StartupModule()
{
    mClipboard = MakeShared<FOdysseyClipboard>();
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
}

TSharedPtr<FOdysseyClipboard>
FOdysseyEditorModule::GetClipboard() const
{
    return mClipboard;
}
