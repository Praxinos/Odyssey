#include "OdysseyVectorBucket.h"
#include "OdysseyVector.h"
#include "OdysseyVectorObject.h"
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
{
    SetSolidColor( 160, 160, 160, 255 );

    SetRotation( 0.0f );
    SetPropagated( false );
    SetGradientColor0( 255, 255, 255, 255 );
    SetGradientColor1( 255, 255, 255, 255 );
    SetColorMode( eBucketColorMode::SolidColor );
    SetSpreadingPolicy( eBucketSpreadingPolicy::Group );
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
}

eBucketSpreadingPolicy
FOdysseyVectorBucket::GetSpreadingPolicy()
{
    return mSpreadingPolicy;
}

void
FOdysseyVectorBucket::SetSpreadingPolicy( eBucketSpreadingPolicy iSpreadingPolicy )
{
    mSpreadingPolicy = iSpreadingPolicy;
}

void
FOdysseyVectorBucket::SetRotation( double iRotation )
{
    mRotation = fmod( iRotation, M_PI * 2.0f );

    if( mRotation < 0.0f )
    {
        mRotation += ( M_PI * 2.0f );
    }
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
}

void
FOdysseyVectorBucket::SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 )
{
    mGradientColor1.R = iR1;
    mGradientColor1.G = iG1;
    mGradientColor1.B = iB1;
    mGradientColor1.A = iA1;
}

void
FOdysseyVectorBucket::SetGradientColor0( FColor& iColor )
{
    mGradientColor0 = iColor;
}

void
FOdysseyVectorBucket::SetGradientColor1( FColor& iColor )
{
    mGradientColor1 = iColor;
}

void
FOdysseyVectorBucket::SetSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mSolidColor.R = iR;
    mSolidColor.G = iG;
    mSolidColor.B = iB;
    mSolidColor.A = iA;
}

void
FOdysseyVectorBucket::SetSolidColor( FColor& iSolidColor )
{
    mSolidColor = iSolidColor;
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
            return Cast< UOdysseyPaletteEntryColor >(mPaletteEntry)->GetUsedColor();
        else
        {
            mPaletteEntry = nullptr;
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
    return mRotation;
}

void
FOdysseyVectorBucket::Copy( FOdysseyVectorBucket* iDestinationBucket )
{
    iDestinationBucket->mCoords                      = mCoords;

    iDestinationBucket->mPaletteEntry    = mPaletteEntry;
    iDestinationBucket->mColorMode       = mColorMode;
    iDestinationBucket->mSpreadingPolicy = mSpreadingPolicy;
    iDestinationBucket->mSolidColor      = mSolidColor;
    iDestinationBucket->mRotation        = mRotation;
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
