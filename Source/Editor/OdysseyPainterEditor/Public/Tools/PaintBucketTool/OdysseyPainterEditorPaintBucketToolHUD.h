#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorPaintBucketToolHUD : public FOdysseyVectorHUD
{
    public:
        static const uint32 HANDLE_RADIUS      = 3;
        static const uint32 HANDLE_DISTANCE    = 40;
        static const uint32 PELLET_RADIUS      = 5;
        static const uint32 RADIAL_AREA_RADIUS = 40;

        static const uint32 PICK_NONE          = 0;
        static const uint32 PICK_HANDLE        = 1;
        static const uint32 PICK_BUCKET        = 2;
        static const uint32 PICK_RADIAL_HANDLE = 3;
        static const uint32 PICK_RADIAL_AREA   = 4;

        virtual ~FOdysseyPainterEditorPaintBucketToolHUD();
        FOdysseyPainterEditorPaintBucketToolHUD(  UOdysseyPainterEditorPaintBucketTool* iPaintBucketTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual void Load( FOdysseyVectorScene* iScene ) override;

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

    private:
        void RecursiveDrawObject( FOdysseyVectorObject* iObject
                                 , BLContext* iBLContext
                                 , BLRgba32 fgColor
                                 , BLRgba32 bgColor
                                 , BLRgba32 hcColor );

        FOdysseyVectorBucket* RecursivePickBucket( FOdysseyVectorObject* iObject
                                                 , double iWorldX
                                                 , double iWorldY );

        static void RecursivePickCycles( FOdysseyVectorObject* iObject
                                       , double iWorldX
                                       , double iWorldY
                                       , std::vector<FOdysseyVectorCycle*>& oPickedCycleArray );

        void DrawBucket( FOdysseyVectorBucket* iBucket
                       , BLContext* iBLContext
                       , BLRgba32 fgColor
                       , BLRgba32 bgColor
                       , BLRgba32 hcColor );
        ::ULIS::FVec2D GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        ::ULIS::FVec2D GetHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld );
        ::ULIS::FVec2D GetRadialPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        ::ULIS::FVec2D GetRadialHandlePosition( FOdysseyVectorBucket* iBucket, bool iWorld );

    private:
        UOdysseyPainterEditorPaintBucketTool* mPaintBucketTool;
        FOdysseyVectorCycle* mCycle;
};
