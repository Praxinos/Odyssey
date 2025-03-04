// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyVectorBucket.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"
#include "Palette/OdysseyPalette.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject* iOwner
                                          , FOdysseyVectorBucket* iImportFrom )
    : mOwner ( iOwner )
    , mLinearP0( 0.0f, 0.0f )
    , mLinearP1( 0.0f, 0.0f )
{
    BLMatrix2D& ownerInverseWorldMatrix = iOwner->GetInverseWorldMatrix();
    BLMatrix2D& importWorldMatrix = iImportFrom->GetOwner()->GetWorldMatrix();
    BLMatrix2D conversionMatrix;
    BLPoint ownerSpaceCoords;

    FOdysseyVector::MatrixMultiply( ownerInverseWorldMatrix, importWorldMatrix, conversionMatrix );

    // copy everything
    iImportFrom->Copy( this );

    // alter position
    ownerSpaceCoords = conversionMatrix.mapPoint( iImportFrom->GetX(), iImportFrom->GetY() );
    Set( ownerSpaceCoords.x, ownerSpaceCoords.y );
}

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject* iOwner
                                          , double iX
                                          , double iY
                                          , bool iPropagated )
    : FOdysseyVectorPoint( iX, iY )
    , mOwner ( iOwner )
    , bSelected( false )
    , mPaletteEntry( nullptr )
    , mPaletteSet( 0 )
{
    SetSolidColor( 160, 160, 160, 255 );

    SetRotation( 0.0f );
    SetPropagated( false );
    SetGradientColor0( 255, 255, 255, 255 );
    SetGradientColor1( 255, 255, 255, 255 );
    SetColorMode( eBucketColorMode::SolidColor );
    SetRadialRadius( 80.0f );
    SetRadialOffset( ::ULIS::FVec2D( 0.0f, 0.0f ) );
}

void
FOdysseyVectorBucket::Invalidate()
{
    if( mOwner ) // owner can be nullptr (when copy pasting a bucket)
    {
        mOwner->Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
    }
}

double
FOdysseyVectorBucket::GetRadialRadius()
{
    return mRadialRadius;
}

void
FOdysseyVectorBucket::SetRadialRadius( double iRadialRadius )
{
    mRadialRadius = iRadialRadius;
}

::ULIS::FVec2D&
FOdysseyVectorBucket::GetRadialOffset()
{
    return mRadialOffset;
}

void
FOdysseyVectorBucket::SetRadialOffset( const ::ULIS::FVec2D& iRadialOffset )
{
    mRadialOffset = iRadialOffset;
}

eBucketColorMode
FOdysseyVectorBucket::GetColorMode()
{
    return mColorMode;
}

void
FOdysseyVectorBucket::SetColorMode( eBucketColorMode iColorMode )
{
    mColorMode = iColorMode;

    if( mColorMode == eBucketColorMode::LinearGradient )
    {
        if( mLinearP0 == mLinearP1 )
        {
            ::ULIS::FRectD bbox = GetOwner()->GetBBox( false, false );

            mLinearP0 = ::ULIS::FVec2D(   bbox.x           , mCoords.y );
            mLinearP1 = ::ULIS::FVec2D( ( bbox.x + bbox.w ), mCoords.y );
        }
    }
}

void
FOdysseyVectorBucket::SetRotation( double iRotation )
{
    mLinearRotation = fmod( iRotation, M_PI * 2.0f );

    if( mLinearRotation < 0.0f )
    {
        mLinearRotation += ( M_PI * 2.0f );
    }
}

void
FOdysseyVectorBucket::SetLinearP0( const ::ULIS::FVec2D& iP0 )
{
    mLinearP0 = iP0;
}

void
FOdysseyVectorBucket::SetLinearP1( const ::ULIS::FVec2D& iP1 )
{
    mLinearP1 = iP1;
}

::ULIS::FVec2D
FOdysseyVectorBucket::GetLinearP0()
{
    return mLinearP0;
}

::ULIS::FVec2D
FOdysseyVectorBucket::GetLinearP1()
{
    return mLinearP1;
}

bool
FOdysseyVectorBucket::IsSelected()
{
    return bSelected;
}

void
FOdysseyVectorBucket::SetSelected( bool iIsSelected )
{
    bSelected = iIsSelected;
}

