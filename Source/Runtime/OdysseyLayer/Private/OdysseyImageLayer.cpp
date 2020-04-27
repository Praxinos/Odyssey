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

FOdysseyImageLayer::FOdysseyImageLayer( const FOdysseyLayerStack* iParentStack, const FName& iName, FVector2D iSize, ETextureSourceFormat iTextureSourceFormat )
    //: IOdysseySerializable( 1 ) // Version of FOdysseyImageLayer
    : IOdysseyLayer( iParentStack, iName, IOdysseyLayer::eType::kImage )
    , mBlock( nullptr )
    , mBlendingMode( ::ul3::eBlendingMode::kNormal )
    , mOpacity( 1.0f )
    , mIsAlphaLocked( false )
{
    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y, iTextureSourceFormat );
    ::ul3::FClearFillContext::Clear( mBlock->GetBlock() );
}

FOdysseyImageLayer::FOdysseyImageLayer( const FOdysseyLayerStack* iParentStack, const FName& iName, FOdysseyBlock* iBlock )
    //: IOdysseySerializable( 1 ) // Version of FOdysseyImageLayer
    : IOdysseyLayer( iParentStack, iName, IOdysseyLayer::eType::kImage )
    , mBlock( iBlock )
    , mBlendingMode( ::ul3::eBlendingMode::kNormal )
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

::ul3::eBlendingMode
FOdysseyImageLayer::GetBlendingMode() const
{
    return mBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( ::ul3::eBlendingMode iBlendingMode )
{
    mBlendingMode = iBlendingMode;
}

void
FOdysseyImageLayer::SetBlendingMode( FText iBlendingMode )
{
    for( uint8 i = 0; i < (int)::ul3::eBlendingMode::kNumBlendingModes; ++i )
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
FOdysseyImageLayer::GetBlendingModeAsText() const
{
    return FText::FromString( ANSI_TO_TCHAR( ::ul3::kwBlendingMode[static_cast<int>( mBlendingMode )] ) );
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
