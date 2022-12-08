#include "OdysseyVectorRectangle.h"

UOdysseyVectorRectangle::~UOdysseyVectorRectangle()
{
}

void UOdysseyVectorRectangle::Init( std::string iName, double iWidth, double iHeight )
{
    SetName( iName );
    SetSize( iWidth, iHeight );
}

UOdysseyVectorObject*
UOdysseyVectorRectangle::CopyShape()
{
    UOdysseyVectorRectangle* rectangleCopy = NewObject<UOdysseyVectorRectangle>();

    rectangleCopy->Init( mName, mWidth, mHeight );

    return Cast<UOdysseyVectorObject>( rectangleCopy );
}

void
UOdysseyVectorRectangle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
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

UOdysseyVectorObject*
UOdysseyVectorRectangle::PickShape( double iX, double iY, double iRadius )
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
UOdysseyVectorRectangle::SetSize( double iWidth, double iHeight )
{
    mWidth  = iWidth;
    mHeight = iHeight;

    mBBox.x = (-mWidth * 0.5f ) - mStrokeWidth;
    mBBox.y = (-mHeight * 0.5f ) - mStrokeWidth;
    mBBox.w =  ( ( mWidth * 0.5f ) +  mStrokeWidth ) * 2;
    mBBox.h =  ( ( mHeight * 0.5f ) +  mStrokeWidth ) * 2;
}

double UOdysseyVectorRectangle::GetWidth()
{
    return mWidth;
}

double UOdysseyVectorRectangle::GetHeight()
{
    return mHeight;
}
