// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterDrawingToolWorker.h"

#include "CoreMinimal.h"

#include <chrono>
#include <functional>
#include <chrono>

FOdysseyPainterEditorRasterDrawingToolWorker::~FOdysseyPainterEditorRasterDrawingToolWorker()
{
}

FOdysseyPainterEditorRasterDrawingToolWorker::FOdysseyPainterEditorRasterDrawingToolWorker()
    : mQueue()
{
}

void
FOdysseyPainterEditorRasterDrawingToolWorker::Push(tFunction iFunction)
{
    //mQueue is ThreadSafe
    mQueue.Enqueue(iFunction);
}

void
FOdysseyPainterEditorRasterDrawingToolWorker::Execute(int iNum)
{
    tFunction func;
    for( int i = 0; !mQueue.IsEmpty() && (i < iNum || iNum <= 0); i++)
    {
        mQueue.Peek(func);
        func();
        mQueue.Pop();
    }
}

void
FOdysseyPainterEditorRasterDrawingToolWorker::ExecuteFor(long long iMaxTimeMs)
{
    auto start_time = std::chrono::steady_clock::now();
    long long delta = 0;
    tFunction func;
    while( !mQueue.IsEmpty() && delta < iMaxTimeMs )
    {
        //Execute an action
        mQueue.Peek(func);
        func();
        mQueue.Pop();

        auto end_time = std::chrono::steady_clock::now();
        delta = std::chrono::duration_cast<std::chrono::milliseconds>( end_time - start_time ).count();
    }
}

void
FOdysseyPainterEditorRasterDrawingToolWorker::Finish()
{
    Execute(0);
}

void
FOdysseyPainterEditorRasterDrawingToolWorker::Clear()
{
    mQueue.Empty();
}
