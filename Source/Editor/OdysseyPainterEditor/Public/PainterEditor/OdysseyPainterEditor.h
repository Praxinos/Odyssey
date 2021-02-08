// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

class FOdysseyPainterEditorToolkit;

/**
 * Base class for a Painting Editor
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditor
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditor();
    FOdysseyPainterEditor(TSharedPtr<FOdysseyPainterEditorToolkit> iToolkit);

public:
    virtual void Init();

public:
    virtual TSharedPtr<FOdysseyPainterEditorToolkit> GetToolkit();
    virtual void OnToolkitInitialized();

public:
    virtual bool OnCloseRequested() = 0;
    virtual const TSharedRef<FTabManager::FLayout>& CreateLayout() const = 0;
    virtual const TArray<TSharedPtr<FExtender>>& CreateMenuExtenders() const = 0;
    virtual TSharedPtr<FWorkspaceItem> RegisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;
    virtual void UnregisterTabSpawners( const TSharedRef<class FTabManager>& iTabManager ) = 0;

private:
    TWeakPtr<FOdysseyPainterEditorToolkit> mToolkit;
};
