// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "OdysseyPainterEditor.h"
#include "OdysseyPainterEditorGUI.h"
#include "OdysseyVector.h"

class FOdysseyPainterEditor;
class UOdysseyPainterEditorTool;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorPaintBucketToolContextMenu
{
    public:
        // FOdysseyEditorTab interface
        static TSharedPtr<SWidget> CreateWidget( FOdysseyPainterEditorToolContext* iToolContext
                                               , FOdysseyVectorBucket* iBucket );
        static void BucketProperties( FOdysseyPainterEditorToolContext* iToolContext
                                    , FOdysseyVectorBucket* iBucket );
};
