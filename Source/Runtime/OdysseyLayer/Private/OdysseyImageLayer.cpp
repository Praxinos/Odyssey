// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImageLayer::~FOdysseyImageLayer()
{
    delete mBlock;
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FVector2D iSize )
    : IOdysseyLayer( iName )
    , mBlock( nullptr )
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y );
    ::ULIS::FClearFillContext::Clear( mBlock->GetIBlock() );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FName& iName, FOdysseyBlock* iBlock )
    : IOdysseyLayer( iName )
    , mBlock( iBlock )
    , mBlendingMode( ::ULIS::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
{
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

IOdysseyLayer::eType
FOdysseyImageLayer::GetType() const
{
    return eType::kImage;
}

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

void
FOdysseyImageLayer::CopyPropertiesFrom( const FOdysseyImageLayer &iCopy )
{
    mOpacity = iCopy.GetOpacity();
    mBlendingMode = iCopy.GetBlendingMode();
    mIsLocked = iCopy.IsLocked();
    mIsVisible = iCopy.IsVisible();
}

//---

#undef LOCTEXT_NAMESPACE
