#pragma once

#include <blend2d.h>
#include <ULIS>
#include "HUD/OdysseyVectorHUD.h"
#include "Tools/PaintBucketTool/OdysseyPainterEditorPaintBucketTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorPaintBucketToolHUD : public FOdysseyVectorHUD
{
    public:
        static const uint32 HANDLE_RADIUS   = 3;
        static const uint32 HANDLE_DISTANCE = 40;
        static const uint32 PELLET_RADIUS   = 5;

        static const uint32 PICK_NONE   = 0;
        static const uint32 PICK_HANDLE = 1;
        static const uint32 PICK_BUCKET = 2;

        virtual ~FOdysseyPainterEditorPaintBucketToolHUD();
        FOdysseyPainterEditorPaintBucketToolHUD(  UOdysseyPainterEditorPaintBucketTool* iPaintBucketTool );

        virtual void Draw( FOdysseyVectorScene* iScene, uint64 iFlags ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        void SetCycle( FOdysseyVectorCycle* iCycle );
        uint32 PickBucketArea( FOdysseyVectorBucket* iBucket, double iWorldX, double iWorldY );
        FOdysseyVectorBucket* PickBucket( FOdysseyVectorGroupPaint* paintGroup
                                        , double iWorldX
                                        , double iWorldY );
        FOdysseyVectorCycle* PickCycle( FOdysseyVectorGroupPaint* paintGroup
                                        , double iWorldX
                                        , double iWorldY );

    private:
        void DrawBucket( FOdysseyVectorBucket* iBucket
                       , BLContext* iBLContext
                       , BLRgba32 fgColor
                       , BLRgba32 bgColor
                       , BLRgba32 hcColor );
        ::ULIS::FVec2D GetBucketPosition( FOdysseyVectorBucket* iBucket, bool iWorld );
        ::ULIS::FVec2D GetHandleVector( FOdysseyVectorBucket* iBucket, bool iWorld );

    private:
        UOdysseyPainterEditorPaintBucketTool* mPaintBucketTool;
        FOdysseyVectorCycle* mCycle;
};
