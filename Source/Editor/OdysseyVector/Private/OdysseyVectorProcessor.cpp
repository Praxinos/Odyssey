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
        //UE_LOG(LogTemp, Warning, TEXT("waiting:%d"), mProcessorID );

        mComputer->mCondition.wait(lock, [this] { return mComputer->mProcessing & ( 1 << mProcessorID ); });

        //UE_LOG(LogTemp, Warning, TEXT("executing!%d"), mProcessorID );

        lock.unlock();
        mComputer->mInstruction( mProcessorID, mComputer->mProcessorCount );
        lock.lock();

        mComputer->mProcessing &= ~(1 << mProcessorID);

 //UE_LOG(LogTemp, Warning, TEXT("ending!%d"), mComputer->mProcessing );
        if( mComputer->mProcessing == 0 )
        {
 //UE_LOG(LogTemp, Warning, TEXT("notifying!%d"), mProcessorID );
            mComputer->mFinishedCondition.notify_one();
        }
    }
}
