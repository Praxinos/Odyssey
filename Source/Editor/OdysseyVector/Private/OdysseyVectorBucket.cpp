#include "OdysseyVectorBucket.h"
#include "Palette/OdysseyPaletteEntryColor.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846L
#endif

FOdysseyVectorBucket::~FOdysseyVectorBucket()
{
}

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject& iParent )
    : mParent ( iParent )
    , mSolidColor( 128, 128, 128, 255 )
    , mRotation( 0.0f )
    , mPropagated( false )
    , mIsGradient ( false )
{
    SetCoords( 0.0, 0.0f );
}

FOdysseyVectorBucket::FOdysseyVectorBucket( FOdysseyVectorObject& iParent, double iX, double iY, bool iPropagated )
    : mParent ( iParent )
    , mPropagated( iPropagated )
    , mIsGradient ( false )
{
    SetCoords( iX, iY );
    SetSolidColor( 128, 128, 128, 255 );
    SetRotation( 0.0f );
}

void 
FOdysseyVectorBucket::SetRotation( double iRotation )
{
    mRotation = iRotation;
}

void 
FOdysseyVectorBucket::SetX( double iX )
{
    FOdysseyVectorPoint::SetX( iX );

    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void 
FOdysseyVectorBucket::SetY( double iY )
{
    FOdysseyVectorPoint::SetY( iY );

    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void 
FOdysseyVectorBucket::Set( double iX, double iY )
{
    FOdysseyVectorPoint::Set( iX, iY );

    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorBucket::Set( double iX, double iY, double iRadius )
{
    FOdysseyVectorPoint::Set( iX, iY, iRadius );

    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorBucket::Invalidate()
{
    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

void
FOdysseyVectorBucket::SetPropagated( bool iPropagated )
{
    mPropagated = iPropagated;

    mParent.Invalidate( FOdysseyVectorObject::INVALIDATE_COLOR );
}

bool
FOdysseyVectorBucket::IsPropagated()
{
    return mPropagated;
}

bool
FOdysseyVectorBucket::IsGradient()
{
    return mIsGradient;
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
FOdysseyVectorBucket::SetGradient( bool iIsGradient )
{
    mIsGradient = iIsGradient;
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

FColor&
FOdysseyVectorBucket::GetColor()
{
    if( mPaletteEntry )
    {
        return Cast< UOdysseyPaletteEntryColor >( mPaletteEntry )->GetUsedColor();
    }

    return mSolidColor;
}

double
FOdysseyVectorBucket::GetRotation()
{
    return mRotation;
}

void
FOdysseyVectorBucket::Copy( FOdysseyVectorBucket* iDestinationBucket )
{
    iDestinationBucket->mCoords = mCoords;
    iDestinationBucket->mSolidColor = mSolidColor;
    iDestinationBucket->mRotation = mRotation;
    iDestinationBucket->mPropagated = mPropagated;
    iDestinationBucket->mIsGradient = mIsGradient;
    iDestinationBucket->mGradientColor0 = mGradientColor0;
    iDestinationBucket->mGradientColor1 = mGradientColor1;
    iDestinationBucket->mPaletteEntry = mPaletteEntry;

    //iDestinationBucket->Invalidate();
}

FOdysseyVectorObject&
FOdysseyVectorBucket::GetParent()
{
    return mParent;
}

void
FOdysseyVectorBucket::SetPaletteEntry( UOdysseyPaletteEntry* iPaletteEntry )
{
    if( iPaletteEntry )
    {
        mPaletteEntryDescription.EntryId = iPaletteEntry->GetFName();
        mPaletteEntryDescription.UsedSet = 1;
    }
    else
    {
        mPaletteEntryDescription.EntryId = FName(TEXT(""));
        mPaletteEntryDescription.UsedSet = 0;
    }

    mPaletteEntry = iPaletteEntry;
}

FPaletteEntryDescription&
FOdysseyVectorBucket::GetPaletteEntryDescription()
{
    return mPaletteEntryDescription;
}

UOdysseyPaletteEntry*
FOdysseyVectorBucket::GetPaletteEntry()
{
    return mPaletteEntry;
}
