// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Modules/ModuleInterface.h"
#include "Modules/ModuleManager.h"
#include <ULIS>

class IULISLoaderModule : public IModuleInterface
{
public:
    static  inline  IULISLoaderModule& Get() {
        return  FModuleManager::LoadModuleChecked< IULISLoaderModule >( "ULISLoader" );
    }

    static  inline  bool  IsAvailable() {
        return  FModuleManager::Get().IsModuleLoaded( "ULISLoader" );
    }

    static inline ::ULIS::FContext& StaticFindOrAddContext( ::ULIS::eFormat iFormat ) {
        static IULISLoaderModule& module = Get();
        return  module.FindOrAddContext( iFormat );
    }

    virtual ::ULIS::FThreadPool& ThreadPool() = 0;
    virtual ::ULIS::FContext& FindOrAddContext( ::ULIS::eFormat iFormat ) = 0;
    virtual void RemoveContext( ::ULIS::eFormat iFormat ) = 0;
    virtual ::ULIS::FFontEngine& FontEngine() = 0;
};

