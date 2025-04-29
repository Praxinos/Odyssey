// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#include "ULISLoaderModule.h"
#include "Containers/Map.h"

class FULISLoaderModule : public IULISLoaderModule
{
public:
    FULISLoaderModule()
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
    struct FData
    {
        ::ULIS::FThreadPool*    mThreadPool;
        ::ULIS::FCommandQueue*  mCommandQueue;
        ::ULIS::FFontEngine*    mFontEngine;
        TMap< uint32, ::ULIS::FContext* > mContextMap;
    };

    TMap<uint32, FData> mThreadsData;

    FData& GetCurrentThreadData();

    FCriticalSection mMutex;
};

IMPLEMENT_MODULE( FULISLoaderModule, ULISLoader )

void FULISLoaderModule::StartupModule() {
    /*
    mThreadPool     = new ::ULIS::FThreadPool();
    mCommandQueue   = new ::ULIS::FCommandQueue( *mThreadPool );
    mFontEngine     = new ::ULIS::FFontEngine();
    */
}

void FULISLoaderModule::ShutdownModule() {
    for( auto it : mThreadsData )
    {
        for( auto itMap : it.Value.mContextMap )
            delete  itMap.Value;
        delete  it.Value.mCommandQueue;
        delete  it.Value.mThreadPool;
        delete  it.Value.mFontEngine;
    }
}

FULISLoaderModule::FData&
FULISLoaderModule::GetCurrentThreadData() {
    FScopeLock lock(&mMutex);
    const uint32 CurrentThreadId = FPlatformTLS::GetCurrentThreadId();
    if (!mThreadsData.Contains(CurrentThreadId))
    {
        ::ULIS::FThreadPool* threadPool = new ::ULIS::FThreadPool();
        mThreadsData.Add(
            CurrentThreadId,
            {
                threadPool,
                new ::ULIS::FCommandQueue( *threadPool),
                new ::ULIS::FFontEngine(),
                TMap< uint32, ::ULIS::FContext* >()
            }
        );
    }

    return mThreadsData[CurrentThreadId];
}

::ULIS::FThreadPool&
FULISLoaderModule::ThreadPool() {

    return  *GetCurrentThreadData().mThreadPool;
}

::ULIS::FContext&
FULISLoaderModule::FindOrAddContext( ::ULIS::eFormat iFormat ) {
    uint32 key = static_cast< uint32 >( iFormat );
    ::ULIS::FContext** val = GetCurrentThreadData().mContextMap.Find( key );
    if( val ) {
        return  **val;
    } else {
        //TODO: why are we using PerformanceIntent_MEM instead of PerformanceIntent_Max ?
        ::ULIS::FContext* ctx = new ::ULIS::FContext( *GetCurrentThreadData().mCommandQueue, iFormat, ::ULIS::PerformanceIntent_MEM );
        GetCurrentThreadData().mContextMap.Add( key, ctx );
        return  *ctx;
    }
}

void
FULISLoaderModule::RemoveContext( ::ULIS::eFormat iFormat ) {
    uint32 key = static_cast< uint32 >( iFormat );
    GetCurrentThreadData().mContextMap.Remove( key );
}

::ULIS::FFontEngine&
FULISLoaderModule::FontEngine() {
    return  *GetCurrentThreadData().mFontEngine;
}
