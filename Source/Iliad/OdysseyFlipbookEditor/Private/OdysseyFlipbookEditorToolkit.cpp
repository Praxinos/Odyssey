// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "OdysseyFlipbookEditorToolkit.h"

#include "IOdysseyFlipbookEditorModule.h"
#include "PaperFlipbook.h"

#define LOCTEXT_NAMESPACE "FlipbookEditor"

/////////////////////////////////////////////////////
// FOdysseyFlipbookEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyFlipbookEditorToolkit::~FOdysseyFlipbookEditorToolkit()
{
}

FOdysseyFlipbookEditorToolkit::FOdysseyFlipbookEditorToolkit() :
    FOdysseyAssetEditorToolkit( TEXT("OdysseyFlipbookEditorApp"))
{
}

void
FOdysseyFlipbookEditorToolkit::OpenAsset(UObject* iObject)
{
    UPaperFlipbook* flipbook = Cast<UPaperFlipbook>(iObject);
    IOdysseyFlipbookEditorModule* odysseyFlipbookEditorModule = &FModuleManager::GetModuleChecked<IOdysseyFlipbookEditorModule>("OdysseyFlipbookEditor");
    odysseyFlipbookEditorModule->CreateOdysseyFlipbookEditor( { flipbook } );
}

FText
FOdysseyFlipbookEditorToolkit::GetBaseToolkitName() const
{
    return LOCTEXT( "editor.name", "Odyssey Flipbook Editor" );
}

FName
FOdysseyFlipbookEditorToolkit::GetToolkitFName() const
{
    return FName( "OdysseyFlipbookEditor" );
}

FString
FOdysseyFlipbookEditorToolkit::GetWorldCentricTabPrefix() const
{
    return LOCTEXT( "editor.world-centric-tab-prefix", "Flipbook " ).ToString();
}

#undef LOCTEXT_NAMESPACE
