// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "ArianeGraphWorker.h"
#include "ArianeGraph.h"

FArianeGraphWorker::FArianeGraphWorker( const TSharedPtr<FArianeGraph>& InGraph )
    : Graph( InGraph )
{
}

void
FArianeGraphWorker::Launch( const FVector& InViewOrigin, const FPlane& InProjectionPlane )
{
    // the task holds its own reference: the graph stays alive until the solve is done,
    // even if the owner of the worker is destroyed meanwhile
    TSharedPtr<FArianeGraph> SolvedGraph = Graph;

    // otherwise the progress bar shows the previous solve's value until the task starts
    Graph->ResetProgress();

    Task =UE::Tasks::Launch( UE_SOURCE_LOCATION
                            , [ SolvedGraph
                              , InViewOrigin
                              , InProjectionPlane ]()
        {
            // worker thread: touches the graph only
            SolvedGraph->Reset( EArianeGraphResetPolicy::Arrangement );
            SolvedGraph->ProjectGeometry( InViewOrigin, InProjectionPlane );
            SolvedGraph->BuildArrangement();
            SolvedGraph->SolveArrangement();
        } );
}

bool
FArianeGraphWorker::IsBusy() const
{
    return Task.IsValid() && ( Task.IsCompleted() == false );
}
