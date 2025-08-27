// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

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

    mCubicVertex[0] = new FOdysseyVectorVertex( 0.0f, 0.0f, mStrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( 0.0f, 0.0f, mStrokeWidth );

    mCubicSegment = new FOdysseyVectorSegmentCubic( this, mCubicVertex[0], mCubicVertex[1], true );

    AddVertex( mCubicVertex[0] );
    AddVertex( mCubicVertex[1] );
    AddSegment( mCubicSegment );

    UpdateShape( 0 );
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

    // a Line is a cubic segment with its handles at vertex coordinates
    mCubicSegment->GetHandle(0)->Set( mCubicVertex[0]->GetCoords() );
    mCubicSegment->GetHandle(1)->Set( mCubicVertex[1]->GetCoords() );

    //mCubicSegment->Update( nullptr );
    FOdysseyVectorPath::UpdateShape( iUpdateFlags );
}

FOdysseyVectorObject*
FOdysseyVectorLine::CopyShape( uint64 iCopyFlags )
{
    FOdysseyVectorLine* lineCopy = new FOdysseyVectorLine( mName
                                                         , mWidth
                                                         , mHeight
                                                         , mStrokeWidth );

    return static_cast<FOdysseyVectorObject*>( lineCopy );
}

void
FOdysseyVectorLine::DrawShape( BLContext* iBLContext
                             , FOdysseyVectorEngine* iEngine
                             , const ::ULIS::FRectD& iInvalidationArea
                             , double iHierarchyOpacity
                             , uint64 iFlags )
{
    FOdysseyVectorPath::DrawShape( iBLContext
                                 , iEngine
                                 , iInvalidationArea
                                 , iHierarchyOpacity
                                 , iFlags );
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

    Invalidate( FOdysseyVectorObjectInvalidationFlags().Set(FOdysseyVectorObjectInvalidationFlags::SHAPE) );
}

double FOdysseyVectorLine::GetWidth()
{
    return mWidth;
}

double FOdysseyVectorLine::GetHeight()
{
    return mHeight;
}
