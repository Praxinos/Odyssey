#include "OdysseyVectorLine.h"

FOdysseyVectorLine::~FOdysseyVectorLine()
{
    delete mCubicVertex[0];
    delete mCubicVertex[1];

    delete mCubicSegment;
}

FOdysseyVectorLine::FOdysseyVectorLine( const FString iName, double iWidth, double iHeight, double iStrokeWidth )
    : FOdysseyVectorPrimitive( iName )
{
    mLineParam.StrokeWidth = iStrokeWidth;

    Init( iName, iWidth, iHeight );
}

bool
FOdysseyVectorLine::HasBaseClass( uint32 iBaseClassID )
{
    if( mStaticClass == iBaseClassID )
    {
        return true;
    }

    return FOdysseyVectorPathCubic::HasBaseClass( iBaseClassID );
}

void
FOdysseyVectorLine::Init( const FString& iName, double iWidth, double iHeight )
{
    SetName( iName );
    SetSize( iWidth, iHeight );

    mCubicVertex[0] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mLineParam.StrokeWidth );
    mCubicVertex[1] = new FOdysseyVectorVertex( this, 0.0f, 0.0f, mLineParam.StrokeWidth );

    mCubicSegment = new FOdysseyVectorSegmentCubic( static_cast<FOdysseyVectorPathCubic*>(this), mCubicVertex[0], mCubicVertex[1] );

    AddVertex ( mCubicVertex[0] );
    AddVertex ( mCubicVertex[1] );

    AddSegment ( mCubicSegment );
}

void
FOdysseyVectorLine::UpdateShape( uint32 iUpdateFlags )
{
    mCubicVertex[0]->SetRadius( mLineParam.StrokeWidth );
    mCubicVertex[1]->SetRadius( mLineParam.StrokeWidth );

    mCubicVertex[0]->Set( 0.0f            , 0.0f              );
    mCubicVertex[1]->Set( mLineParam.Width, mLineParam.Height );

    mCubicSegment->GetHandle(0)->Set( mLineParam.Width  * 0.25f, mLineParam.Height  * 0.25f );
    mCubicSegment->GetHandle(1)->Set( mLineParam.Width  * 0.75f, mLineParam.Height  * 0.75f );

    mCubicSegment->Update();
}

FOdysseyVectorObject*
FOdysseyVectorLine::CopyShape()
{
    FOdysseyVectorLine* lineCopy = new FOdysseyVectorLine( mObjectParam.Name
                                                         , mLineParam.Width
                                                         , mLineParam.Height
                                                         , mLineParam.StrokeWidth );

    return static_cast<FOdysseyVectorObject*>( lineCopy );
}

FOdysseyVectorPathCubic*
FOdysseyVectorLine::Convert()
{
    FOdysseyVectorPathCubic* path = static_cast<FOdysseyVectorPathCubic*>(this->FOdysseyVectorPathCubic::CopyShape());

    this->CopySettings( *path );

    return path;
}

void
FOdysseyVectorLine::DrawShape( uint64 iFlags )
{
    if ( mLineParam.Width && mLineParam.Height )
    {
        FOdysseyVectorPathCubic::DrawShape ( iFlags );
    }
}

bool
FOdysseyVectorLine::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    return false;
}

void
FOdysseyVectorLine::SetSize( double iWidth, double iHeight )
{
    mLineParam.Width  = iWidth;
    mLineParam.Height = iHeight;

    mBBox.x = - mLineParam.StrokeWidth;
    mBBox.y = - mLineParam.StrokeWidth;
    mBBox.w = mLineParam.Width  + mLineParam.StrokeWidth;
    mBBox.h = mLineParam.Height + mLineParam.StrokeWidth;

    Invalidate();
}

double FOdysseyVectorLine::GetWidth()
{
    return mLineParam.Width;
}

double FOdysseyVectorLine::GetHeight()
{
    return mLineParam.Height;
}
