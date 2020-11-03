// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#include "ULISLoaderModule.h"


class FULISLoaderModule : public IULISLoaderModule
{

public:
    FULISLoaderModule()
        : mHostDeviceInfo ( ::ul3::FHostDeviceInfo::Detect() )
    {
    }

private:
    /** IModuleInterface implementation */
    virtual void StartupModule() override;
    virtual void ShutdownModule() override;

public:
    /** IULISLoaderModule implementation */
    virtual ::ul3::FThreadPool* ThreadPool() override;
    virtual const ::ul3::FHostDeviceInfo& HostDeviceInfo() const override;
    virtual const ::ul3::FFontEngine& FontEngine() const override;
    virtual const ::ul3::FFontRegistry& FontRegistry() const override;

private:
    // Private Data Members
    ::ul3::FThreadPool*     mThreadPool;
    ::ul3::FHostDeviceInfo  mHostDeviceInfo;
    ::ul3::FFontEngine*     mFontEngine;
    ::ul3::FFontRegistry*   mFontRegistry;
};


IMPLEMENT_MODULE( FULISLoaderModule, ULISLoader )

void FULISLoaderModule::StartupModule() {
    mThreadPool     = ::ul3::XCreateThreadPool();
    mFontEngine     = new ::ul3::FFontEngine();
    mFontRegistry   = new ::ul3::FFontRegistry( *mFontEngine );
}


void FULISLoaderModule::ShutdownModule() {
    ::ul3::XDeleteThreadPool( mThreadPool );
    delete  mFontRegistry;
    delete  mFontEngine;
}


::ul3::FThreadPool*
FULISLoaderModule::ThreadPool() {
    return  mThreadPool;
}


const ::ul3::FHostDeviceInfo&
FULISLoaderModule::HostDeviceInfo() const {
    return  mHostDeviceInfo;
}


const ::ul3::FFontEngine&
FULISLoaderModule::FontEngine() const {
    return  *mFontEngine;
}


const ::ul3::FFontRegistry&
FULISLoaderModule::FontRegistry() const {
    return  *mFontRegistry;
}

