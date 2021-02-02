// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorController.h"
#include "OdysseyPainterEditorData.h"
#include "OdysseyPainterEditorGUI.h"

/**
 * Implements an Editor toolkit for the Painter Editor.
 * The toolkit is the main entry point for the Painter Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorToolkit
    : public FAssetEditorToolkit
    , public FEditorUndoClient
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorToolkit();
    FOdysseyPainterEditorToolkit();

public:
    //void InitPainterEditorToolkit( const EToolkitMode::Type iMode, const TSharedPtr< class IToolkitHost >& iInitToolkitHost, const FName& iAppIdentifier, TArray<UObject*>& iObjectsToEdit);
    void InitPainterEditorToolkit(const FName& iAppIdentifier, TArray<UObject*>& iObjectsToEdit); //TODO: Rename to Init() if possible

protected:
    // FAssetEditorToolkit interface
    virtual void RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) override;

protected:
    // FEditorUndoClient interface
    virtual void PostUndo( bool iSuccess ) override;
    virtual void PostRedo( bool iSuccess ) override;

protected:
    virtual const TSharedRef<FTabManager::FLayout>& GetLayout() const = 0;
    virtual const TArray<TSharedPtr<FExtender>>& GetMenuExtenders() const = 0;

private:
	void InitializeExtenders();
};

