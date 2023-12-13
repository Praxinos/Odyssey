#include "OdysseyVectorProcessor.h"
#include "OdysseyVectorComputer.h"

FOdysseyVectorProcessor::~FOdysseyVectorProcessor()
{

}

FOdysseyVectorProcessor::FOdysseyVectorProcessor( FOdysseyVectorComputer* iComputer, uint32 iProcessorID )
    : mExecute ( false )
    , mRunning ( false )
    , mComputer( iComputer )
    , mProcessorID( iProcessorID )
{
}

void
FOdysseyVectorProcessor::Run()
{
    std::unique_lock<std::mutex> lock( mComputer->mMutex );

    FGenericPlatformProcess::SetThreadAffinityMask( (uint64) 1 << mProcessorID );

    mRunning = true;

    while( mComputer->mRunning )
    {

    }
}
