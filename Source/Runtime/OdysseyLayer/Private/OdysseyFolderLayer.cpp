// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyFolderLayer.h"

#define LOCTEXT_NAMESPACE "OdysseyFolderLayer"

//---

FOdysseyFolderLayer::~FOdysseyFolderLayer()
{
}

FOdysseyFolderLayer::FOdysseyFolderLayer( const FName& iName )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kFolder )
    , mBlendingMode( ::ul3::BM_NORMAL )
    , mOpacity( 1.0f )
    , mIsOpen( true )
{
}

//---

::ul3::eBlendingMode
FOdysseyFolderLayer::GetBlendingMode()
{
    return mBlendingMode;
}

void
FOdysseyFolderLayer::SetBlendingMode( ::ul3::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

void
FOdysseyFolderLayer::SetBlendingMode( FText iBlendingMode )
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
FOdysseyFolderLayer::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
}

float
FOdysseyFolderLayer::GetOpacity() const
{
    return mOpacity;
}

void
FOdysseyFolderLayer::SetOpacity( float iOpacity )
{
    if( iOpacity < 0.f || iOpacity > 1.f )
        return;

    mOpacity = iOpacity;
}

bool
FOdysseyFolderLayer::IsOpen() const
{
    return mIsOpen;
}

void
FOdysseyFolderLayer::SetIsOpen( bool iIsOpen )
{
    mIsOpen = iIsOpen;
}



FArchive& 
operator<<(FArchive &Ar, FOdysseyFolderLayer* ioSaveFolderLayer )
{
    if(!ioSaveFolderLayer)
        return Ar;

    Ar << ioSaveFolderLayer->mName;
    Ar << ioSaveFolderLayer->mIsLocked;
    Ar << ioSaveFolderLayer->mIsVisible;
    int bm = static_cast< int >( ioSaveFolderLayer->mBlendingMode );
    Ar << bm;
    Ar << ioSaveFolderLayer->mIsOpen;
    Ar << ioSaveFolderLayer->mOpacity;
    return Ar;
}

#undef LOCTEXT_NAMESPACE
