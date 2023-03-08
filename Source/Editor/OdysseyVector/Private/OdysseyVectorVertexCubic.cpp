#include "OdysseyVectorVertexCubic.h"

UOdysseyVectorVertexCubic::~UOdysseyVectorVertexCubic()
{
}

UOdysseyVectorVertexCubic::UOdysseyVectorVertexCubic()
    : UOdysseyVectorVertex()
{
}

bool
UOdysseyVectorVertexCubic::IsSmooth()
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
UOdysseyVectorVertexCubic::GetVectorOnSegment( UOdysseyVectorSegment* iSegment, bool iNormalize )
{
    UOdysseyVectorSegmentCubic* cubicSegment = Cast<UOdysseyVectorSegmentCubic>(iSegment);
    ::ULIS::FVec2D vec;

    vec = ( iSegment->GetVertex(0) == this ) ? cubicSegment->GetPointAt(0.01f) - cubicSegment->GetPointAt(0.0f)
                                             : cubicSegment->GetPointAt(0.99f) - cubicSegment->GetPointAt(1.0f);

    if( iNormalize && vec.DistanceSquared() )
    {
        vec.Normalize();
    }

    return vec;
}

void
UOdysseyVectorVertexCubic::Init( double iX, double iY, double iRadius )
{
    mCtrlPoint = UOdysseyVectorHandlePoint::New( this );

    //TODO: use an Init function of the UOdysseyVectorVertex class (does not exist yet ) ?
    Set( iX, iY );
    SetRadius( iRadius, false );
}

// static
UOdysseyVectorVertexCubic*
UOdysseyVectorVertexCubic::New( double iX, double iY, double iRadius )
{
    UOdysseyVectorVertexCubic* cubicVertex = NewObject<UOdysseyVectorVertexCubic>();

    cubicVertex->Init ( iX, iY, iRadius );

    return cubicVertex;
}

UOdysseyVectorHandlePoint*
UOdysseyVectorVertexCubic::GetControlPoint()
{
    return mCtrlPoint;
}

void
UOdysseyVectorVertexCubic::SmoothSegments( bool iBuildSegments )
{
    ::ULIS::FVec2D averageVector( 0.0f, 0.0f );

    if ( this->GetSegmentCount() > 1 )
    {
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

            averageVector += cubicSegment->GetVector( true );
        }

        if ( averageVector.DistanceSquared() )
        {
            averageVector.Normalize();
        }
 
        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic* cubicSegment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            double distance = cubicSegment->GetStraightDistance();
            ::ULIS::FVec2D smoothVector = averageVector;

            if ( this == cubicSegment->GetPoint(0) )
            {
                ::ULIS::FVec2D segmentVector = cubicSegment->GetVector( false );

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                cubicSegment->GetControlPoint(0)->Set( this->GetX() + ( smoothVector.x * distance * 0.35f ),
                                                       this->GetY() + ( smoothVector.y * distance * 0.35f ) );
            }

            if ( this == cubicSegment->GetPoint(1) )
            {
                ::ULIS::FVec2D segmentVector = - cubicSegment->GetVector( false );

                if ( smoothVector.DotProduct( segmentVector ) < 0.0f )
                {
                    smoothVector = - smoothVector;
                }

                cubicSegment->GetControlPoint(1)->Set( this->GetX() + ( smoothVector.x * distance * 0.35f ),
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
UOdysseyVectorVertexCubic::Set( double iX
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
UOdysseyVectorVertexCubic::Set( double iX
                              , double iY )
{
    UOdysseyVectorVertexCubic::Set( iX, iY, true );
}

void
UOdysseyVectorVertexCubic::Set( double iX
                              , double iY
                              , double iRadius
                              , bool iBuildSegments )
{
    SetRadius( iRadius, false );
    Set( iX, iY, iBuildSegments );
}

void
UOdysseyVectorVertexCubic::SetRadius( double iRadius, bool iBuildSegments )
{
    FOdysseyVectorPoint::SetRadius ( iRadius );

    if ( iBuildSegments == true )
    {
        BuildSegments();
    }
}

void
UOdysseyVectorVertexCubic::BuildSegments()
{
    for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
    {
        UOdysseyVectorSegmentCubic* segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);

        segment->BuildVariable();
    }
}

::ULIS::FVec2D
UOdysseyVectorVertexCubic::GetPerpendicularVector( bool iNormalize )
{
    ::ULIS::FVec2D parallel = { 0.0f, 0.0f };
    ::ULIS::FVec2D perpendicular = { 0.0f, 0.0f };

    if ( mSegmentList.size() )
    {
        uint32 count = 0;

        for( std::list<UOdysseyVectorSegment*>::iterator it = mSegmentList.begin(); it != mSegmentList.end(); ++it )
        {
            UOdysseyVectorSegmentCubic *segment = static_cast<UOdysseyVectorSegmentCubic*>(*it);
            FOdysseyVectorPoint* p0 = segment->GetPoint(0);
            FOdysseyVectorPoint* p1 = segment->GetPoint(1);
            ::ULIS::FVec2D& point0 = segment->GetPoint(0)->GetCoords();
            ::ULIS::FVec2D& point1 = segment->GetPoint(1)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint0 = segment->GetControlPoint(0)->GetCoords();
            ::ULIS::FVec2D& ctrlPoint1 = segment->GetControlPoint(1)->GetCoords();

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