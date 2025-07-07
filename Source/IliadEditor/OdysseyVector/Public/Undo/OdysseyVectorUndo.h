// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorTagInbetweener.h"


class FOdysseyVectorPoint;
class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;
class FOdysseyVectorVertex;
class FOdysseyVectorSegmentCubic;
class FOdysseyVectorTagInbetweener;
class FOdysseyVectorLayer;
class FOdysseyVectorCell;

enum class eSnapshotState : uint8
{
    None = 0,
    Initial = 1,
    Altered = 2
};

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
            static const uint64 LINEARP0        = ( 1ULL << 12 );
            static const uint64 LINEARP1        = ( 1ULL << 13 );
            static const uint64 PARAM = ( COLORMODE
                                        | SPREADINGPOLICY
                                        | SOLIDCOLOR
                                        | ROTATION
                                        | PROPAGATION
                                        | GRADIENTCOLOR0
                                        | GRADIENTCOLOR1
                                        | RADIALRADIUS
                                        | RADIALOFFSET
                                        | PALETTEENTRY
                                        | LINEARP0
                                        | LINEARP1 );
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

    namespace Trajectory
    {
        static const uint64 BEZIER    = ( 1ULL <<  0 );
        static const uint64 WAYPOINTS = ( 1ULL <<  1 );
    }

    namespace Route
    {
        static const uint64 TRAJECTORIES = ( 1ULL <<  0 );
        static const uint64 STEPS        = ( 1ULL <<  1 );
    }

    namespace Breakdown
    {
        static const uint64 GRIDGEOMETRY     = ( 1ULL <<  0 );
        static const uint64 TRANSFORMATIONS  = ( 1ULL <<  1 );
        static const uint64 CHART            = ( 1ULL <<  2 );
        static const uint64 TARGETVISIBILITY = ( 1ULL <<  3 );
    }

    namespace Chart
    {
        static const uint64 SPACING = ( 1ULL <<  1 );
        static const uint64 BEZIER  = ( 1ULL <<  2 );
    }

    namespace Tag
    {
        namespace Inbetweener
        {
            static const uint64 GRIDSIZE          = ( 1ULL <<  0 );
            static const uint64 GRIDTYPE          = ( 1ULL <<  1 );
            static const uint64 BREAKDOWNS        = ( 1ULL <<  2 );
            static const uint64 ROUTES            = ( 1ULL <<  3 );
            static const uint64 INTERPOLATIONTYPE = ( 1ULL <<  4 );
            static const uint64 ARAPRIGIDITY      = ( 1ULL <<  5 );
            static const uint64 COLOR             = ( 1ULL <<  6 );
            static const uint64 MAPASPOLYLINE     = ( 1ULL <<  7 );
            static const uint64 SQUARE            = ( 1ULL <<  8 );
            static const uint64 WITHTHICKNESS     = ( 1ULL <<  9 );
            static const uint64 CONSTANTWIDTH     = ( 1ULL << 10 );
            static const uint64 PARAM             = ( GRIDSIZE
                                                    | GRIDTYPE
                                                    | INTERPOLATIONTYPE
                                                    | ARAPRIGIDITY
                                                    | COLOR
                                                    | MAPASPOLYLINE
                                                    | SQUARE
                                                    | WITHTHICKNESS );
        }
    }

    namespace Object
    {
        static const uint64 TRANSFORMATIONS           = ( 1ULL <<  0 );
        static const uint64 COLORING                  = ( 1ULL <<  1 );
        static const uint64 OPACITY                   = ( 1ULL <<  2 );
        static const uint64 VISIBILITY                = ( 1ULL <<  3 );
        static const uint64 NAME                      = ( 1ULL <<  4 );
        static const uint64 HIERARCHY                 = ( 1ULL <<  5 );
        //static const uint64 CHILDREN_TRANSFORMATIONS  = ( 1ULL <<  6 );

        namespace Path
        {
            static const uint64 VERTICES          = ( 1ULL <<  7 );
            static const uint64 SEGMENTS          = ( 1ULL <<  8 );
            static const uint64 SELECTED_VERTICES = ( 1ULL <<  9 );
            static const uint64 TOPOLOGY          = ( 1ULL << 10 );
            static const uint64 BRUSH             = ( 1ULL << 11 );
            static const uint64 JOINTTYPE         = ( 1ULL << 12 );
            static const uint64 MITERLIMIT        = ( 1ULL << 13 );
        }

        namespace Group
        {
            static const uint64 HUDCOLOR = ( 1ULL <<  7 );

            namespace Paint
            {
                static const uint64 BUCKETS                    = ( 1ULL <<  8 );
                static const uint64 SELECTED_BUCKETS           = ( 1ULL <<  9 );
                static const uint64 PAINTED                    = ( 1ULL << 10 );
                static const uint64 MONOCHROME                 = ( 1ULL << 11 );
                static const uint64 MONOCHROMECOLOR            = ( 1ULL << 12 );
                static const uint64 REALTIME                   = ( 1ULL << 13 );
                static const uint64 GAPTOLERANCE               = ( 1ULL << 14 );
                static const uint64 WIREFRAME                  = ( 1ULL << 15 );
                static const uint64 WIREFRAMECOLOR             = ( 1ULL << 16 );
                static const uint64 INTERSECTSCANVAS           = ( 1ULL << 17 );
                static const uint64 GAPDETECTIONSCHEME         = ( 1ULL << 18 );
                static const uint64 SEGMENTEXTENSIONSCHEME     = ( 1ULL << 19 );
                static const uint64 SEGMENTEXTENSIONSIMPLIFIED = ( 1ULL << 20 );
            }
        }
    }
}

