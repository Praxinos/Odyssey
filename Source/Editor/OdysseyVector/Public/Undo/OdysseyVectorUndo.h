#pragma once

#include "CoreMinimal.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorBrush.h"
#include "OdysseyVectorPath.h"

class FOdysseyVectorPoint;
class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorVertex;
class FOdysseyVectorSegmentCubic;

class ODYSSEYVECTOR_API FSnapshotPoint
{
    public:
        static const uint32 SNAPSHOT_POSITION = ( 1 << 0 );
        static const uint32 SNAPSHOT_RADIUS   = ( 1 << 1 );
        static const uint32 SNAPSHOT_ALL      = 0xFFFFFFFF;

        virtual ~FSnapshotPoint();
        FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint32 iPointSnapshotFlags );

        virtual void Restore();

    protected:
        uint32 mPointSnapshotFlags;
        FOdysseyVectorPoint* mPoint;
        ULIS::FVec2D mCoords;
        double mRadius;
};

class ODYSSEYVECTOR_API FSnapshotVertex : public FSnapshotPoint
{
    public:
        static const uint32 SNAPSHOT_ALIGNMENT = ( 1 << 0 );
        static const uint32 SNAPSHOT_ALL       = 0xFFFFFFFF;

        virtual ~FSnapshotVertex();
        FSnapshotVertex( FOdysseyVectorVertex* iVertex
                       , uint32 iPointSnapshotFlags
                       , uint32 iVertexSnapshotFlags );

        virtual void Restore() override;

    protected:
        uint32 mVertexSnapshotFlags;
        bool mAlignment;
};

class ODYSSEYVECTOR_API FSnapshotSegmentCubic
{
    public:
        static const uint32 SNAPSHOT_HANDLES = ( 1 << 0 );
        static const uint32 SNAPSHOT_ALL     = 0xFFFFFFFF;

        ~FSnapshotSegmentCubic();
        FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment, uint32 iCubicSegmentSnapshotFlags );

        void Restore();

        FOdysseyVectorSegmentCubic* GetCubicSegment();

    private:
        uint32 mCubicSegmentSnapshotFlags;
        FOdysseyVectorSegmentCubic* mCubicSegment;
        ULIS::FVec2D mHandleCoords[2];
};

class ODYSSEYVECTOR_API FSnapshotBucket
{
    public:
        static const uint32 SNAPSHOT_POSITION = ( 1 << 0 );
        static const uint32 SNAPSHOT_PARAM    = ( 1 << 1 );
        static const uint32 SNAPSHOT_ALL      = 0xFFFFFFFF;

        ~FSnapshotBucket();
        FSnapshotBucket(){};
        FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint32 iBucketSnapshotFlags );

        void Restore();

    private:
        uint32 mBucketSnapshotFlags;
        FOdysseyVectorBucket* mBucket;
        ULIS::FVec2D mCoords;
        FBucketParam mBucketParam;
};

