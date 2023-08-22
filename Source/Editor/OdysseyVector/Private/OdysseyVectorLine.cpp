#include "OdysseyVectorLine.h"

FOdysseyVectorLine::~FOdysseyVectorLine()
{
   // vertices ans segments freed in OdysseyVectorPath::~destructor
}

FOdysseyVectorLine::FOdysseyVectorLine( const FString iName, double iWidth, double iHeight, double iStrokeWidth )
    : FOdysseyVectorPrimitive( iName )
{
    mStrokeWidth = iStrokeWidth;

    SetSize( iWidth, iHeight );

    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mStrokeWidth );

    mCubicSegment = new FOdysseyVectorSegmentCubic( this, mCubicVertex[0], mCubicVertex[1] );

    AddVertex( mCubicVertex[0] );
    AddVertex( mCubicVertex[1] );
    AddSegment( mCubicSegment );
}

bool
FOdysseyVectorLine::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPath::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorLine::UpdateShape( uint32 iUpdateFlags )
{
    mCubicVertex[0]->SetRadius( mStrokeWidth );
    mCubicVertex[1]->SetRadius( mStrokeWidth );

    mCubicVertex[0]->Set( 0.0f  , 0.0f    );
    mCubicVertex[1]->Set( mWidth, mHeight );

    // a Line is a cubic segment with its handles aligned
    mCubicSegment->GetHandle(0)->Set( mWidth  * 0.25f, mHeight  * 0.25f );
    mCubicSegment->GetHandle(1)->Set( mWidth  * 0.75f, mHeight  * 0.75f );

    mCubicSegment->Update();
}

FOdysseyVectorObject*
FOdysseyVectorLine::CopyShape()
{
    FOdysseyVectorLine* lineCopy = new FOdysseyVectorLine( mObjectParam.Name
                                                         , mWidth
                                                         , mHeight
                                                         , mStrokeWidth );

    return static_cast<FOdysseyVectorObject*>( lineCopy );
}

void
FOdysseyVectorLine::DrawShape( uint64 iFlags )
{
    FOdysseyVectorPath::DrawShape( iFlags );
}

void
FOdysseyVectorLine::SetSize( double iWidth, double iHeight )
{
    mWidth  = iWidth;
    mHeight = iHeight;

    mBBox.x = - mStrokeWidth;
    mBBox.y = - mStrokeWidth;
    mBBox.w = mWidth  + mStrokeWidth;
    mBBox.h = mHeight + mStrokeWidth;

    Invalidate();
}

double FOdysseyVectorLine::GetWidth()
{
    return mWidth;
}

double FOdysseyVectorLine::GetHeight()
{
    return mHeight;
}
