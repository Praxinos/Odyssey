// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorPaintBucketTool/OdysseyPainterEditorVectorPaintBucketTool.h"

class FOdysseyVectorCycle;
class FOdysseyVectorBucket;
class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorPaintBucketToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        virtual ~FOdysseyPainterEditorVectorPaintBucketToolHUD();
        FOdysseyPainterEditorVectorPaintBucketToolHUD(  UOdysseyPainterEditorVectorPaintBucketTool* iVectorPaintBucketTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Reset( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Load( FOdysseyVectorGroupPaint* iScene ) override;
        virtual void Unload( FOdysseyVectorGroupPaint* iScene ) override;
        void UpdateWorkingPaintgroupList( FOdysseyVectorGroupPaint* iScene );

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
        std::list<FOdysseyVectorGroupPaint*>& GetWorkingPaintgroupList();

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
        std::list<FOdysseyVectorGroupPaint*> mWorkingPaintgroupList;
        ::ULIS::FVec2D mCursorAt;
};
