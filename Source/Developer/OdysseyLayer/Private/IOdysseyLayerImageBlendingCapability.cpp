// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "IOdysseyLayerImageBlendingCapability.h"

#include <ULIS>

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

IOdysseyLayerImageBlendingCapability::IOdysseyLayerImageBlendingCapability(::ULIS::eBlendMode iBlendingMode, float iOpacity )
    : IOdysseyLayerImageRenderingCapability()
    , mBlendingMode(iBlendingMode)
    , mOpacity(iOpacity)
{
}

//---


::ULIS::eBlendMode
IOdysseyLayerImageBlendingCapability::GetBlendingMode() const
{
    return mBlendingMode;
}

void
IOdysseyLayerImageBlendingCapability::SetBlendingMode( ::ULIS::eBlendMode iBlendingMode )
{
    ::ULIS::eBlendMode oldValue = mBlendingMode;
    mBlendingMode = iBlendingMode;
    mBlendingModeChangedDelegate.Broadcast(oldValue);
	ImageResultChangedDelegate().Broadcast( nullptr, 0 );
}

void
IOdysseyLayerImageBlendingCapability::SetBlendingMode( FText iBlendingMode )
{
    const int max = static_cast< int >( ::ULIS::NumBlendModes );
    for( uint8 i = 0; i < max; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[i] ) );
        if( iBlendingMode.EqualTo( entry ) )
        {
            SetBlendingMode( static_cast<::ULIS::eBlendMode>( i ) );
            return;
        }
    }
}

FText
IOdysseyLayerImageBlendingCapability::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[static_cast<int>( mBlendingMode )] ) );
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
    mOpacityChangedDelegate.Broadcast( oldValue );
    ImageResultChangedDelegate().Broadcast( nullptr, 0 );
}

void
IOdysseyLayerImageBlendingCapability::SerializeImageBlendingCapability(FArchive &Ar)
{
    int bm = static_cast< int >( mBlendingMode );
    Ar << bm;
    Ar << mOpacity;
    
    if( Ar.IsLoading() )
    {
        mBlendingMode = (::ULIS::eBlendMode)bm;
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
    const int max = static_cast< int >( ::ULIS::NumBlendModes );
    for( int i = 0; i < max; ++i )
        array.Add(MakeShared< FText >( FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendMode[i] ) ) ) );

    return  array;
}