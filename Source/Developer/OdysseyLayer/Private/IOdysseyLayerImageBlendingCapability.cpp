// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyLayerImageBlendingCapability.h"

#include <ULIS3>

//---

IOdysseyLayerImageBlendingCapability::~IOdysseyLayerImageBlendingCapability()
{
}

IOdysseyLayerImageBlendingCapability::IOdysseyLayerImageBlendingCapability(const IOdysseyLayerImageBlendingCapability& iCapability )
    : IOdysseyLayerImageRenderingCapability(iCapability)
    , mBlendingMode(iCapability.mBlendingMode)
    , mOpacity(iCapability.mOpacity)
{
}

IOdysseyLayerImageBlendingCapability::IOdysseyLayerImageBlendingCapability(::ul3::eBlendingMode iBlendingMode, float iOpacity )
    : IOdysseyLayerImageRenderingCapability()
    , mBlendingMode(iBlendingMode)
    , mOpacity(iOpacity)
{
}

//---


::ul3::eBlendingMode
IOdysseyLayerImageBlendingCapability::GetBlendingMode() const
{
    return mBlendingMode;
}

void
IOdysseyLayerImageBlendingCapability::SetBlendingMode( ::ul3::eBlendingMode iBlendingMode )
{
    ::ul3::eBlendingMode oldValue = mBlendingMode;
    mBlendingMode = iBlendingMode;
    mBlendingModeChangedDelegate.Broadcast(oldValue);
	ImageResultChangedDelegate().Broadcast();
}

void
IOdysseyLayerImageBlendingCapability::SetBlendingMode( FText iBlendingMode )
{
    const int max = static_cast< int >( ::ul3::NUM_BLENDING_MODES );
    for( uint8 i = 0; i < max; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[i] ) );
        if( iBlendingMode.EqualTo( entry ) )
        {
            SetBlendingMode( static_cast<::ul3::eBlendingMode>( i ) );
            return;
        }
    }
}

FText
IOdysseyLayerImageBlendingCapability::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
}

float
IOdysseyLayerImageBlendingCapability::GetOpacity() const
{
    return mOpacity;
}

void
IOdysseyLayerImageBlendingCapability::SetOpacity( float iOpacity )
{
    if( iOpacity < 0.f || iOpacity > 1.f )
        return;

    float oldValue = mOpacity;
    mOpacity = iOpacity;
    mOpacityChangedDelegate.Broadcast(oldValue);
	ImageResultChangedDelegate().Broadcast();
}

void
IOdysseyLayerImageBlendingCapability::SerializeImageBlendingCapability(FArchive &Ar)
{
    int bm = static_cast< int >( mBlendingMode );
    Ar << bm;
    Ar << mOpacity;
    
    if( Ar.IsLoading() )
    {
        mBlendingMode = (::ul3::eBlendingMode)bm;
    }
}

IOdysseyLayerImageBlendingCapability::FOdysseyLayerBlendingModeChanged&
IOdysseyLayerImageBlendingCapability::BlendingModeChangedDelegate()
{
    return mBlendingModeChangedDelegate;
}

IOdysseyLayerImageBlendingCapability::FOdysseyLayerOpacityChanged&
IOdysseyLayerImageBlendingCapability::OpacityChangedDelegate()
{
    return mOpacityChangedDelegate;
}

const FGuid&
IOdysseyLayerImageBlendingCapability::GetGuid()
{
    static FGuid guid = FGuid::NewGuid();
    return guid;
}

TArray<FGuid>
IOdysseyLayerImageBlendingCapability::GetGuids()
{
    TArray<FGuid> guids;
    guids.Add(GetGuid());
    guids.Append(IOdysseyLayerImageRenderingCapability::GetGuids());
    return guids;
}

TArray< TSharedPtr< FText > >
IOdysseyLayerImageBlendingCapability::GetBlendingModesAsText()
{
    TArray< TSharedPtr< FText > > array;
    const int max = static_cast< int >( ::ul3::NUM_BLENDING_MODES );
    for( int i = 0; i < max; ++i )
        array.Add(MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[i] ) ) ) );

    return  array;
}