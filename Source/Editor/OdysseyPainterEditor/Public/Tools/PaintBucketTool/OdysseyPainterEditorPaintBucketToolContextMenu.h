// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditorContextMenu.h"
#include "OdysseyVector.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorPaintBucketToolContextMenu :
    public FOdysseyPainterEditorContextMenu
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorPaintBucketToolContextMenu();
    FOdysseyPainterEditorPaintBucketToolContextMenu( FOdysseyPainterEditor* iEditor );

public:
    // FOdysseyEditorTab interface
    virtual TSharedPtr<SWidget> CreateWidget() override;
    virtual void BindShortcuts(FBaseToolkit* iToolkit) override;

private:
    FOdysseyVectorBucket* mWorkingBucket;
};
