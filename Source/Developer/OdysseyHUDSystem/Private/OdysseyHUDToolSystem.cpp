// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "OdysseyHUDToolSystem.h"

//--------------------------------------------------------------------------------------
//----------------------------------------------------------- Construction / Destruction
FOdysseyHUDToolSystem::~FOdysseyHUDToolSystem()
{
    delete mHUDSurface;
    delete mHUDBlock;
}

FOdysseyHUDToolSystem::FOdysseyHUDToolSystem()
    : mHUDBlock( nullptr ),
      mHUDSurface(nullptr)
{
}

//--------------------------------------------------------------------------------------
//------------------------------------------------------------------------------ Setters

void
FOdysseyHUDToolSystem::SetHUDBlock(::ULIS::FBlock* iBlock)
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
FOdysseyHUDToolSystem::GetHUDBlock() const
{
    return mHUDBlock;
}

FOdysseySurfaceTexture2DEditable* FOdysseyHUDToolSystem::GetHUDSurface() const
{
    return mHUDSurface;
}

//--------------------------------------------------------------------------------------
//----------------------------------------------------------------------- Callback Usage

void FOdysseyHUDToolSystem::RefreshHUDSurface(FVector2D iSize)
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