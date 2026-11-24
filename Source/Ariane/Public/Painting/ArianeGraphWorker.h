// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

// Unreal
#include "CoreMinimal.h"
#include "Tasks/Task.h"

class FArianeGraph;

/**
 * Runs the view dependent part of the FArianeGraph pipeline on a worker thread:
 * Reset( Arrangement ) -> ProjectGeometry -> BuildArrangement -> SolveArrangement.
 * The geometry must have been imported on the game thread beforehand (ImportGeometry).
 * The graph must not be read nor modified while IsBusy() is true.
 */
class ARIANE_API FArianeGraphWorker
{
public:
    // needed by owners that are UObjects (UHT generated constructors default construct the members)
    FArianeGraphWorker() = default;
    explicit FArianeGraphWorker( const TSharedPtr<FArianeGraph>& InGraph );

    /** Game thread. Must not be called while busy. */
    void Launch( const FVector& InViewOrigin, const FPlane& InProjectionPlane );

    /** Game thread. */
    bool IsBusy() const;

private:
    TSharedPtr<FArianeGraph> Graph;
    UE::Tasks::FTask Task;
};
