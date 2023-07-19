#include "OdysseyVectorRectangle.h"

FOdysseyVectorRectangle::~FOdysseyVectorRectangle()
{
   // vertices ans segments freed in OdysseyVectorPath::~destructor
}

FOdysseyVectorRectangle::FOdysseyVectorRectangle( const FString iName, double iWidth, double iHeight, double iStrokeWidth )
    : FOdysseyVectorPrimitive( iName )
    , mWidth( iWidth )
    , mHeight( iHeight )
    , mStrokeWidth( iStrokeWidth )
{
    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[2] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[3] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );

    mCubicSegment[0] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[0], mCubicVertex[1] );
    mCubicSegment[1] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[1], mCubicVertex[2] );
    mCubicSegment[2] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[2], mCubicVertex[3] );
    mCubicSegment[3] = new FOdysseyVectorSegmentCubic( this, mCubicVertex[3], mCubicVertex[0] );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );
    AddVertex ( mCubicVertex[2] );
    AddVertex ( mCubicVertex[3] );

    AddSegment ( mCubicSegment[0] );
    AddSegment ( mCubicSegment[1] );
    AddSegment ( mCubicSegment[2] );
    AddSegment ( mCubicSegment[3] );

    UpdateShape( 0 );
}

bool
FOdysseyVectorRectangle::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPath::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorRectangle::UpdateShape( uint32 iUpdateFlags )
{
    mCubicVertex[0]->SetRadius( mStrokeWidth );
    mCubicVertex[1]->SetRadius( mStrokeWidth );
    mCubicVertex[2]->SetRadius( mStrokeWidth );
    mCubicVertex[3]->SetRadius( mStrokeWidth );

    mCubicVertex[0]->Set( 0.0f  , 0.0f    );
    mCubicVertex[1]->Set( mWidth, 0.0f    );
    mCubicVertex[2]->Set( mWidth, mHeight );
    mCubicVertex[3]->Set( 0.0f  , mHeight );

    mCubicSegment[0]->GetHandle(0)->Set(  mWidth  * 0.25f,  0.0f           );
    mCubicSegment[0]->GetHandle(1)->Set(  mWidth  * 0.75f,  0.0f           );

    mCubicSegment[1]->GetHandle(0)->Set(  mWidth         , mHeight * 0.25f );
    mCubicSegment[1]->GetHandle(1)->Set(  mWidth         , mHeight * 0.75f );

    mCubicSegment[2]->GetHandle(0)->Set(  mWidth  * 0.75f,  mHeight        );
    mCubicSegment[2]->GetHandle(1)->Set(  mWidth  * 0.25f,  mHeight        );

    mCubicSegment[3]->GetHandle(0)->Set(  0.0f           , mHeight * 0.75f );
    mCubicSegment[3]->GetHandle(1)->Set(  0.0f           , mHeight * 0.25f );

    mCubicSegment[0]->Update();
    mCubicSegment[1]->Update();
    mCubicSegment[2]->Update();
    mCubicSegment[3]->Update();

    // Update bounding box
    mBBox.x =    (-mWidth  * 0.5f ) -  mStrokeWidth;
    mBBox.y =    (-mHeight * 0.5f ) -  mStrokeWidth;
    mBBox.w =  ( ( mWidth  * 0.5f ) +  mStrokeWidth ) * 2;
    mBBox.h =  ( ( mHeight * 0.5f ) +  mStrokeWidth ) * 2;
}

FOdysseyVectorObject*
FOdysseyVectorRectangle::CopyShape()
{
    FOdysseyVectorRectangle* rectangleCopy = new FOdysseyVectorRectangle( mObjectParam.Name
                                                                        , mWidth
                                                                        , mHeight
                                                                        , mStrokeWidth );

    return static_cast<FOdysseyVectorObject*>( rectangleCopy );
}

void
FOdysseyVectorRectangle::DrawShape( uint64 iFlags )
{
    if ( mWidth && mHeight )
    {
        FOdysseyVectorPath::DrawShape ( iFlags );
    }
}

void
FOdysseyVectorRectangle::SetSize( double iWidth, double iHeight )
{
    mWidth  = iWidth;
    mHeight = iHeight;

    Invalidate();
}

double FOdysseyVectorRectangle::GetWidth()
{
    return mWidth;
}

double FOdysseyVectorRectangle::GetHeight()
{
    return mHeight;
}
