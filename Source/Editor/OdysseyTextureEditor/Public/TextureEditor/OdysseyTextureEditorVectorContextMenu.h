// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorVectorContextMenu.h"

class FOdysseyTextureEditor;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorVectorContextMenu :
	public FOdysseyPainterEditorVectorContextMenu
{
    public:
        // Construction / Destruction
        virtual ~FOdysseyTextureEditorVectorContextMenu();
        FOdysseyTextureEditorVectorContextMenu(FOdysseyTextureEditor* iEditor);

    protected:
        // FOdysseyTextureEditorVectorContextMenu interface
        virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
        //virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
};

