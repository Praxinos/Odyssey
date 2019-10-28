// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#include "OdysseyImageLayer.h"
#include "OdysseyBlock.h"
#include <ULIS_CORE>

#define LOCTEXT_NAMESPACE "OdysseyImageLayer"


//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyImageLayer::~FOdysseyImageLayer()
{
    delete  mBlock;
}

FOdysseyImageLayer::FOdysseyImageLayer( FName iName, FVector2D iSize )
{
    mName = iName;

    check( iSize.X >= 0 && iSize.Y >= 0 );

    mBlock = new FOdysseyBlock( iSize.X, iSize.Y );
    ::ULIS::FClearFillContext::Clear( mBlock->GetIBlock() );
    mBlendingMode = ::ULIS::eBlendingMode::kNormal;

    mIsLocked = false;
    mIsVisible = true;

    mOpacity = 1.f;
}

FOdysseyImageLayer::FOdysseyImageLayer( FName iName, FOdysseyBlock* iBlock )
{
    mName = iName;

    mBlock = iBlock;
    mBlendingMode = ::ULIS::eBlendingMode::kNormal;

    mIsLocked = false;
    mIsVisible = true;

    mOpacity = 1.f;
}

//--------------------------------------------------------------------------------------
//--------------------------------------------------------------------------- Public API

IOdysseyLayer::eType
FOdysseyImageLayer::GetType() const
{
    return eType::kImage;
}

FOdysseyBlock*
FOdysseyImageLayer::GetBlock()  const
{
    return  mBlock;
}

::ULIS::eBlendingMode
FOdysseyImageLayer::GetBlendingMode()  const
{
    return  mBlendingMode;
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
        auto entry = FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[ i ] ) );
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
    return  FText::FromString( ANSI_TO_TCHAR( ::ULIS::kwBlendingMode[ static_cast< int >( mBlendingMode ) ] ) );
}

float
FOdysseyImageLayer::GetOpacity() const
{
    return mOpacity;
}

void
FOdysseyImageLayer::SetOpacity( float iOpacity )
{
    if( iOpacity <= 1.f && iOpacity >= 0.f )
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


#undef LOCTEXT_NAMESPACE
