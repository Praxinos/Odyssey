#pragma once

#include <ULIS>
#include <blend2d.h>
#include <OdysseyVectorProcessor.h>

class ODYSSEYVECTOR_API FOdysseyVectorComputer
{
    friend class FOdysseyVectorProcessor;

    public:
        static FOdysseyVectorComputer& GetMainComputer();

        ~FOdysseyVectorComputer();
        FOdysseyVectorComputer();

        void Run( std::function<bool(uint32 iProcessorID,uint32 iProcessorCount)> iInstruction );
        uint32 GetProcessorCount();

    private:
        bool mRunning;
        uint32 mProcessorCount;
        uint32 mProcessing;
        std::mutex mProcessedMutex;

        std::vector<FOdysseyVectorProcessor*> mProcessorArray;
        std::vector<std::thread> mProcessorThreadArray;
        std::function<bool(uint32 iProcessorID,uint32 iProcessorCount)> mInstruction;
        std::mutex mMutex;
        std::mutex mFinishedMutex;
        std::condition_variable mCondition;
        std::condition_variable mFinishedCondition;
};
