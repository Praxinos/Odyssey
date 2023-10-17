#include "OdysseyVectorProcessor.h"
#include "OdysseyVectorComputer.h"

FOdysseyVectorProcessor::~FOdysseyVectorProcessor()
{

}

FOdysseyVectorProcessor::FOdysseyVectorProcessor( FOdysseyVectorComputer* iComputer, uint32 iProcessorID )
    : mComputer( iComputer )
    , mProcessorID( iProcessorID )
    , mExecute ( false )
    , mRunning ( false )
{
}

void
FOdysseyVectorProcessor::Run()
{
    std::unique_lock<std::mutex> lock( mComputer->mMutex );

    FGenericPlatformProcess::SetThreadAffinityMask( (uint64) 1 << mProcessorID );

    mRunning = true;
//UE_LOG(LogTemp, Warning, TEXT("running %d"), mProcessorID  );
    while( mComputer->mRunning )
    {
        //mProcessedMutex.lock();
//UE_LOG(LogTemp, Warning, TEXT("waiting cpu:%d"), mProcessorID  );
        mComputer->mCondition.wait( lock/*, [this]{ return mExecute == true; }*/ );
//UE_LOG(LogTemp, Warning, TEXT("processing cpu:%d"), mProcessorID  );
        if( mComputer->mInstruction( mProcessorID, mComputer->mProcessorCount ) )
        {
//UE_LOG(LogTemp, Warning, TEXT("before ending cpu:%d"), mProcessorID  );
            //mComputer->mMutex.lock();
            mComputer->mStarted = false;
            mComputer->mFinishedCondition.notify_one();
            //mComputer->mMutex.unlock();
//UE_LOG(LogTemp, Warning, TEXT("after ending cpu:%d"), mProcessorID  );
        }

        //mProcessedMutex.lock();

        //mProcessedMutex.unlock(); 
    }
}
