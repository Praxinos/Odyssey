// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDSystem.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUDSystem::~FOdysseyHUDSystem()
{
    delete mHUDSurface;
}

FOdysseyHUDSystem::FOdysseyHUDSystem()
    : mHUDSurface(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyHUDSystem::SetHUDBlock(TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> iBlock)
{
    mHUDBlock = iBlock;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe>
FOdysseyHUDSystem::GetHUDBlock() const
{
    return mHUDBlock;
}

FOdysseySurfaceTexture2DEditable* FOdysseyHUDSystem::GetHUDSurface() const
{
    return mHUDSurface;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Callback Usage

void FOdysseyHUDSystem::RefreshHUDSurface(FVector2D iSize)
{
    if( mHUDSurface )
        delete mHUDSurface;

    SetHUDBlock( MakeShared<::ULIS::FBlock>(iSize.X, iSize.Y, ::ULIS::Format_BGRA8) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    ctx.Clear(*mHUDBlock);
    ctx.Finish();

    mHUDSurface = new FOdysseySurfaceTexture2DEditable( mHUDBlock );
}