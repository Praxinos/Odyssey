// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"

#include <functional>

// A Drawing Queue
class FOdysseyStrokeEngineWorker
{ 
public:
    typedef ::std::function<void()> tFunction;
 
public:
    //Destructor
    virtual ~FOdysseyStrokeEngineWorker();

    //Constructor
    FOdysseyStrokeEngineWorker();

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
