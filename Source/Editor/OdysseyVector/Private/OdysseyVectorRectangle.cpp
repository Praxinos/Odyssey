#include "OdysseyVectorRectangle.h"

FOdysseyVectorRectangle::~FOdysseyVectorRectangle()
{
}

FOdysseyVectorRectangle::FOdysseyVectorRectangle( std::string iName )
    : FOdysseyVectorObject( iName )
    , mWidth( 0 )
    , mHeight( 0 )
{
}

FOdysseyVectorRectangle::FOdysseyVectorRectangle( std::string iName, double iWidth, double iHeight )
    : FOdysseyVectorObject( iName )
    , mWidth( iWidth )
    , mHeight( iHeight )
{
}

FOdysseyVectorObject*
FOdysseyVectorRectangle::CopyShape()
{
    FOdysseyVectorRectangle* rectangleCopy = new FOdysseyVectorRectangle ( mName, mWidth, mHeight );

    return Cast<FOdysseyVectorObject*>( rectangleCopy );
}

void
FOdysseyVectorRectangle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext& blctx = FOdysseyVectorEngine::GetBLContext();

    blctx.setCompOp(BL_COMP_OP_SRC_COPY);

    if( mIsFilled )
    {
        blctx.setFillStyle( BLRgba32( mFillColor ) );
        blctx.fillRoundRect( -mWidth * 0.5f, -mHeight * 0.5f, mWidth, mHeight, 0.0f, 0.0f );
    }

    blctx.setStrokeStyle ( BLRgba32( mStrokeColor ) );
    blctx.setStrokeWidth ( mStrokeWidth );
    blctx.strokeRoundRect( -mWidth * 0.5f, -mHeight * 0.5f, mWidth, mHeight, 0.0f, 0.0f );
}

FOdysseyVectorObject*
FOdysseyVectorRectangle::PickShape( double iX, double iY, double iRadius )
{
    double x1 = - mWidth  * 0.5f;
    double y1 = - mHeight * 0.5f;
    double x2 = x1 + mWidth;
    double y2 = y1 + mHeight;

    if( ( iX >= x1 ) && ( iX <= x2 ) &&
        ( iY >= y1 ) && ( iY <= y2 ) )
    {
        return this;
    }

    return nullptr;
}

void
FOdysseyVectorRectangle::SetSize( double iWidth, double iHeight )
{
    mWidth  = iWidth;
    mHeight = iHeight;

    mBBox.x = (-mWidth * 0.5f ) - mStrokeWidth;
    mBBox.y = (-mHeight * 0.5f ) - mStrokeWidth;
    mBBox.w =  ( ( mWidth * 0.5f ) +  mStrokeWidth ) * 2;
    mBBox.h =  ( ( mHeight * 0.5f ) +  mStrokeWidth ) * 2;
}

double FOdysseyVectorRectangle::GetWidth()
{
    return mWidth;
}

double FOdysseyVectorRectangle::GetHeight()
{
    return mHeight;
}
