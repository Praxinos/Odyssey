#pragma once

#include "CoreMinimal.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class FOdysseyVectorPoint;
class FOdysseyVectorScene;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorVertex;
class FOdysseyVectorSegmentCubic;

class ODYSSEYVECTOR_API FSnapshotPoint
{
    public:
        static const uint32 SNAPSHOT_POSITION = ( 1 << 0 );
        static const uint32 SNAPSHOT_RADIUS   = ( 1 << 1 );
        static const uint32 SNAPSHOT_ALL      = 0xFFFFFFFF;

        ~FSnapshotPoint();
        FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint32 iPointSnapshotFlags );

        virtual void Restore();

    private:
        uint32 mPointSnapshotFlags;
        FOdysseyVectorPoint* mPoint;
        ULIS::FVec2D mCoords;
        double mRadius;
};

class ODYSSEYVECTOR_API FSnapshotVertex : public FSnapshotPoint
{
    public:
        static const uint32 SNAPSHOT_FLAGS = ( 1 << 0 );
        static const uint32 SNAPSHOT_ALL   = 0xFFFFFFFF;

        ~FSnapshotVertex();
        FSnapshotVertex( FOdysseyVectorVertex* iVertex
                       , uint32 iPointSnapshotFlags
                       , uint32 iVertexSnapshotFlags );

        virtual void Restore() override;

    private:
        uint32 mVertexSnapshotFlags;
        uint32 mVertexFlags;
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
        FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint32 iBucketSnapshotFlags );

        void Restore();

    private:
        uint32 mBucketSnapshotFlags;
        FOdysseyVectorBucket* mBucket;
        ULIS::FVec2D mCoords;
        FBucketParam mBucketParam;
};

class ODYSSEYVECTOR_API FSnapshotObject
{
    public:
        static const uint32 SNAPSHOT_TRANSFORMATIONS          = ( 1 << 0 );
        static const uint32 SNAPSHOT_CHILDREN_TRANSFORMATIONS = ( 1 << 1 );
        static const uint32 SNAPSHOT_ALL                      = 0xFFFFFFFF;

        ~FSnapshotObject();
        FSnapshotObject( FOdysseyVectorObject* iObject, uint32 iObjectSnapshotFlags );

        virtual void Restore();

    protected:
        uint32 mObjectSnapshotFlags;
        FOdysseyVectorObject* mObject;
        std::vector<FSnapshotObject*> mChildrenSnapshotArray;
        double mTranslationX;
        double mTranslationY;
        double mRotation;
        double mScalingX;
        double mScalingY;
};

class ODYSSEYVECTOR_API FSnapshotPath : public FSnapshotObject
{
    public:
        static const uint32 SNAPSHOT_VERTICES          = ( 1 << 0 );
        static const uint32 SNAPSHOT_SEGMENTS          = ( 1 << 1 );
        static const uint32 SNAPSHOT_SELECTED_VERTICES = ( 1 << 2 );
        static const uint32 SNAPSHOT_ALL               = 0xFFFFFFFF;

        ~FSnapshotPath();
        FSnapshotPath( FOdysseyVectorPath* iPath
                      , uint32 iObjectSnapshotFlags
                      , uint32 iPathSnapshotFlags );

        virtual void Restore() override;

    private:
        uint32 mPathSnapshotFlags;
        std::vector<FSnapshotVertex> mVertexSnapshotArray;
        std::vector<FSnapshotSegmentCubic> mCubicSegmentSnapshotArray;
        std::list<FOdysseyVectorVertex*> mSelectedVertexList;
};

class ODYSSEYVECTOR_API FSnapshotGroupPaint : public FSnapshotObject
{
    public:
        static const uint32 SNAPSHOT_BUCKETS          = ( 1 << 0 );
        static const uint32 SNAPSHOT_SELECTED_BUCKETS = ( 1 << 1 );
        static const uint32 SNAPSHOT_ALL              = 0xFFFFFFFF;

        ~FSnapshotGroupPaint();
        FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                           , uint32 iObjectSnapshotFlags
                           , uint32 iPaintGroupSnapshotFlags );

        virtual void Restore() override;

    private:
        uint32 mPaintGroupSnapshotFlags;
        std::vector<FSnapshotBucket> mBucketSnapshotArray;
        std::list<FOdysseyVectorBucket*> mSelectedBucketList;
};


class ODYSSEYVECTOR_API FOdysseyVectorUndo : public FCommandChange
{
    public:
        ~FOdysseyVectorUndo();
        FOdysseyVectorUndo( FOdysseyVectorScene* iScene );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        //virtual FString ToString() const override;

    protected:
        bool mApplied;
        FOdysseyVectorScene* mScene;
};