void
FOdysseyVectorBucket::SetCoords( double iX, double iY )
{
    FOdysseyVectorPoint::SetCoords( iX, iY );

    Invalidate();
}

void
FOdysseyVectorBucket::SetPropagated( bool iPropagated )
{
    bPropagated = iPropagated;

    Invalidate();
}

bool
FOdysseyVectorBucket::IsPropagated()
{
    return bPropagated;
}

FColor&
FOdysseyVectorBucket::GetGradientColor0()
{
    return mGradientColor0;
}

FColor&
FOdysseyVectorBucket::GetGradientColor1()
{
    return mGradientColor1;
}

void
FOdysseyVectorBucket::SetGradientColor0( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0 )
{
    mGradientColor0.R = iR0;
    mGradientColor0.G = iG0;
    mGradientColor0.B = iB0;
    mGradientColor0.A = iA0;

    Invalidate();
}

void
FOdysseyVectorBucket::SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 )
{
    mGradientColor1.R = iR1;
    mGradientColor1.G = iG1;
    mGradientColor1.B = iB1;
    mGradientColor1.A = iA1;

    Invalidate();
}

void
FOdysseyVectorBucket::SetGradientColor0( FColor& iColor )
{
    mGradientColor0 = iColor;

    Invalidate();
}

void
FOdysseyVectorBucket::SetGradientColor1( FColor& iColor )
{
    mGradientColor1 = iColor;

    Invalidate();
}

void
FOdysseyVectorBucket::SetSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mSolidColor.R = iR;
    mSolidColor.G = iG;
    mSolidColor.B = iB;
    mSolidColor.A = iA;

    Invalidate();
}

void
FOdysseyVectorBucket::SetSolidColor( FColor& iSolidColor )
{
    mSolidColor = iSolidColor;

    Invalidate();
}

FColor&
FOdysseyVectorBucket::GetSolidColor()
{
    return mSolidColor;
}

FColor
FOdysseyVectorBucket::GetColor()
{
    switch ( mColorMode )
    {
        case eBucketColorMode::Palette:
        if( mPaletteEntry && mPaletteEntry->IsValidLowLevel() )
        {
            UOdysseyPalette* palette = mPaletteEntry->GetPalette();
            return Cast< UOdysseyPaletteEntryColor >(mPaletteEntry)->GetColor(mPaletteSet);
        }
        else
        {
            mPaletteEntry = nullptr;
            mPaletteSet = 0;
            return mSolidColor;
        }
        case eBucketColorMode::SolidColor:
        return mSolidColor;
    }

    return FColor(0,0,0,255);
}

double
FOdysseyVectorBucket::GetRotation()
{
    return mLinearRotation;
}

void
FOdysseyVectorBucket::Copy( FOdysseyVectorBucket* iDestinationBucket )
{
    iDestinationBucket->mCoords          = mCoords;

    iDestinationBucket->mPaletteEntry    = mPaletteEntry;
    iDestinationBucket->mPaletteSet      = mPaletteSet;
    iDestinationBucket->mColorMode       = mColorMode;
    iDestinationBucket->mSolidColor      = mSolidColor;
    iDestinationBucket->mLinearRotation        = mLinearRotation;
    iDestinationBucket->bPropagated      = bPropagated;
    iDestinationBucket->mColorMode       = mColorMode;
    iDestinationBucket->mGradientColor0  = mGradientColor0;
    iDestinationBucket->mGradientColor1  = mGradientColor1;
    iDestinationBucket->mRadialRadius    = mRadialRadius;
    iDestinationBucket->mRadialOffset    = mRadialOffset;

    iDestinationBucket->Invalidate();
}

FOdysseyVectorObject*
FOdysseyVectorBucket::GetOwner()
{
    return mOwner;
}

void
FOdysseyVectorBucket::SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry )
{
    mPaletteEntry = iPaletteEntry;
}

UOdysseyPaletteEntry*
FOdysseyVectorBucket::GetPaletteEntry()
{
    return mPaletteEntry;
}

void
FOdysseyVectorBucket::SetPaletteSet( int iPaletteSet )
{
    mPaletteSet = iPaletteSet;
}

int
FOdysseyVectorBucket::GetPaletteSet()
{
    return mPaletteSet;
}
