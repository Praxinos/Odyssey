#include "OdysseyVectorRectangle.h"

FOdysseyVectorRectangle::~FOdysseyVectorRectangle()
{
}

FOdysseyVectorRectangle::FOdysseyVectorRectangle( const FString iName, double iWidth, double iHeight )
    : FOdysseyVectorPathCubic( iName )
{
    mRectangleParam.StrokeWidth = 4.0f;

    Init( iName, iWidth, iHeight );
}

bool
FOdysseyVectorRectangle::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPathCubic::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorRectangle::Init( const FString& iName, double iWidth, double iHeight )
{
    SetName( iName );
    SetSize( iWidth, iHeight );

    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mRectangleParam.Width );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mRectangleParam.Width );
    mCubicVertex[2] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mRectangleParam.Width );
    mCubicVertex[3] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mRectangleParam.Width );

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
FOdysseyVectorRectangle::UpdateShape( uint32 iUpdateFlags )
{
    mCubicVertex[0]->SetRadius( mRectangleParam.Width );
    mCubicVertex[1]->SetRadius( mRectangleParam.Width );
    mCubicVertex[2]->SetRadius( mRectangleParam.Width );
    mCubicVertex[3]->SetRadius( mRectangleParam.Width );

    mCubicVertex[0]->Set( 0.0f                 , 0.0f                 );
    mCubicVertex[1]->Set( mRectangleParam.Width, 0.0f                 );
    mCubicVertex[2]->Set( mRectangleParam.Width, mRectangleParam.Height );
    mCubicVertex[3]->Set( 0.0f                 , mRectangleParam.Height );

    mCubicSegment[0]->GetHandle(0)->Set(  mRectangleParam.Width  * 0.25f,  0.0f );
    mCubicSegment[0]->GetHandle(1)->Set( -mRectangleParam.Width  * 0.25f,  0.0f );

    mCubicSegment[1]->GetHandle(0)->Set(  mRectangleParam.Height * 0.25f,  0.0f );
    mCubicSegment[1]->GetHandle(1)->Set( -mRectangleParam.Height * 0.25f,  0.0f );

    mCubicSegment[2]->GetHandle(0)->Set( -mRectangleParam.Width  * 0.25f,  0.0f );
    mCubicSegment[2]->GetHandle(1)->Set(  mRectangleParam.Width  * 0.25f,  0.0f );

    mCubicSegment[3]->GetHandle(0)->Set( -mRectangleParam.Height * 0.25f,  0.0f );
    mCubicSegment[3]->GetHandle(1)->Set(  mRectangleParam.Height * 0.25f,  0.0f );

    mCubicSegment[0]->Update();
    mCubicSegment[1]->Update();
    mCubicSegment[2]->Update();
    mCubicSegment[3]->Update();
}

FOdysseyVectorObject*
FOdysseyVectorRectangle::CopyShape()
{
    FOdysseyVectorRectangle* rectangleCopy = new FOdysseyVectorRectangle( mObjectParam.Name
                                                                        , mRectangleParam.Width
                                                                        , mRectangleParam.Height );

    return static_cast<FOdysseyVectorObject*>( rectangleCopy );
}

FOdysseyVectorPathCubic*
FOdysseyVectorRectangle::Convert()
{
    FOdysseyVectorPathCubic* path = static_cast<FOdysseyVectorPathCubic*>(this->FOdysseyVectorPathCubic::CopyShape());

    this->CopySettings( *path );

    return path;
}

void
FOdysseyVectorRectangle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    if ( mRectangleParam.Width && mRectangleParam.Height )
    {
        FOdysseyVectorPathCubic::DrawShape ( iFlags );
    }
}

bool
FOdysseyVectorRectangle::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
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
FOdysseyVectorRectangle::SetSize( double iWidth, double iHeight )
{
    mRectangleParam.Width  = iWidth;
    mRectangleParam.Height = iHeight;

    mBBox.x =    (-mRectangleParam.Width  * 0.5f ) -  mRectangleParam.StrokeWidth;
    mBBox.y =    (-mRectangleParam.Height * 0.5f ) -  mRectangleParam.StrokeWidth;
    mBBox.w =  ( ( mRectangleParam.Width  * 0.5f ) +  mRectangleParam.StrokeWidth ) * 2;
    mBBox.h =  ( ( mRectangleParam.Height * 0.5f ) +  mRectangleParam.StrokeWidth ) * 2;

    Invalidate();
}

double FOdysseyVectorRectangle::GetWidth()
{
    return mRectangleParam.Width;
}

double FOdysseyVectorRectangle::GetHeight()
{
    return mRectangleParam.Height;
}
