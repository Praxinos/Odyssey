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
    , mInstruction ( nullptr )
{
    mProcessorCount = FPlatformMisc::NumberOfCoresIncludingHyperthreads();

    mProcessorThreadArray.reserve( mProcessorCount );
    mProcessorArray.reserve( mProcessorCount );

    for( uint32 i = 0; i < mProcessorCount; i++ )
    {
        mProcessorArray.push_back( new FOdysseyVectorProcessor( this, i ) );

        mProcessorThreadArray.emplace_back( &FOdysseyVectorProcessor::Run, mProcessorArray.back() );

        // loop until thread as started
        while( mProcessorArray[i]->mRunning == false );
    }

    FPlatformProcess::Sleep(0.3f);
}

uint32
FOdysseyVectorComputer::GetProcessorCount()
{
    return mProcessorCount;
}

void
FOdysseyVectorComputer::Run( std::function<bool(uint32 iProcessorID,uint32 iProcessorCount)> iInstruction )
{
    std::unique_lock<std::mutex> lock( mFinishedMutex );

    //mMutex.lock();


    
//UE_LOG(LogTemp, Warning, TEXT("--- notifying all ---") );

    mMutex.lock();

    mInstruction = iInstruction;
    mProcessing = 0xFFFFFFFF >> ( 32 - mProcessorCount );
    mCondition.notify_all();

    mMutex.unlock();

    mFinishedCondition.wait( lock, [this]{ return mProcessing == 0; } );
}

//static
FOdysseyVectorComputer&
FOdysseyVectorComputer::GetMainComputer()
{
    static FOdysseyVectorComputer mainComputer;

    return mainComputer;
}
