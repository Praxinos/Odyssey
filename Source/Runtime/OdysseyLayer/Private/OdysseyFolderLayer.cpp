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
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
    , mIsOpen( true )
{
}

//---

::ULIS::eBlendingMode
FOdysseyFolderLayer::GetBlendingMode()
{
    return mBlendingMode;
}

void
FOdysseyFolderLayer::SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

void
FOdysseyFolderLayer::SetBlendingMode( FText iBlendingMode )
{
    for( uint8 i = 0; i < (int)::ULIS::eBlendingMode::kNumBlendingModes; ++i )
    {
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[i] ) );
        if( iBlendingMode.EqualTo( entry ) )
        {
            SetBlendingMode( static_cast<::ULIS::eBlendingMode>( i ) );
            return;
        }
    }
}

FText
FOdysseyFolderLayer::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
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

//---

#undef LOCTEXT_NAMESPACE
