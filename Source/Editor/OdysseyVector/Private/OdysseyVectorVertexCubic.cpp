#include "OdysseyVectorVertexCubic.h"

FOdysseyVectorVertexCubic::~FOdysseyVectorVertexCubic()
{
}

FOdysseyVectorVertexCubic::FOdysseyVectorVertexCubic()
    : FOdysseyVectorVertex()
{
}

bool
FOdysseyVectorVertexCubic::IsSmooth()
{
    if( mSegmentList.size() == 2 )
    {
        ::ULIS::FVec2D firstSegmentVector = GetVectorOnSegment( GetFirstSegment(), true );
        ::ULIS::FVec2D lastSegmentVector = GetVectorOnSegment( GetLastSegment(), true );
        double dot = firstSegmentVector.DotProduct( lastSegmentVector );

        if( fabs(dot) > 0.99f )
        {
            return true;
        }
    }

    return false;
}

::ULIS::FVec2D
FOdysseyVectorVertexCubic::GetVectorOnSegment( FOdysseyVectorSegment* iSegment, bool iNormalize )
{
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ? iSegment->GetPointAt(0.01f) - iSegment->GetPointAt(0.0f)
                                             : iSegment->GetPointAt(0.99f) - iSegment->GetPointAt(1.0f);

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

void
FOdysseyVectorVertexCubic::Init( double iX, double iY, double iRadius )
{
    mCtrlPoint = FOdysseyVectorHandlePoint::New( this );

    //TODO: use an Init function of the FOdysseyVectorVertex class (does not exist yet ) ?
    Set( iX, iY );
    SetRadius( iRadius );
}

// static
FOdysseyVectorVertexCubic*
FOdysseyVectorVertexCubic::New( double iX, double iY, double iRadius )
{
    FOdysseyVectorVertexCubic* cubicVertex = new FOdysseyVectorVertexCubic();

    cubicVertex->Init ( iX, iY, iRadius );

    return cubicVertex;
}

FOdysseyVectorHandlePoint*
FOdysseyVectorVertexCubic::GetHandle()
{
    return mCtrlPoint;
}

void
FOdysseyVectorVertexCubic::SmoothSegments( bool iBuildSegments, bool iPreserveHandleLength )
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
            ::ULIS::FVec2D smoothVector = averageVector;

            if ( this == cubicSegment->GetPoint(0) )
            {
                ::ULIS::FVec2D segmentVector = cubicSegment->GetVector( false );
                double distance;

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                if( iPreserveHandleLength )
                {
                    ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(0)->GetCoords();
                    ::ULIS::FVec2D handleVec = handlePos - this->GetCoords( nullptr );

                    distance = handleVec.Distance();
                }
                else
                {
                    distance = cubicSegment->GetStraightDistance() * 0.35f;
                }

                cubicSegment->GetHandle(0)->Set( this->GetX() + ( smoothVector.x * distance ),
                                                 this->GetY() + ( smoothVector.y * distance ) );
            }

            if ( this == cubicSegment->GetPoint(1) )
            {
                ::ULIS::FVec2D segmentVector = - cubicSegment->GetVector( false );
                double distance;

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                if( iPreserveHandleLength )
                {
                    ::ULIS::FVec2D& handlePos = cubicSegment->GetHandle(1)->GetCoords();
                    ::ULIS::FVec2D handleVec = handlePos - this->GetCoords( nullptr );

                    distance = handleVec.Distance();
                }
                else
                {
                    distance = cubicSegment->GetStraightDistance() * 0.35f;
                }

                cubicSegment->GetHandle(1)->Set( this->GetX() + ( smoothVector.x * distance ),
                                                 this->GetY() + ( smoothVector.y * distance ) );
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
FOdysseyVectorVertexCubic::BuildSegments()
{
    for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        FOdysseyVectorSegmentCubic* segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);

        segment->BuildVariable();
    }
}

::ULIS::FVec2D
FOdysseyVectorVertexCubic::GetPerpendicularVector( bool iNormalize )
{
    ::ULIS::FVec2D parallel = { 0.0f, 0.0f };
    ::ULIS::FVec2D perpendicular = { 0.0f, 0.0f };

    if ( mSegmentList.size() )
    {
        uint32 count = 0;

        for( std::list<FOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            FOdysseyVectorSegmentCubic *segment = static_cast<FOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorVertex* p0 = segment->GetVertex(0);
            FOdysseyVectorVertex* p1 = segment->GetVertex(1);
            ::ULIS::FVec2D& point0 = segment->GetVertex(0)->GetCoords( nullptr );
            ::ULIS::FVec2D& point1 = segment->GetVertex(1)->GetCoords( nullptr );
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetHandle(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetHandle(1)->GetCoords();

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