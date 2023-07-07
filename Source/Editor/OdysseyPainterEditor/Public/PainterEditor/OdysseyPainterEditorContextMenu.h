// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyEditorContextMenu.h"

class FOdysseyPainterEditor;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorContextMenu :
    public FOdysseyEditorContextMenu
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorContextMenu();
    FOdysseyPainterEditorContextMenu( FOdysseyPainterEditor* iEditor
                                    , FName iID
                                    , FText iDisplayName
                                    , FSlateIcon iIcon );
    FOdysseyPainterEditor* GetEditor() const { return mEditor; };
    template<class T> T* GetEditorAs() const { return static_cast<T*>(mEditor); };

private:
    FOdysseyPainterEditor* mEditor;
};
