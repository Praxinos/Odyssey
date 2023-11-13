#pragma once

#include <ULIS>
#include <blend2d.h>

class FOdysseyVectorComputer;

class ODYSSEYVECTOR_API FOdysseyVectorProcessor
{
    friend class FOdysseyVectorComputer;

    public:
        ~FOdysseyVectorProcessor();
        FOdysseyVectorProcessor( FOdysseyVectorComputer* iComputer, uint32 iProcessorID );


        void Run();

    private:
        bool mExecute;
        bool mRunning;
        std::mutex mProcessedMutex;
        std::mutex mMutex;
        FOdysseyVectorComputer* mComputer;
        uint32 mProcessorID;
};
