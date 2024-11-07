#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPaintBucketToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPaintBucketToolHUD();
        FOdysseyPainterEditorVectorPaintBucketToolHUD(  UOdysseyPainterEditorVectorPaintBucketTool* iVectorPaintBucketTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;

        void SetCycle( FOdysseyVectorCycle* iCycle );

        FOdysseyVectorBucket* PickBucket( FOdysseyVectorGroupPaint* iScene
                                        , double iWorldX
                                        , double iWorldY );

        void PickCycles( FOdysseyVectorGroupPaint* iScene
                       , double iWorldX
                       , double iWorldY
                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

        void SetCursorPosition( double iWorldX, double iWorldY );

        uint32 PickBucketArea( FOdysseyVectorBucket* iBucket
                             , double iWorldX
                             , double iWorldY );

    private:
        FOdysseyVectorBucket* RecursivePickBucket( FOdysseyVectorObject* iObject
                                                 , double iWorldX
                                                 , double iWorldY );

        static void RecursivePickCycles( FOdysseyVectorObject* iObject
                                       , double iWorldX
                                       , double iWorldY
                                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

    private:
        UOdysseyPainterEditorVectorPaintBucketTool* mPaintBucketTool;
        bool mAnyPaintGroupSelected;
        ::ULIS::FVec2D mCursorAt;
};
