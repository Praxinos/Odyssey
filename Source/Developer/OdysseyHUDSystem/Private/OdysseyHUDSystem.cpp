// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDSystem.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUDSystem::~FOdysseyHUDSystem()
{
    delete mHUDSurface;
    delete mHUDBlock;
}

FOdysseyHUDSystem::FOdysseyHUDSystem()
    : mHUDBlock( nullptr ),
      mHUDSurface(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyHUDSystem::SetHUDBlock(::ULIS::FBlock* iBlock)
{
    if (mHUDBlock == iBlock)
        return;

    delete mHUDBlock;

    mHUDBlock = iBlock;
    if (!mHUDBlock) 
        return;
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Getters

::ULIS::FBlock*
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
    {
        delete mHUDSurface;
    }

    SetHUDBlock( new ::ULIS::FBlock(iSize.X, iSize.Y, ::ULIS::Format_BGRA8) );

    ::ULIS::FContext& ctx = IULISLoaderModule::StaticFindOrAddContext(::ULIS::Format_BGRA8);
    ctx.Clear(*mHUDBlock);
    ctx.Finish();

    mHUDSurface = new FOdysseySurfaceTexture2DEditable( mHUDBlock );
}