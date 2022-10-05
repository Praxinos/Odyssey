// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "Tools/DrawingTool/OdysseyDrawingToolWorker.h"

#include "CoreMinimal.h"

#include <functional>

FOdysseyDrawingToolWorker::~FOdysseyDrawingToolWorker()
{
}

FOdysseyDrawingToolWorker::FOdysseyDrawingToolWorker()
    : mQueue()
{
}

void
FOdysseyDrawingToolWorker::Push(tFunction iFunction)
{
    //mQueue is ThreadSafe
    mQueue.Enqueue(iFunction);
}

void
FOdysseyDrawingToolWorker::Execute(int iNum)
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
FOdysseyDrawingToolWorker::ExecuteFor(long long iMaxTimeMs)
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
FOdysseyDrawingToolWorker::Finish()
{
    Execute(0);
}

void
FOdysseyDrawingToolWorker::Clear()
{
    mQueue.Empty();
}