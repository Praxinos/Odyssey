// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "ULISLoaderModule.h"


class FULISLoaderModule : public IULISLoaderModule
{
private:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    /** IULISLoaderModule implementation */
    virtual ::ul3::FThreadPool* ThreadPool() override;
    virtual const ::ul3::FHostDeviceInfo& HostDeviceInfo() override;
    virtual const ::ul3::FFontEngine& FontEngine() override;
    virtual const ::ul3::FFontRegistry& FontRegistry() override;

    // Private Data Members
private:
    ::ul3::FThreadPool*     mThreadPool;
    ::ul3::FHostDeviceInfo  mHostDeviceInfo;
    ::ul3::FFontEngine*     mFontEngine;
    ::ul3::FFontRegistry*   mFontRegistry;
};


IMPLEMENT_MODULE( FULISLoaderModule, ULISLoader )

void FULISLoaderModule::StartupModule() {
    mThreadPool     = new ::ul3::FThreadPool();
    mHostDeviceInfo = ::ul3::FHostDeviceInfo::Detect();
    mFontEngine     = new ::ul3::FFontEngine();
    mFontRegistry   = new ::ul3::FFontRegistry( *mFontEngine );
}


void FULISLoaderModule::ShutdownModule() {
    delete  mThreadPool;
    delete  mFontEngine;
    delete  mFontRegistry;
}


::ul3::FThreadPool*
FULISLoaderModule::ThreadPool() {
    return  mThreadPool;
}


const ::ul3::FHostDeviceInfo&
FULISLoaderModule::HostDeviceInfo() {
    return  mHostDeviceInfo;
}


const ::ul3::FFontEngine&
FULISLoaderModule::FontEngine() {
    return  mFontEngine;
}


const ::ul3::FFontRegistry&
FULISLoaderModule::FontRegistry() {
    return  mFontRegistry;
}

