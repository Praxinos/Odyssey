#include "OdysseyVectorEllipse.h"

// https://stackoverflow.com/a/27863181
// https://stackoverflow.com/questions/1734745/how-to-create-circle-with-b%c3%a9zier-curves

#define MAGICRATIO 0.55191502449 // = 4*(sqrt(2)-1)/3

FOdysseyVectorEllipse::~FOdysseyVectorEllipse()
{
}

FOdysseyVectorEllipse::FOdysseyVectorEllipse( const FString& iName, double iRadiusX, double iRadiusY, double iStrokeWidth )
    : FOdysseyVectorPrimitive( iName )
{
    mEllipseParam.StrokeWidth = iStrokeWidth;

    Init( iName, iRadiusX, iRadiusY );
}

bool
FOdysseyVectorEllipse::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPathCubic::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorEllipse::Init( const FString& iName, double iRadiusX, double iRadiusY )
{
    SetName( iName );
    SetRadius( iRadiusX, iRadiusY );

    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mEllipseParam.StrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mEllipseParam.StrokeWidth );
    mCubicVertex[2] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mEllipseParam.StrokeWidth );
    mCubicVertex[3] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mEllipseParam.StrokeWidth );

    mCubicSegment[0] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[0], mCubicVertex[1] );
    mCubicSegment[1] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[1], mCubicVertex[2] );
    mCubicSegment[2] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[2], mCubicVertex[3] );
    mCubicSegment[3] = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[3], mCubicVertex[0] );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );
    AddVertex ( mCubicVertex[2] );
    AddVertex ( mCubicVertex[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );
}

void
FOdysseyVectorEllipse::UpdateShape( uint32 iUpdateFlags )
{
    double ctlDistX = mEllipseParam.RadiusX * MAGICRATIO;
    double ctlDistY = mEllipseParam.RadiusY * MAGICRATIO;

    mCubicVertex[0]->SetRadius( mEllipseParam.StrokeWidth );
    mCubicVertex[1]->SetRadius( mEllipseParam.StrokeWidth );
    mCubicVertex[2]->SetRadius( mEllipseParam.StrokeWidth );
    mCubicVertex[3]->SetRadius( mEllipseParam.StrokeWidth );

    mCubicVertex[0]->Set(  0.0f                 ,  mEllipseParam.RadiusY );
    mCubicVertex[1]->Set(  mEllipseParam.RadiusX,  0.0f                  );
    mCubicVertex[2]->Set(  0.0f                 , -mEllipseParam.RadiusY );
    mCubicVertex[3]->Set( -mEllipseParam.RadiusX,  0.0f                  );

    mCubicSegment[0]->GetHandle(0)->Set(  ctlDistX             ,  mEllipseParam.RadiusY );
    mCubicSegment[0]->GetHandle(1)->Set(  mEllipseParam.RadiusX,  ctlDistY              );

    mCubicSegment[1]->GetHandle(0)->Set(  mEllipseParam.RadiusX, -ctlDistY              );
    mCubicSegment[1]->GetHandle(1)->Set(  ctlDistX             , -mEllipseParam.RadiusY );

    mCubicSegment[2]->GetHandle(0)->Set( -ctlDistX             , -mEllipseParam.RadiusY );
    mCubicSegment[2]->GetHandle(1)->Set( -mEllipseParam.RadiusX, -ctlDistY              );

    mCubicSegment[3]->GetHandle(0)->Set( -mEllipseParam.RadiusX,  ctlDistY              );
    mCubicSegment[3]->GetHandle(1)->Set( -ctlDistX             ,  mEllipseParam.RadiusY );

    mCubicSegment[0]->Update();
    mCubicSegment[1]->Update();
    mCubicSegment[2]->Update();
    mCubicSegment[3]->Update();
}

FOdysseyVectorObject*
FOdysseyVectorEllipse::CopyShape()
{
    FOdysseyVectorEllipse* circleCopy = new FOdysseyVectorEllipse( mObjectParam.Name
                                                                 , mEllipseParam.RadiusX
                                                                 , mEllipseParam.RadiusY
                                                                 , mEllipseParam.StrokeWidth );

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
FOdysseyVectorEllipse::DrawShape( uint64 iFlags )
{
    if ( mEllipseParam.RadiusX && mEllipseParam.RadiusY )
    {
        FOdysseyVectorPathCubic::DrawShape ( iFlags );
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
                ::ULIS::FVec2D &point0 = mCubicSegment[i]->GetVertex(0)->GetCoords();
                ::ULIS::FVec2D &point1 = mCubicSegment[i]->GetVertex(1)->GetCoords();
                ::ULIS::FVec2D &ctrlPoint0 = mCubicSegment[i]->GetHandle(0)->GetCoords();
                ::ULIS::FVec2D &ctrlPoint1 = mCubicSegment[i]->GetHandle(1)->GetCoords();

                path.moveTo( point0.x, point0.y );
                path.cubicTo( ctrlPoint0.x, ctrlPoint0.y, ctrlPoint1.x, ctrlPoint1.y, point1.x, point1.y );
            }

            path.close();

            return ( path.hitTest( pt, BL_FILL_RULE_EVEN_ODD ) == BL_HIT_TEST_IN ) ? true : false;
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
    mEllipseParam.RadiusX = iRadiusX;
    mEllipseParam.RadiusY = iRadiusY;

    mBBox.x = - mEllipseParam.RadiusX - mEllipseParam.StrokeWidth;
    mBBox.y = - mEllipseParam.RadiusY - mEllipseParam.StrokeWidth;
    mBBox.w =  ( mEllipseParam.RadiusX +  mEllipseParam.StrokeWidth ) * 2;
    mBBox.h =  ( mEllipseParam.RadiusY +  mEllipseParam.StrokeWidth ) * 2;

    Invalidate();
}

double
FOdysseyVectorEllipse::GetRadiusX()
{
    return mEllipseParam.RadiusX;
}

double
FOdysseyVectorEllipse::GetRadiusY()
{
    return mEllipseParam.RadiusY;
}

uint32
FOdysseyVectorEllipse::GetType()
{
    return FOdysseyVectorObject::VECTORELLIPSETYPE;
}