class ODYSSEYVECTOR_API FSnapshotPoint
{
    struct State
    {
        bool inited;

        ULIS::FVec2D coords;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotPoint();
        FSnapshotPoint( FOdysseyVectorPoint* iPoint
                      , uint64 iSnapshotFlags
                      , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iState );
        virtual bool LoadState( eSnapshotState iState );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotPoint& operator=( const FSnapshotPoint& iOther )
        {
            mSnapshotFlags = iOther.mSnapshotFlags;
            mPoint = iOther.mPoint;

            mPointInitialState = new State();
            mPointAlteredState = new State();

            *mPointInitialState = *iOther.mPointInitialState;
            *mPointAlteredState = *iOther.mPointAlteredState;

            return *this;
        }

    protected:
        uint64 mSnapshotFlags;
        FOdysseyVectorPoint* mPoint;
        State* mPointInitialState;
        State* mPointAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotVertex : public FSnapshotPoint
{
    struct State
    {
        bool inited;
        bool alignment;
        bool locked;
        double radius;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotVertex();
        FSnapshotVertex( FOdysseyVectorVertex* iVertex
                       , uint64 iSnapshotFlags
                       , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iState ) override;
        virtual bool LoadState( eSnapshotState iState ) override;

        FOdysseyVectorVertex* GetVertex();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotVertex& operator=( const FSnapshotVertex& iOther )
        {
            FSnapshotPoint::operator=(iOther);

            mVertexInitialState = new State();
            mVertexAlteredState = new State();

            *mVertexInitialState = *iOther.mVertexInitialState;
            *mVertexAlteredState = *iOther.mVertexAlteredState;

            return *this;
        }

    protected:
        State* mVertexInitialState;
        State* mVertexAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotBucket : public FSnapshotPoint
{
    struct State
    {
        bool inited;

        eBucketColorMode colorMode;
        FColor solidColor;
        double rotation;
        bool propagated;
        FColor gradientColor0;
        FColor gradientColor1;
        double radialRadius;
        ::ULIS::FVec2D radialOffset;
        ::ULIS::FVec2D linearP0;
        ::ULIS::FVec2D linearP1;
        UOdysseyPaletteEntry* paletteEntry;

        State() { inited = false; }
    };

    public:
        ~FSnapshotBucket();
        FSnapshotBucket( FOdysseyVectorBucket* iBucket
                       , uint64 iSnapshotFlags
                       , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iState ) override;
        virtual bool LoadState( eSnapshotState iState ) override;

        FOdysseyVectorBucket* GetBucket();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotBucket& operator=( const FSnapshotBucket& iOther )
        {
            FSnapshotPoint::operator=(iOther);

            mBucketInitialState = new State();
            mBucketAlteredState = new State();

            *mBucketInitialState = *iOther.mBucketInitialState;
            *mBucketAlteredState = *iOther.mBucketAlteredState;

            return *this;
        }

    private:
        State* mBucketInitialState;
        State* mBucketAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotSegmentCubic
{
    struct State
    {
        bool inited;
        ULIS::FVec2D handleCoords[2];

        State() { inited = false; }
    };

    public:
        ~FSnapshotSegmentCubic();
        FSnapshotSegmentCubic( FOdysseyVectorSegmentCubic* iCubicSegment
                             , uint64 iSnapshotFlags
                             , eSnapshotState iSnapshotState );

        void RecordState( eSnapshotState iState );
        bool LoadState( eSnapshotState iState );

        FOdysseyVectorSegmentCubic* GetCubicSegment();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotSegmentCubic& operator=( const FSnapshotSegmentCubic& iOther )
        {
            mSnapshotFlags = iOther.mSnapshotFlags;
            mCubicSegment = iOther.mCubicSegment;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    private:
        uint32 mSnapshotFlags;
        FOdysseyVectorSegmentCubic* mCubicSegment;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotTrajectory
{
    struct State
    {
        bool inited;
        ::ULIS::FVec2D handleDirection[2];
        double handleLengthRatio[2];
        std::vector<float> waypointSpacingBuffer;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotTrajectory();
        FSnapshotTrajectory( FInbetweenerTrajectory* iTrajectory
                           , uint64 iSnapshotFlags
                           , eSnapshotState iState );

        void RecordState( eSnapshotState iState );
        bool LoadState( eSnapshotState iState );

        static void WaypointSpacingToArray( FInbetweenerTrajectory* iTrajectory
                                          , std::vector<float>& oSpacingBuffer );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotTrajectory& operator=( const FSnapshotTrajectory& iOther )
        {
            mSnapshotFlags = iOther.mSnapshotFlags;
            mRoute = iOther.mRoute;
            mIndex = iOther.mIndex;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        uint64 mSnapshotFlags;
        FInbetweenerRoute* mRoute;
        uint32 mIndex;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotStep
{
    struct State
    {
        bool inited;
        bool aligned;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotStep();
        FSnapshotStep( FInbetweenerStep* iStep
                     , eSnapshotState iState );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotStep& operator=( const FSnapshotStep& iOther )
        {
            //mSnapshotFlags = iOther.mSnapshotFlags;
            mRoute = iOther.mRoute;
            mIndex = iOther.mIndex;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FInbetweenerRoute* mRoute;
        uint32 mIndex;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotRoute
{
    struct State
    {
        bool inited;
        std::vector<FSnapshotTrajectory> trajectorySnapshotBuffer;
        std::vector<FSnapshotStep> stepSnapshotBuffer;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotRoute();
        FSnapshotRoute( FInbetweenerRoute* iRoute
                      , uint64 iSnapshotflags
                      , uint64 iTrajectorySnapshotFlags
                      , eSnapshotState iState );

        bool LoadState( eSnapshotState iStateType );
        void RecordState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotRoute& operator=( const FSnapshotRoute& iOther )
        {
            mRoute = iOther.mRoute;
            mSnapshotFlags = iOther.mSnapshotFlags;
            mTrajectorySnapshotFlags = iOther.mTrajectorySnapshotFlags;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FInbetweenerRoute* mRoute;
        uint64 mSnapshotFlags;
        uint64 mTrajectorySnapshotFlags;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotLayout
{
    struct State
    {
        bool inited;
        std::vector<FInbetweenerBreakdown*> breakdownArray;
        std::vector<uint32> targetBuffer;

        ~State();
        State() { inited = false; }
    };

    public:
        ~FSnapshotLayout();
        FSnapshotLayout( FOdysseyVectorTagInbetweener* iInbetweenerTag
                       , eSnapshotState iState );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotLayout& operator=( const FSnapshotLayout& iOther )
        {
            mInbetweenerTag = iOther.mInbetweenerTag;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotDynamics
{
    struct State
    {
        bool inited;
        std::vector<FInbetweenerRoute*> routeArray;

        State() { inited = false; }
    };

    public:
        ~FSnapshotDynamics();
        FSnapshotDynamics();
        FSnapshotDynamics( FOdysseyVectorTagInbetweener* iInbetweenerTag
                         , eSnapshotState iState );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotDynamics& operator=( const FSnapshotDynamics& iOther )
        {
            mInbetweenerTag = iOther.mInbetweenerTag;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotInbetweenerChart
{
    struct State
    {
        bool inited;
        std::vector<float> spacing;
        ::ULIS::FVec2D HUDBezier[3];

        State() { inited = false; }
    };

    public:
        ~FSnapshotInbetweenerChart();
        FSnapshotInbetweenerChart( FInbetweenerChart* iChart
                                 , uint64 iSnapshotFlags
                                 , eSnapshotState iState );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotInbetweenerChart& operator=( const FSnapshotInbetweenerChart& iOther )
        {
            mChart = iOther.mChart;
            mSnapshotFlags = iOther.mSnapshotFlags;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FInbetweenerChart* mChart;
        uint64 mSnapshotFlags;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotInbetweenerBreakdown
{
    struct State
    {
        bool inited;
        std::vector<::ULIS::FVec2D> gridGeometry;
        double translationX;
        double translationY;
        double rotation;
        double scalingX;
        double scalingY;
        bool targetVisibility;
        std::vector<FSnapshotInbetweenerChart> chartSnapshotBuffer;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotInbetweenerBreakdown();
        FSnapshotInbetweenerBreakdown( FInbetweenerBreakdown* iBreakdown
                                     , uint64 iSnapshotFlags
                                     , eSnapshotState iState );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotInbetweenerBreakdown& operator=( const FSnapshotInbetweenerBreakdown& iOther )
        {
            mBreakdown = iOther.mBreakdown;
            mSnapshotFlags = iOther.mSnapshotFlags;

            mInitialState = new State();
            mAlteredState = new State();

            *mInitialState = *iOther.mInitialState;
            *mAlteredState = *iOther.mAlteredState;

            return *this;
        }

    protected:
        FInbetweenerBreakdown* mBreakdown;
        uint64 mSnapshotFlags;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotTagInbetweener
{
    struct State
    {
        bool inited;

        uint32 drawingCount;
        eInbetweenerGridType gridType;
        eInbetweenerInterpolationType interpolationType;
        eInbetweenerInterpolationDirection interpolationDirection;
        uint32 gridSizeX;
        uint32 gridSizeY;
        uint32 ARAPRigidity;
        FColor inbetweenColor;
        FColor chartColor;
        FColor gridColor;
        FColor trajectoryColor;
        bool mapAsPolyline;
        bool square;
        bool withThickness;
        bool constantWidth;
        std::vector<FSnapshotLayout> layoutSnapshotBuffer;
        std::vector<FSnapshotDynamics> dynamicsSnapshotBuffer;
        std::vector<FSnapshotInbetweenerBreakdown> inbetweenerBreakdownSnapshotBuffer;
        std::vector<FSnapshotRoute> routeSnapshotBuffer;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotTagInbetweener();
        FSnapshotTagInbetweener( FOdysseyVectorTagInbetweener* iObject
                               , uint64 iSnapshotFlags
                               , uint64 iBreakdownSnapshotFlags
                               , uint64 iRouteSnapshotFlags
                               , uint64 iTrajectorySnapshotFlags
                               , eSnapshotState iStateType );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

    protected:
        uint64 mSnapshotFlags;
        uint64 mBreakdownSnapshotFlags;
        uint64 mRouteSnapshotFlags;
        uint64 mTrajectorySnapshotFlags;
        FOdysseyVectorTagInbetweener* mInbetweenerTag;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotObject
{
    struct State
    {
        bool inited;

        FOdysseyVectorObject* parent;
        FOdysseyVectorObject* previousChild;
        double translationX;
        double translationY;
        double rotation;
        double scalingX;
        double scalingY;
        FString name;
        double opacity;
        bool visibility;
        FSnapshotBucket foregroundBucketSnapshot;
        FSnapshotBucket backgroundBucketSnapshot;

        State( FOdysseyVectorObject* iObject
             , uint64 iSnapshotFlags
             , eSnapshotState iStateType )
        : foregroundBucketSnapshot( &iObject->GetForegroundBucket()
                                 , ( iSnapshotFlags & FSnapshotFlags::Object::COLORING ) ? FSnapshotFlags::Point::Bucket::PARAM : 0
                                 , iStateType )
        , backgroundBucketSnapshot( &iObject->GetBackgroundBucket()
                                 , ( iSnapshotFlags & FSnapshotFlags::Object::COLORING ) ? FSnapshotFlags::Point::Bucket::PARAM : 0
                                 , iStateType )
        {
            inited = false;
        }
    };

    public:
        virtual ~FSnapshotObject();
        FSnapshotObject( FOdysseyVectorObject* iObject
                       , uint64 iSnapshotFlags
                       , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iStateType );
        virtual bool LoadState( eSnapshotState iStateType );

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotObject& operator=( const FSnapshotObject& iOther )
        {
            mObject = iOther.mObject;
            mSnapshotFlags = iOther.mSnapshotFlags;

            mObjectInitialState = new State( mObject, mSnapshotFlags, eSnapshotState::None );
            mObjectAlteredState = new State( mObject, mSnapshotFlags, eSnapshotState::None );

            *mObjectInitialState = *iOther.mObjectInitialState;
            *mObjectAlteredState = *iOther.mObjectAlteredState;

            return *this;
        }

    protected:
        FOdysseyVectorObject* mObject;
        uint64 mSnapshotFlags;
        State* mObjectInitialState;
        State* mObjectAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotPath : public FSnapshotObject
{
    struct State
    {
        bool inited;

        std::vector<FSnapshotVertex> vertexSnapshotBuffer;
        std::vector<FSnapshotSegmentCubic> cubicSegmentSnapshotBuffer;
        std::list<FOdysseyVectorVertex*> selectedVertexList;
        std::list<FOdysseyVectorVertex*> topologyVertexList;
        std::list<FOdysseyVectorSegment*> topologySegmentList;
        FOdysseyVectorBrush brush;
        eJointType jointType;
        double miterLimit;

        State() : brush( nullptr ) { inited = false; }
    };

    public:
        virtual ~FSnapshotPath();
        FSnapshotPath( FOdysseyVectorPath* iPath
                     , uint64 iSnapshotFlags
                     , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iStateType );
        virtual bool LoadState( eSnapshotState iStateType );

        FOdysseyVectorPath* GetPath();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotPath& operator=( const FSnapshotPath& iOther )
        {
            mPathInitialState = new State();
            mPathAlteredState = new State();

            *mPathInitialState = *iOther.mPathInitialState;
            *mPathAlteredState = *iOther.mPathAlteredState;

            return *this;
        }

    private:
        State* mPathInitialState;
        State* mPathAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotGroup : public FSnapshotObject
{
    struct State
    {
        bool inited;

        FColor HUDColor;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotGroup();
        FSnapshotGroup( FOdysseyVectorGroup* iGroup
                      , uint64 iSnapshotFlags
                      , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iStateType ) override;
        virtual bool LoadState( eSnapshotState iStateType ) override;

        FOdysseyVectorGroup* GetGroup();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotGroup& operator=( const FSnapshotGroup& iOther )
        {
            mGroupInitialState = new State();
            mGroupAlteredState = new State();

            *mGroupInitialState = *iOther.mGroupInitialState;
            *mGroupAlteredState = *iOther.mGroupAlteredState;

            return *this;
        }

    private:
        State* mGroupInitialState;
        State* mGroupAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotGroupPaint : public FSnapshotGroup
{
    struct State
    {
        bool inited;

        std::list<FOdysseyVectorBucket*> selectedBucketList;
        bool painted;
        bool monochrome;
        FColor monochromeColor;
        bool realtime;
        double gapTolerance;
        bool wireframe;
        FColor wireframeColor;
        eSegmentExtensionScheme segmentExtensionScheme;
        eGapDetectionScheme gapDetectionScheme;
        bool segmentExtensionSimplified;
        std::vector<FSnapshotBucket> bucketSnapshotBuffer;
        bool intersectsCanvas;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotGroupPaint();
        FSnapshotGroupPaint( FOdysseyVectorGroupPaint* iPaintGroup
                           , uint64 iSnapshotFlags
                           , eSnapshotState iStateType );

        virtual void RecordState( eSnapshotState iStateType ) override;
        virtual bool LoadState( eSnapshotState iStateType ) override;

        FOdysseyVectorGroupPaint* GetPaintGroup();

        // copy operator for safe copies, as we delete states on destruction
        // each copy need its own allocated pointers to those states.
        FSnapshotGroupPaint& operator=( const FSnapshotGroupPaint& iOther )
        {
            mPaintgroupInitialState = new State();
            mPaintgroupAlteredState = new State();

            *mPaintgroupInitialState = *iOther.mPaintgroupInitialState;
            *mPaintgroupAlteredState = *iOther.mPaintgroupAlteredState;

            return *this;
        }

    private:
        State* mPaintgroupInitialState;
        State* mPaintgroupAlteredState;
};

class ODYSSEYVECTOR_API FSnapshotCell : public FSnapshotObject
{
    struct State
    {
        std::vector<FOdysseyVectorObject*> selectedObjectArray;
        bool inited;

        State() { inited = false; }
    };

    public:
        virtual ~FSnapshotCell();
        FSnapshotCell( FOdysseyVectorCell* iCell, uint64 iSnapshotFlags, eSnapshotState iStateType );

        void RecordState( eSnapshotState iStateType );
        bool LoadState( eSnapshotState iStateType );

        FOdysseyVectorCell* GetCell();

    private:
        uint64 mSnapshotFlags;
        State* mInitialState;
        State* mAlteredState;
};

class ODYSSEYVECTOR_API FOdysseyVectorUndo : public FCommandChange
{
    public:
        ~FOdysseyVectorUndo();
        FOdysseyVectorUndo( FOdysseyVectorLayer* iSharedEnv, uint64 iReturnFlags );

    protected:
        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        //virtual FString ToString() const override;

        void Update();

    protected:
        bool mApplied;
        std::list<FOdysseyVectorCell*> mRootList; // list of engines that need to be redrawn
        FOdysseyVectorLayer* mLayer;
        uint64 mReturnFlags;
};
