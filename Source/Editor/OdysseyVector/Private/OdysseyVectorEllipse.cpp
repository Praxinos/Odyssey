#include "OdysseyVectorEllipse.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

FOdysseyVectorEllipse::~FOdysseyVectorEllipse()
{
}

FOdysseyVectorEllipse::FOdysseyVectorEllipse()
    : FOdysseyVectorPathCubic()
    , mStrokeWidth ( 4.0f )
{
    SetName( "Circle" );

    mRadiusX = mRadiusY = 0.0f;
}

void
FOdysseyVectorEllipse::Init( std::string iName, double iRadiusX, double iRadiusY )
{
    SetName( iName );
    SetRadius( iRadiusX, iRadiusY );

    mCubicVertex[0] = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[1] = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[2] = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );
    mCubicVertex[3] = FOdysseyVectorVertexCubic::New( 0.0f, 0.0f, 1.0f );

    mCubicSegment[0] = FOdysseyVectorSegmentCubic::New( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[0], mCubicVertex[1] );
    mCubicSegment[1] = FOdysseyVectorSegmentCubic::New( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[1], mCubicVertex[2] );
    mCubicSegment[2] = FOdysseyVectorSegmentCubic::New( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[2], mCubicVertex[3] );
    mCubicSegment[3] = FOdysseyVectorSegmentCubic::New( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[3], mCubicVertex[0] );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );
    AddVertex ( mCubicVertex[2] );
    AddVertex ( mCubicVertex[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );
}

//static
FOdysseyVectorEllipse*
FOdysseyVectorEllipse::New( std::string iName, double iRadiusX, double iRadiusY )
{
    FOdysseyVectorEllipse* circle = new FOdysseyVectorEllipse();

    circle->Init( iName, iRadiusX, iRadiusY );

    return circle;
}

void
FOdysseyVectorEllipse::UpdateShape( uint32 iUpdateFlags )
{
    double ctlDistX = mRadiusX * MAGICRATIO;
    double ctlDistY = mRadiusY * MAGICRATIO;

    mCubicVertex[0]->Set(  0.0f    ,  mRadiusY );
    mCubicVertex[1]->Set(  mRadiusX,  0.0f     );
    mCubicVertex[2]->Set(  0.0f    , -mRadiusY );
    mCubicVertex[3]->Set( -mRadiusX,  0.0f     );

    mCubicSegment[0]->GetHandle(0)->Set(  ctlDistX,  mRadiusY );
    mCubicSegment[0]->GetHandle(1)->Set(  mRadiusX,  ctlDistY );

    mCubicSegment[1]->GetHandle(0)->Set(  mRadiusX, -ctlDistY );
    mCubicSegment[1]->GetHandle(1)->Set(  ctlDistX, -mRadiusY );

    mCubicSegment[2]->GetHandle(0)->Set( -ctlDistX, -mRadiusY );
    mCubicSegment[2]->GetHandle(1)->Set( -mRadiusX, -ctlDistY );

    mCubicSegment[3]->GetHandle(0)->Set( -mRadiusX,  ctlDistY );
    mCubicSegment[3]->GetHandle(1)->Set( -ctlDistX,  mRadiusY );

    mCubicSegment[0]->Update();
    mCubicSegment[1]->Update();
    mCubicSegment[2]->Update();
    mCubicSegment[3]->Update();
}

FOdysseyVectorObject*
FOdysseyVectorEllipse::CopyShape()
{
    FOdysseyVectorEllipse* circleCopy = new FOdysseyVectorEllipse();

    circleCopy->Init ( Name, mRadiusX, mRadiusY );

    return static_cast<FOdysseyVectorObject*>( circleCopy );
}

FOdysseyVectorPathCubic*
FOdysseyVectorEllipse::Convert()
{
    FOdysseyVectorPathCubic* path = static_cast<FOdysseyVectorPathCubic*>(this->FOdysseyVectorPathCubic::CopyShape());

    this->CopySettings( *path );

    return path;
}

void
FOdysseyVectorEllipse::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    if ( mRadiusX && mRadiusY )
    {
        FOdysseyVectorPathCubic::DrawShape ( iRoi, iFlags );
    }
}

bool
FOdysseyVectorEllipse::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    if( mPathParam.Filled )
    {
        if( iSelectionFlags & FOdysseyVectorPathCubic::PICK_MATH_BASED )
        {
            BLPath path;
            BLPoint pt = mInverseWorldMatrix.mapPoint( iRoi.x, iRoi.y );

            path.clear();

            for( int i = 0; i < 4; i++ )
            {
                ::ULIS::FVec2D &point0 = mCubicSegment[i]->GetVertex(0)->GetCoords( mCubicSegment[i] );
                ::ULIS::FVec2D &point1 = mCubicSegment[i]->GetVertex(1)->GetCoords( mCubicSegment[i] );
                ::ULIS::FVec2D &ctrlPoint0 = mCubicSegment[i]->GetHandle(0)->GetCoords();
                ::ULIS::FVec2D &ctrlPoint1 = mCubicSegment[i]->GetHandle(1)->GetCoords();

                path.moveTo( point0.x, point0.y );
                path.cubicTo( ctrlPoint0.x, ctrlPoint0.y, ctrlPoint1.x, ctrlPoint1.y, point1.x, point1.y );
            }

            path.close();

            return path.hitTest( pt, BL_FILL_RULE_EVEN_ODD ) ? true : false;
        }
    }
    else
    {
        return FOdysseyVectorPathCubic::PickShape( iRoi, iSelectionFlags );
    }

    return false;
}

void
FOdysseyVectorEllipse::SetRadius( double iRadius )
{
    SetRadius( iRadius, iRadius );
}

void
FOdysseyVectorEllipse::SetRadius( double iRadiusX, double iRadiusY )
{
    mRadiusX = iRadiusX;
    mRadiusY = iRadiusY;

    mBBox.x = -mRadiusX - mStrokeWidth;
    mBBox.y = -mRadiusY - mStrokeWidth;
    mBBox.w =  ( mRadiusX +  mStrokeWidth ) * 2;
    mBBox.h =  ( mRadiusY +  mStrokeWidth ) * 2;

    //Invalidate();
}

double
FOdysseyVectorEllipse::GetRadiusX()
{
    return mRadiusX;
}

double
FOdysseyVectorEllipse::GetRadiusY()
{
    return mRadiusY;
}

uint32
FOdysseyVectorEllipse::GetType()
{
    return FOdysseyVectorObject::VECTORELLIPSETYPE;
}
