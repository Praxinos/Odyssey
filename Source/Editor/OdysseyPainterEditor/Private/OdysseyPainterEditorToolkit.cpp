// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyPainterEditorToolkit.h"

#define LOCTEXT_NAMESPACE "OdysseyPainterEditorToolkit"

/////////////////////////////////////////////////////
// FOdysseyPainterEditorToolkit
//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyPainterEditorToolkit::~FOdysseyPainterEditorToolkit()
{
    //GEditor->UnregisterForUndo(this);
}

FOdysseyPainterEditorToolkit::FOdysseyPainterEditorToolkit()
{
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Initialization
void
FOdysseyPainterEditorToolkit::InitPainterEditorToolkit( const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, TArray<UObject*>& iObjectsToEdit )
{
    FAssetEditorToolkit::InitAssetEditor( iMode, iInitToolkitHost, iAppIdentifier, GetLayout(), true, false, iObjectsToEdit);
    InitializeExtenders();
    RegenerateMenusAndToolbars();
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------- FAssetEditorToolkit interface
void
FOdysseyPainterEditorToolkit::RegisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
	FAssetEditorToolkit::RegisterTabSpawners(iTabManager);
}

void
FOdysseyPainterEditorToolkit::UnregisterTabSpawners(const TSharedRef<class FTabManager>& iTabManager)
{
	FAssetEditorToolkit::UnregisterTabSpawners(iTabManager);
}

//--------------------------------------------------------------------------------------
//---------------------------------------------------------- FEditorUndoClient interface
void
FOdysseyPainterEditorToolkit::PostUndo( bool iSuccess )
{
}

void
FOdysseyPainterEditorToolkit::PostRedo( bool iSuccess )
{
    PostUndo( iSuccess );
}

//--------------------------------------------------------------------------------------
//-------------------------------------------------------------------- Commands building

void
FOdysseyPainterEditorToolkit::InitializeExtenders()
{
    TArray<TSharedPtr<FExtender>> extenders = GetMenuExtenders();
    // Create the Extender that will add content to the menu
    for( int i = 0; i < extenders.Num(); i++)
    {
        AddMenuExtender(extenders[i]);
    }
}


//--------------------------------------------------------------------------------------
//---------------------------------------- Transaction ( Undo / Redo ) methods overrides

/*
void
FOdysseyPainterEditorToolkit::BeginTransaction( const FText& iSessionName )
{
}

void
FOdysseyPainterEditorToolkit::MarkTransactionAsDirty()
{
}

void
FOdysseyPainterEditorToolkit::EndTransaction()
{
}
*/

#undef LOCTEXT_NAMESPACE