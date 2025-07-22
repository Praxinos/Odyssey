// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
        static const uint32 PICK_NONE           = 0;
        static const uint32 PICK_BUCKET         = 2;
        static const uint32 PICK_RADIAL_HANDLE  = 3;
        static const uint32 PICK_RADIAL_AREA    = 4;
        static const uint32 PICK_LINEAR_HANDLE0 = 5;
        static const uint32 PICK_LINEAR_HANDLE1 = 6;
        static const uint32 PICK_LINEAR_AREA    = 7;
        static const uint32 PICK_PROPAGATE      = 8;

        virtual ~FOdysseyPainterEditorVectorPaintBucketToolHUD();
        FOdysseyPainterEditorVectorPaintBucketToolHUD(  UOdysseyPainterEditorVectorPaintBucketTool* iVectorPaintBucketTool );

        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset() override;
        virtual void Load() override;
        virtual void Unload() override;
        void UpdateWorkingPaintgroupList();

        void DrawHUD( const FOdysseyHUD::FDrawHUDParams& iParams );

        void SetCycle( FOdysseyVectorCycle* iCycle );

        FOdysseyVectorBucket* PickBucket( double iWorldX
                                        , double iWorldY );

        void PickCycles( double iWorldX
                       , double iWorldY
                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

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
};