namespace FSnapshotFlags
{
    namespace Object
    {
        static const uint64 TRANSFORMATIONS           = ( 1ULL <<  0 );
        static const uint64 COLORING                  = ( 1ULL <<  1 );
        static const uint64 OPACITY                   = ( 1ULL <<  2 );
        static const uint64 NAME                      = ( 1ULL <<  3 );
        static const uint64 HIERARCHY                 = ( 1ULL <<  4 );
        static const uint64 CHILDREN_TRANSFORMATIONS  = ( 1ULL <<  5 );
        static const uint64 PARAM                     = ( TRANSFORMATIONS
                                                        | COLORING
                                                        | OPACITY
                                                        | NAME );
        namespace Path
        {
            static const uint64 VERTICES                  = ( 1ULL <<  6 );
            static const uint64 SEGMENTS                  = ( 1ULL <<  7 );
            static const uint64 SELECTED_VERTICES         = ( 1ULL <<  8 );
            static const uint64 TOPOLOGY                  = ( 1ULL <<  9 );
            static const uint64 BRUSH                     = ( 1ULL << 10 );
            static const uint64 JOINTTYPE                 = ( 1ULL << 11 );
            static const uint64 MITERLIMIT                = ( 1ULL << 12 );
            static const uint64 PARAM                     = ( BRUSH
                                                            | JOINTTYPE
                                                            | MITERLIMIT );
        }
        namespace GroupPaint
        {
            static const uint64 BUCKETS                   = ( 1ULL <<  6 );
            static const uint64 SELECTED_BUCKETS          = ( 1ULL <<  7 );
            static const uint64 PAINTED                   = ( 1ULL <<  8 );
            static const uint64 MONOCHROME                = ( 1ULL <<  9 );
            static const uint64 MONOCHROMECOLOR           = ( 1ULL << 10 );
            static const uint64 REALTIME                  = ( 1ULL << 11 );
            static const uint64 GAPTOLERANCE              = ( 1ULL << 12 );
            static const uint64 WIREFRAME                 = ( 1ULL << 13 );
            static const uint64 WIREFRAMECOLOR            = ( 1ULL << 14 );
            static const uint64 PARAM                     = ( PAINTED
                                                            | MONOCHROME
                                                            | MONOCHROMECOLOR
                                                            | REALTIME
                                                            | GAPTOLERANCE
                                                            | WIREFRAME
                                                            | WIREFRAMECOLOR );
        }
        static const uint64 ALL                      = 0xFFFFFFFFFFFFFFFFULL;
    }
}

class ODYSSEYVECTOR_API FSnapshotObject
{
    public:
        virtual ~FSnapshotObject();
        FSnapshotObject( FOdysseyVectorObject* iObject, uint64 iSnapshotFlags );

        virtual bool Restore();

    protected:
        FSnapshotBucket mForegroundBucketSnapshot;
        FSnapshotBucket mBackgroundBucketSnapshot;
        uint64 mSnapshotFlags;
        FOdysseyVectorObject* mObject;
        FOdysseyVectorObject* mParent;
        FOdysseyVectorObject* mPreviousChild;
        std::vector<FSnapshotObject*> mChildrenSnapshotArray;
        double mTranslationX;
        double mTranslationY;
        double mRotation;
        double mScalingX;
        double mScalingY;
        FString mName;
        double mOpacity;
};

class ODYSSEYVECTOR_API FSnapshotPath : public FSnapshotObject
{
    public:
        virtual ~FSnapshotPath();
        FSnapshotPath( FOdysseyVectorPath* iPath, uint64 iSnapshotFlags );

        virtual bool Restore() override;

    private:
        std::vector<FSnapshotVertex> mVertexSnapshotArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
        std::list<FOdysseyVectorVertex*> mSelectedVertexList;
        std::list<FOdysseyVectorVertex*> mTopologyVertexList;
        std::list<FOdysseyVectorSegment*> mTopologySegmentList;
        FOdysseyVectorBrush mBrush;
        eJointType mJointType;
        double mMiterLimit;
};

class ODYSSEYVECTOR_API FSnapshotGroupPaint : public FSnapshotObject
{
    public:
        virtual ~FSnapshotGroupPaint();
        FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup, uint64 iSnapshotFlags );

        virtual bool Restore() override;

    private:
        std::vector<FSnapshotBucket> mBucketSnapshotArray;
        std::list<FOdysseyVectorBucket*> mSelectedBucketList;
        bool bPainted;
        bool bMonochrome;
        FColor mMonochromeColor;
        bool bRealtime;
        double mGapTolerance;
        bool bWireframe;
        FColor mWireframeColor;
};


class ODYSSEYVECTOR_API FOdysseyVectorUndo : public FCommandChange
{
    public:
        ~FOdysseyVectorUndo();
        FOdysseyVectorUndo( FOdysseyVectorGroupPaint* iScene );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        //virtual FString ToString() const override;

    protected:
        bool mApplied;
        FOdysseyVectorGroupPaint* mScene;
};
