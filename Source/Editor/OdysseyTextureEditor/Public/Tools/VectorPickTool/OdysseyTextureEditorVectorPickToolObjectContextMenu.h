// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/VectorPickTool/OdysseyPainterEditorVectorPickToolObjectContextMenu.h"

class FOdysseyTextureEditor;

class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorVectorPickToolObjectContextMenu :
	public FOdysseyPainterEditorVectorPickToolObjectContextMenu
{
    public:
        // Construction / Destruction
        virtual ~FOdysseyTextureEditorVectorPickToolObjectContextMenu();
        FOdysseyTextureEditorVectorPickToolObjectContextMenu( FOdysseyTextureEditor* iEditor );

    protected:
        // FOdysseyTextureEditorVectorPickToolObjectContextMenu interface
        virtual void BindShortcuts(FBaseToolkit* iToolkit) override;
        //virtual void ExtendMenu( FToolMenuOwner iOwner, FName iMenuName ) override;
        //virtual void OnToolkitInitialized( FBaseToolkit* iToolkit ) override;
};

