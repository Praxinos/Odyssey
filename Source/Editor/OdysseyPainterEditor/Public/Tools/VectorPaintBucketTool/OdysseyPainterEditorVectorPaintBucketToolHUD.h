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

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;
        virtual void Unload( FOdysseyVectorScene* iScene ) override;

        void SetCycle( FOdysseyVectorCycle* iCycle );

        FOdysseyVectorBucket* PickBucket( FOdysseyVectorScene* iScene
                                        , double iWorldX
                                        , double iWorldY );

        void PickCycles( FOdysseyVectorScene* iScene
                       , double iWorldX
                       , double iWorldY
                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

        uint32 PickBucketArea( FOdysseyVectorBucket* iBucket
                             , double iWorldX
                             , double iWorldY );
        void SetPickedCycles( std::vector<FOdysseyVectorCycle*>& pickedCycleArray );

    private:
        FOdysseyVectorBucket* RecursivePickBucket( FOdysseyVectorObject* iObject
                                                 , double iWorldX
                                                 , double iWorldY );

        static void RecursivePickCycles( FOdysseyVectorObject* iObject
                                       , double iWorldX
                                       , double iWorldY
                                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

    private:
        std::vector<FOdysseyVectorCycle*> mPickedCycleArray;
        UOdysseyPainterEditorVectorPaintBucketTool* mPaintBucketTool;
        bool mAnyPaintGroupSelected;
};
