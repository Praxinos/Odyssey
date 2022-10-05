// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#include "ULISLoaderModule.h"
#include "Containers/Map.h"

class FULISLoaderModule : public IULISLoaderModule
{
public:
    FULISLoaderModule()
        : mThreadPool( nullptr )
        , mCommandQueue( nullptr )
        , mFontEngine( nullptr )
        , mContextMap()
    {}

private:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    /** IULISLoaderModule implementation */
    virtual ::ULIS::FThreadPool& ThreadPool() override;
    virtual ::ULIS::FContext& FindOrAddContext( ::ULIS::eFormat iFormat ) override;
    virtual void RemoveContext( ::ULIS::eFormat iFormat ) override;
    virtual ::ULIS::FFontEngine& FontEngine() override;

private:
    // Private Data Members
    ::ULIS::FThreadPool*    mThreadPool;
    ::ULIS::FCommandQueue*  mCommandQueue;
    ::ULIS::FFontEngine*    mFontEngine;
    TMap< uint32, ::ULIS::FContext* > mContextMap;
};

IMPLEMENT_MODULE( FULISLoaderModule, ULISLoader )

void FULISLoaderModule::StartupModule() {
    mThreadPool     = new ::ULIS::FThreadPool();
    mCommandQueue   = new ::ULIS::FCommandQueue( *mThreadPool );
    mFontEngine     = new ::ULIS::FFontEngine();
}

void FULISLoaderModule::ShutdownModule() {
    for( auto it : mContextMap )
        delete  it.Value;
    delete  mCommandQueue;
    delete  mThreadPool;
    delete  mFontEngine;
}

::ULIS::FThreadPool&
FULISLoaderModule::ThreadPool() {
    return  *mThreadPool;
}

::ULIS::FContext&
FULISLoaderModule::FindOrAddContext( ::ULIS::eFormat iFormat ) {
    uint32 key = static_cast< uint32 >( iFormat );
    ::ULIS::FContext** val = mContextMap.Find( key );
    if( val ) {
        return  **val;
    } else {
        ::ULIS::FContext* ctx = new ::ULIS::FContext( *mCommandQueue, iFormat, ::ULIS::PerformanceIntent_MEM );
        mContextMap.Add( key, ctx );
        return  *ctx;
    }
}

void
FULISLoaderModule::RemoveContext( ::ULIS::eFormat iFormat ) {
    uint32 key = static_cast< uint32 >( iFormat );
    mContextMap.Remove( key );
}

::ULIS::FFontEngine&
FULISLoaderModule::FontEngine() {
    return  *mFontEngine;
}

