#include "OdysseyVectorComputer.h"


FOdysseyVectorComputer::~FOdysseyVectorComputer()
{
    mRunning = false;
    // run an empty instruction to force the infinite loop to terminate
    Run( []( uint32 iProcessorID, uint32 iProcessorCount ) -> bool { return true; } );

    for( int i = 0; i < mProcessorThreadArray.size(); i++ )
    {
        // wait for the processor to end its loop
        mProcessorThreadArray[i].join();

        delete mProcessorArray[i];
    }
}

FOdysseyVectorComputer::FOdysseyVectorComputer()
    : mRunning ( true )
    , mStarted ( false )
    , mInstruction ( nullptr )
{
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    mProcessorThreadArray.resize( mProcessorCount );
    mProcessorArray.resize( mProcessorCount );

    for( uint32 i = 0; i < mProcessorCount; i++ )
    {
        mProcessorArray[i] = new FOdysseyVectorProcessor( this, i );
        mProcessorThreadArray[i] = std::thread( &FOdysseyVectorProcessor::Run, mProcessorArray[i] );

        // loop until thread as started
        while( mProcessorArray[i]->mRunning == false );
    }
}

uint32
FOdysseyVectorComputer::GetProcessorCount()
{
    return mProcessorCount;
}

void
FOdysseyVectorComputer::Run( std::function<bool(uint32 iProcessorID,uint32 iProcessorCount)> iInstruction )
{
    std::unique_lock<std::mutex> lock( mMutex );

    //mMutex.lock();
    

    mInstruction = iInstruction;
    
//UE_LOG(LogTemp, Warning, TEXT("--- notifying all ---") );

    mStarted = true;
    mCondition.notify_all();
/*
    for( uint32 i = 0; i < mProcessorCount; i++ )
    {
        mProcessorArray[i]->mProcessedMutex.lock();
        mProcessorArray[i]->mProcessedMutex.unlock();
    }
*/
    //mMutex.unlock();

    /*while( mStarted == true )
    {*/
//UE_LOG(LogTemp, Warning, TEXT("before Finished")  );
        mFinishedCondition.wait(lock);
//UE_LOG(LogTemp, Warning, TEXT("after Finished")  );
    /*}*/
}

//static
FOdysseyVectorComputer&
FOdysseyVectorComputer::GetMainComputer()
{
    static FOdysseyVectorComputer mainComputer;

    return mainComputer;
}
