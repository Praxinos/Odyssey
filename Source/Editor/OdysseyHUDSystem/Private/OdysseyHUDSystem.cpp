// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "OdysseyHUDSystem.h"

#include "OdysseySurfaceTexture2DEditable.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUDSystem::~FOdysseyHUDSystem()
{
    delete mHUDSurface;
}

FOdysseyHUDSystem::FOdysseyHUDSystem()
    : mHUDBlock(nullptr)
    , mHUDSurface(nullptr)
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

void FOdysseyHUDSystem::RebuildHUDSurface(FVector2D iSize)
{
    if( mHUDSurface )
    {
        delete mHUDSurface;
        mHUDSurface = nullptr;
    }

    SetHUDBlock( MakeShared<::ULIS::FBlock>(iSize.X, iSize.Y, ::ULIS::Format_BGRA8) );

    ClearHUDSurface();

    mHUDSurface = new FOdysseySurfaceTexture2DEditable( mHUDBlock );
}

void FOdysseyHUDSystem::ClearHUDSurface()
{
    if( mHUDBlock )
    {
        ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
        ctx.Clear(*mHUDBlock);
        ctx.Finish();
    }

    if( mHUDSurface )
        mHUDSurface->Invalidate();
}

void
FOdysseyHUDSystem::DrawHUD( const FDrawHUDParams& iParams )
{
    OnDrawHUD().ExecuteIfBound(iParams);
}

FOdysseyHUDSystem::FOnDrawHUD&
FOdysseyHUDSystem::OnDrawHUD()
{
    return mOnDrawHUD;
}