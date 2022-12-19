#include "OdysseyVectorPointCubic.h"

FOdysseyVectorPointCubic::~FOdysseyVectorPointCubic()
{
}

FOdysseyVectorPointCubic::FOdysseyVectorPointCubic( double iX,double iY )
    : FOdysseyVectorPoint( iX, iY )
    , mCtrlPoint ( *this )
{
    SetRadius( 1.0f, false );
}

FOdysseyVectorPointCubic::FOdysseyVectorPointCubic()
    : FOdysseyVectorPointCubic( 0.0f, 0.0f )
{
}

FOdysseyVectorPointCubic::FOdysseyVectorPointCubic( double iX, double iY, double iRadius )
    : FOdysseyVectorPointCubic ( iX, iY )
{
    SetRadius( iRadius, false );
}

FOdysseyVectorHandlePoint&
FOdysseyVectorPointCubic::GetControlPoint()
{
    return mCtrlPoint;
}

void
FOdysseyVectorPointCubic::SmoothSegments( bool iBuildSegments )
{
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if ( this->GetSegmentCount() > 1 )
    {
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

            averageVector += cubicSegment->GetVector( true );
        }

        if ( averageVector.DistanceSquared() )
        {
            averageVector.Normalize();
        }
 
        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic* cubicSegment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            double distance = cubicSegment->GetStraightDistance();
            ::ULIS::FVec2D smoothVector = averageVector;

            if ( this == cubicSegment->GetPoint(0) )
            {
                ::ULIS::FVec2D segmentVector = cubicSegment->GetVector( false );

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                cubicSegment->GetControlPoint(0).Set( this->GetX() + ( smoothVector.x * distance * 0.35f ),
                                                      this->GetY() + ( smoothVector.y * distance * 0.35f ) );
            }

            if ( this == cubicSegment->GetPoint(1) )
            {
                ::ULIS::FVec2D segmentVector = - cubicSegment->GetVector( false );

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                cubicSegment->GetControlPoint(1).Set( this->GetX() + ( smoothVector.x * distance * 0.35f ),
                                                      this->GetY() + ( smoothVector.y * distance * 0.35f ) );
            }

            if ( iBuildSegments == true )
            {
                cubicSegment->Update();
            }
            else
            {
                cubicSegment->Invalidate();
            }
        }
    }
}

void
FOdysseyVectorPointCubic::Set( double iX
                      , double iY
                      , bool iBuildSegments )
{
    FOdysseyVectorPoint::Set( iX, iY );

    if( iBuildSegments == true )
    {
        BuildSegments();
    }
}

void
FOdysseyVectorPointCubic::Set( double iX
                      , double iY )
{
    FOdysseyVectorPointCubic::Set( iX, iY, true );
}

void
FOdysseyVectorPointCubic::Set( double iX
                      , double iY
                      , double iRadius
                      , bool iBuildSegments )
{
    SetRadius( iRadius, false );
    Set( iX, iY, iBuildSegments );
}

void
FOdysseyVectorPointCubic::SetRadius( double iRadius, bool iBuildSegments )
{
    FOdysseyVectorPoint::SetRadius ( iRadius );

    if ( iBuildSegments == true )
    {
        BuildSegments();
    }
}

void
FOdysseyVectorPointCubic::BuildSegments()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        segment->BuildVariable();
    }
}

::ULIS::FVec2D
FOdysseyVectorPointCubic::GetPerpendicularVector( bool iNormalize )
{
    ::ULIS::FVec2D parallel = { 0.0f, 0.0f };
    ::ULIS::FVec2D perpendicular = { 0.0f, 0.0f };

    if ( mSegmentList.size() )
    {
        uint32 count = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint* p0 = segment->GetPoint(0);
            FOdysseyVectorPoint* p1 = segment->GetPoint(1);
            ::ULIS::FVec2D& point0 = segment->GetPoint(0)->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint(1)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint(0).GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint(1).GetCoords();

            if( this == p0 )
            {
                // this is less computation-heavy
                ::ULIS::FVec2D vec = { ctrlPoint0 - point0 };
                /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                     , ctrlPoint0.GetCoords()
                                                                     , ctrlPoint1.GetCoords()
                                                                     , point1.GetCoords()
                                                                     , 0.0f ) };*/

                if ( vec.DistanceSquared() )
                {
                    vec.Normalize();

                    parallel.x += vec.x;
                    parallel.y += vec.y;
                }
            }

            if( this == p1 )
            {
                ::ULIS::FVec2D vec = { point1 - ctrlPoint1 };
                /*::ULIS::FVec2D vec =  { CubicBezierTangentAtParameter<::ULIS::FVec2D>( point0.GetCoords()
                                                                     , ctrlPoint0.GetCoords()
                                                                     , ctrlPoint1.GetCoords()
                                                                     , point1.GetCoords()
                                                                     , 1.0f ) };*/

                if ( vec.DistanceSquared() )
                {
                    vec.Normalize();

                    parallel.x += vec.x;
                    parallel.y += vec.y;
                }
            }
        }

        if ( iNormalize == true )
        {
            if ( parallel.DistanceSquared() )
            {
                parallel.Normalize();
            }

            perpendicular.x =   parallel.y;
            perpendicular.y = - parallel.x;
        }
    }

    return perpendicular;
}