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
#include "OdysseyVectorTagInbetweener.h"

class FOdysseyVectorPoint;
class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorVertex;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorTagInbetweener;

namespace FSnapshotFlags
{
    static const uint64 ALL = 0xFFFFFFFFFFFFFFFFULL;

    namespace Point
    {
        static const uint64 POSITION = ( 1ULL <<  0 );

        namespace Vertex
        {
            static const uint64 RADIUS          = ( 1ULL <<  1 );
            static const uint64 ALIGNMENT       = ( 1ULL <<  2 );
            static const uint64 LOCK            = ( 1ULL <<  3 );
            static const uint64 PARAM           = ( ALIGNMENT );
        }

        namespace Bucket
        {
            static const uint64 COLORMODE       = ( 1ULL <<  1 );
            static const uint64 SPREADINGPOLICY = ( 1ULL <<  2 );
            static const uint64 SOLIDCOLOR      = ( 1ULL <<  3 );
            static const uint64 ROTATION        = ( 1ULL <<  4 );
            static const uint64 PROPAGATION     = ( 1ULL <<  5 );
            static const uint64 GRADIENTCOLOR0  = ( 1ULL <<  6 );
            static const uint64 GRADIENTCOLOR1  = ( 1ULL <<  7 );
            static const uint64 RADIALRADIUS    = ( 1ULL <<  8 );
            static const uint64 RADIALOFFSET    = ( 1ULL <<  9 );
            static const uint64 PALETTEENTRY    = ( 1ULL << 10 );
            static const uint64 PARAM = ( COLORMODE
                                        | SPREADINGPOLICY
                                        | SOLIDCOLOR 
                                        | ROTATION
                                        | PROPAGATION
                                        | GRADIENTCOLOR0
                                        | GRADIENTCOLOR1 
                                        | RADIALRADIUS
                                        | RADIALOFFSET
                                        | PALETTEENTRY );
        }
    }

    namespace Segment
    {
        namespace Cubic
        {
            static const uint64 HANDLES = ( 1ULL << 0 );
        }

        //static const uint32 SNAPSHOT_ALL = 0xFFFFFFFFFFFFFFFFULL;
    }

    namespace Tag
    {
        namespace Inbetweener
        {
            static const uint64 TRANSFORMATIONS   = ( 1ULL <<  0 );
            static const uint64 CHART             = ( 1ULL <<  1 );
            static const uint64 INBETWEENCOUNT    = ( 1ULL <<  3 );
            static const uint64 GRIDSIZE          = ( 1ULL <<  4 );
            static const uint64 GRIDTYPE          = ( 1ULL <<  5 );
            static const uint64 GRIDGEOMETRY      = ( 1ULL <<  6 );
            static const uint64 INTERPOLATIONTYPE = ( 1ULL <<  7 );
            static const uint64 PARAM             = ( INBETWEENCOUNT
                                                    | GRIDSIZE
                                                    | GRIDTYPE
                                                    | GRIDGEOMETRY
                                                    | INTERPOLATIONTYPE );
        }
    }

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
    }
}

class ODYSSEYVECTOR_API FSnapshotTrajectory
{
    public:
        virtual ~FSnapshotTrajectory();
        FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory );

        virtual void Restore();

    protected:
        FInbetweenerTrajectory* mTrajectory;
        ::ULIS::FVec2D mHandleDirection[2];
        double mHandleLengthRatio[2];
};

class ODYSSEYVECTOR_API FSnapshotPoint
{
    public:
        virtual ~FSnapshotPoint();
        FSnapshotPoint( FOdysseyVectorPoint* iPoint, uint64 iSnapshotFlags );

        virtual void Restore();

    protected:
        uint64 mSnapshotFlags;
        FOdysseyVectorPoint* mPoint;
        ULIS::FVec2D mCoords;
        double mRadius;
};

class ODYSSEYVECTOR_API FSnapshotVertex : public FSnapshotPoint
{
    public:
        virtual ~FSnapshotVertex();
        FSnapshotVertex( FOdysseyVectorVertex* iVertex, uint64 iSnapshotFlags );

        FOdysseyVectorVertex* GetVertex();

        virtual void Restore() override;

    protected:
        bool mAlignment;
        bool mLocked;
};

class ODYSSEYVECTOR_API FSnapshotBucket : public FSnapshotPoint
{
    public:
        ~FSnapshotBucket();
        //FSnapshotBucket();
        FSnapshotBucket( FOdysseyVectorBucket* iBucket, uint64 iSnapshotFlags );

        void Restore();

        FOdysseyVectorBucket* GetBucket();

    private:
        eBucketColorMode mColorMode;
        eBucketSpreadingPolicy mSpreadingPolicy;
        FColor mSolidColor;
        double mRotation;
        bool mPropagated;
        FColor mGradientColor0;
        FColor mGradientColor1;
        double mRadialRadius;
        ::ULIS::FVec2D mRadialOffset;
        UOdysseyPaletteEntry* mPaletteEntry;
};

class ODYSSEYVECTOR_API FSnapshotSegmentCubic
{
    public:
        ~FSnapshotSegmentCubic();
        FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment, uint64 iSnapshotFlags );

        void Restore();

        FOdysseyVectorSegmentCubic* GetCubicSegment();

    private:
        uint32 mSnapshotFlags;
        FOdysseyVectorSegmentCubic* mCubicSegment;
        ULIS::FVec2D mHandleCoords[2];
};

class ODYSSEYVECTOR_API FSnapshotTagInbetweener
{
    public:
        virtual ~FSnapshotTagInbetweener();
        FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iObject
                               , uint64 iSnapshotFlags );

        virtual bool Restore();

    protected:
        uint64 mSnapshotFlags;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        FInbetweenerChart  mChart;
        double mTranslationX;
        double mTranslationY;
        double mRotation;
        double mScalingX;
        double mScalingY;
        uint32 mInbetweenCount;
        eInbetweenerGridType mGridType;
        eInbetweenerInterpolationType mInterpolationType;
        uint32 mGridSizeX;
        uint32 mGridSizeY;
        std::vector<::ULIS::FVec2D> mGridGeometry;
};

class ODYSSEYVECTOR_API FSnapshotObject
{
    public:
        virtual ~FSnapshotObject();
        FSnapshotObject( FOdysseyVectorObject* iObject, uint64 iSnapshotFlags );

        virtual bool Restore();

    protected:
        uint64 mSnapshotFlags;
        FOdysseyVectorObject* mObject;
        FOdysseyVectorObject* mParent;
        FOdysseyVectorObject* mPreviousChild;
        FSnapshotBucket mForegroundBucketSnapshot;
        FSnapshotBucket mBackgroundBucketSnapshot;
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
