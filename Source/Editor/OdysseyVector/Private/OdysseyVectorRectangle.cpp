#include "OdysseyVectorRectangle.h"

FOdysseyVectorRectangle::~FOdysseyVectorRectangle()
{
}

FOdysseyVectorRectangle::FOdysseyVectorRectangle()
    : mStrokeWidth ( 4.0f )
{
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

void FOdysseyVectorRectangle::Init( std::string iName, double iWidth, double iHeight )
{
    SetName( iName );
    SetSize( iWidth, iHeight );
}

FOdysseyVectorObject*
FOdysseyVectorRectangle::CopyShape()
{
    FOdysseyVectorRectangle* rectangleCopy = new FOdysseyVectorRectangle();

    rectangleCopy->Init( Name, mWidth, mHeight );

    return static_cast<FOdysseyVectorObject*>( rectangleCopy );
}

void
FOdysseyVectorRectangle::DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags )
{
    BLContext* blctx = GetScene()->GetEngine()->GetBLContext();
    BLRgba32 strokeColor;

    strokeColor.r = mObjectParam.Foreground.R;
    strokeColor.g = mObjectParam.Foreground.G;
    strokeColor.b = mObjectParam.Foreground.B;
    strokeColor.a = mObjectParam.Foreground.A;

    blctx->setCompOp(BL_COMP_OP_SRC_COPY);
/*
    if( Filled )
    {
        BLRgba32 fillColor;

        fillColor.r = Background.R;
        fillColor.g = Background.G;
        fillColor.b = Background.B;
        fillColor.a = Background.A;

        blctx->setFillStyle( fillColor );
        blctx->fillRoundRect( -mWidth * 0.5f, -mHeight * 0.5f, mWidth, mHeight, 0.0f, 0.0f );
    }
*/
    blctx->setStrokeStyle ( strokeColor );
    blctx->setStrokeWidth ( mStrokeWidth );
    blctx->strokeRoundRect( -mWidth * 0.5f, -mHeight * 0.5f, mWidth, mHeight, 0.0f, 0.0f );
}

bool
FOdysseyVectorRectangle::PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags )
{
    /*double x1 = - mWidth  * 0.5f;
    double y1 = - mHeight * 0.5f;
    double x2 = x1 + mWidth;
    double y2 = y1 + mHeight;

    if( ( iX >= x1 ) && ( iX <= x2 ) &&
        ( iY >= y1 ) && ( iY <= y2 ) )
    {
        return this;
    }*/

    return false;
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
