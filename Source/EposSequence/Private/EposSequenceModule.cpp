// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "EposSequenceModule.h"

#include "INamingFormatter.h"

#define LOCTEXT_NAMESPACE "FEposSequenceModule"

void
FEposSequenceModule::StartupModule()
{
    // This code will execute after your module is loaded into memory; the exact timing is specified in the .uplugin file per-module
}

void
FEposSequenceModule::ShutdownModule()
{
    // This function may be called during shutdown to clean up your module.  For modules that support dynamic reloading,
    // we call this function before unloading the module.
}

//---

void
FEposSequenceModule::RegisterNamingFormatter( UNamingFormatter* iFormatter )
{
    mNamingFormatters.Add( iFormatter );
}

void
FEposSequenceModule::UnregisterNamingFormatter( UNamingFormatter* iFormatter )
{
    mNamingFormatters.Remove( iFormatter );
}

template<typename Formatter>
UNamingFormatter*
FEposSequenceModule::GetNamingFormatter()
{
    for( auto formatter : mNamingFormatters )
    {
        if( formatter->IsA<Formatter>() )
        {
            return formatter;
        }
    }

    return nullptr;
}

#undef LOCTEXT_NAMESPACE

IMPLEMENT_MODULE( FEposSequenceModule, EposSequence )
