// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyImageLayer.h"

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImageLayer::~FOdysseyImageLayer()
{
    delete mBlock;
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FVector2D iSize, ETextureSourceFormat iTextureSourceFormat )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseySerializable( 1 ) //Version of FOdysseyImageLayer
    , mBlock( nullptr )
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
    , mIsAlphaLocked( false )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y, iTextureSourceFormat );
    ::ULIS::FClearFillContext::Clear( mBlock->GetIBlock() );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock )
    : IOdysseyLayer( iName, IOdysseyLayer::eType::kImage )
    , IOdysseySerializable( 1 ) //Version of FOdysseyImageLayer
    , mBlock( iBlock )
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
    , mIsAlphaLocked( false )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

FOdysseyBlock*
FOdysseyImageLayer::GetBlock() const
{
    return mBlock;
}

::ULIS::eBlendingMode
FOdysseyImageLayer::GetBlendingMode() const
{
    return mBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( ::ULIS::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( FText iBlendingMode )
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
FOdysseyImageLayer::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
}

float
FOdysseyImageLayer::GetOpacity() const
{
    return mOpacity;
}

void
FOdysseyImageLayer::SetOpacity( float iOpacity )
{
    if( iOpacity < 0.f || iOpacity > 1.f )
        return;

    mOpacity = iOpacity;
}

bool
FOdysseyImageLayer::IsAlphaLocked() const
{
    return mIsAlphaLocked;
}

void
FOdysseyImageLayer::SetIsAlphaLocked( bool iIsAlphaLocked )
{
    mIsAlphaLocked = iIsAlphaLocked;
}


void
FOdysseyImageLayer::CopyPropertiesFrom( const FOdysseyImageLayer &iCopy )
{
    mOpacity = iCopy.GetOpacity();
    mBlendingMode = iCopy.GetBlendingMode();
    mIsLocked = iCopy.IsLocked();
    mIsVisible = iCopy.IsVisible();
    mIsAlphaLocked = iCopy.IsAlphaLocked();
}

//---

#undef LOCTEXT_NAMESPACE
