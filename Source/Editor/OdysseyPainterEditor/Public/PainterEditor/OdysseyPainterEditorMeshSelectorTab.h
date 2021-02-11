// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPainterEditorTab.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorMeshSelectorTab :
	public FOdysseyPainterEditorTab
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorMeshSelectorTab();
    FOdysseyPainterEditorMeshSelectorTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyPainterEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;

public:
    // Public Getters
    TSharedPtr<SOdysseyMeshSelector> MeshSelector();

private:
    // Event Listeners
    void OnMeshSelected( UStaticMesh* iMesh );

private:
    FOdysseyPainterEditor* mEditor;

    TSharedPtr<SOdysseyMeshSelector> mMeshSelector;
};

