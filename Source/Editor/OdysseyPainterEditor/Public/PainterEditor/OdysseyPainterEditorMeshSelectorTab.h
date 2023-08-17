// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorTab.h"
#include "Mesh/SOdysseyMeshSelector.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorMeshSelectorTab :
	public FOdysseyEditorTab
{
public:
    static const FName& StaticId();

public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorMeshSelectorTab();
    FOdysseyPainterEditorMeshSelectorTab(FOdysseyPainterEditor* iEditor);

protected:
    // FOdysseyEditorTab interface
    virtual const FName& GetId() const override;
    virtual TSharedPtr<SWidget> CreateWidget() override;

protected:
    // Event Listeners
    virtual void OnMeshSelected( UStaticMesh* iMesh );

private:
    FOdysseyPainterEditor* mEditor;
};

