// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <functional>

// A Drawing Queue
class FOdysseyPainterEditorRasterDrawingToolWorker
{ 
public:
    typedef ::std::function<void()> tFunction;
 
public:
    //Destructor
    virtual ~FOdysseyPainterEditorRasterDrawingToolWorker();

    //Constructor
    FOdysseyPainterEditorRasterDrawingToolWorker();

public:
    // API
    //Pushes a function in the queue
    void Push(tFunction iFunction);

    //Executes a given number of functions in the queue
    //if iNum == 0 Executes the whole Queue
    void Execute(int iNum);

    //Executes the Queue for X milliseconds
    void ExecuteFor(long long iMaxTimeMs);

    //Executes the whole Queue
    void Finish();

    //Clears the queue
    void Clear();

private:
    TQueue<tFunction, EQueueMode::Spsc> mQueue;
};
