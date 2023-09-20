#include "OdysseyVectorBucket.h"
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

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject* iOwner, double iX, double iY, bool iPropagated )
    : FOdysseyVectorPoint( iX, iY, 0.0f )
    , mOwner ( iOwner )
    , mIsSelected( false )
{
    SetSolidColor( 128, 128, 128, 255 );

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
    return mBucketParam.RadialRadius;
}

void
FOdysseyVectorBucket::SetRadialRadius( double iRadialRadius )
{
    mBucketParam.RadialRadius = iRadialRadius;
}

::ULIS::FVec2D&
FOdysseyVectorBucket::GetRadialOffset()
{
    return mBucketParam.RadialOffset;
}

void
FOdysseyVectorBucket::SetRadialOffset( const ::ULIS::FVec2D& iRadialOffset )
{
    mBucketParam.RadialOffset = iRadialOffset;
}

eBucketColorMode
FOdysseyVectorBucket::GetColorMode()
{
    return mBucketParam.ColorMode;
}

void
FOdysseyVectorBucket::SetColorMode( eBucketColorMode iColorMode )
{
    mBucketParam.ColorMode = iColorMode;
}

eBucketSpreadingPolicy
FOdysseyVectorBucket::GetSpreadingPolicy()
{
    return mBucketParam.SpreadingPolicy;
}

void
FOdysseyVectorBucket::SetSpreadingPolicy( eBucketSpreadingPolicy iSpreadingPolicy )
{
    mBucketParam.SpreadingPolicy = iSpreadingPolicy;
}

void 
FOdysseyVectorBucket::SetRotation( double iRotation )
{
    mBucketParam.Rotation = fmod( iRotation, M_PI * 2.0f );

    if( mBucketParam.Rotation < 0.0f )
    {
        mBucketParam.Rotation += ( M_PI * 2.0f );
    }
}

bool
FOdysseyVectorBucket::IsSelected()
{
    return mIsSelected;
}

void
FOdysseyVectorBucket::SetSelected( bool iIsSelected )
{
    mIsSelected = iIsSelected;
}

void
FOdysseyVectorBucket::SetCoords( double iX, double iY, double iRadius )
{
    FOdysseyVectorPoint::SetCoords( iX, iY, iRadius );

    Invalidate();
}

void
FOdysseyVectorBucket::SetPropagated( bool iPropagated )
{
    mBucketParam.Propagated = iPropagated;

    Invalidate();
}

bool
FOdysseyVectorBucket::IsPropagated()
{
    return mBucketParam.Propagated;
}

FColor&
FOdysseyVectorBucket::GetGradientColor0()
{
    return mBucketParam.GradientColor0;
}

FColor&
FOdysseyVectorBucket::GetGradientColor1()
{
    return mBucketParam.GradientColor1;
}

void
FOdysseyVectorBucket::SetGradientColor0( uint8 iR0, uint8 iG0, uint8 iB0, uint8 iA0 )
{
    mBucketParam.GradientColor0.R = iR0;
    mBucketParam.GradientColor0.G = iG0;
    mBucketParam.GradientColor0.B = iB0;
    mBucketParam.GradientColor0.A = iA0;
}

void
FOdysseyVectorBucket::SetGradientColor1( uint8 iR1, uint8 iG1, uint8 iB1, uint8 iA1 )
{
    mBucketParam.GradientColor1.R = iR1;
    mBucketParam.GradientColor1.G = iG1;
    mBucketParam.GradientColor1.B = iB1;
    mBucketParam.GradientColor1.A = iA1;
}

void
FOdysseyVectorBucket::SetGradientColor0( FColor& iColor )
{
    mBucketParam.GradientColor0 = iColor;
}

void
FOdysseyVectorBucket::SetGradientColor1( FColor& iColor )
{
    mBucketParam.GradientColor1 = iColor; 
}

void
FOdysseyVectorBucket::SetSolidColor( uint8 iR, uint8 iG, uint8 iB, uint8 iA )
{
    mBucketParam.SolidColor.R = iR;
    mBucketParam.SolidColor.G = iG;
    mBucketParam.SolidColor.B = iB;
    mBucketParam.SolidColor.A = iA;
}

void
FOdysseyVectorBucket::SetSolidColor( FColor& iSolidColor )
{
    mBucketParam.SolidColor = iSolidColor;
}

FColor&
FOdysseyVectorBucket::GetSolidColor()
{
    return mBucketParam.SolidColor;
}

FColor
FOdysseyVectorBucket::GetColor()
{
    switch ( mBucketParam.ColorMode )
    {
        case eBucketColorMode::Palette:
        return ( mBucketParam.PaletteEntry ) ? Cast< UOdysseyPaletteEntryColor >( mBucketParam.PaletteEntry )->GetUsedColor() : mBucketParam.SolidColor;

        case eBucketColorMode::SolidColor:
        return mBucketParam.SolidColor;
    }

    return FColor(0,0,0,255);
}

double
FOdysseyVectorBucket::GetRotation()
{
    return mBucketParam.Rotation;
}

void
FOdysseyVectorBucket::Copy( FOdysseyVectorBucket* iDestinationBucket )
{
    iDestinationBucket->mCoords                      = mCoords;

    iDestinationBucket->mBucketParam.PaletteEntry    = mBucketParam.PaletteEntry;
    iDestinationBucket->mBucketParam.ColorMode       = mBucketParam.ColorMode;
    iDestinationBucket->mBucketParam.SpreadingPolicy = mBucketParam.SpreadingPolicy;
    iDestinationBucket->mBucketParam.SolidColor      = mBucketParam.SolidColor;
    iDestinationBucket->mBucketParam.Rotation        = mBucketParam.Rotation;
    iDestinationBucket->mBucketParam.Propagated      = mBucketParam.Propagated;
    iDestinationBucket->mBucketParam.ColorMode       = mBucketParam.ColorMode;
    iDestinationBucket->mBucketParam.GradientColor0  = mBucketParam.GradientColor0;
    iDestinationBucket->mBucketParam.GradientColor1  = mBucketParam.GradientColor1;
    iDestinationBucket->mBucketParam.RadialRadius    = mBucketParam.RadialRadius;
    iDestinationBucket->mBucketParam.RadialOffset    = mBucketParam.RadialOffset;

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
    mBucketParam.PaletteEntry = iPaletteEntry;
}

UOdysseyPaletteEntry*
FOdysseyVectorBucket::GetPaletteEntry()
{
    return mBucketParam.PaletteEntry;
}
