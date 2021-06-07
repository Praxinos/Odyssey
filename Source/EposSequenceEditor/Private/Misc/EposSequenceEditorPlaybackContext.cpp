// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Misc/EposSequenceEditorPlaybackContext.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "EposSequenceEditorPlaybackContext"

UObject*
FEposSequenceEditorPlaybackContext::GetPlaybackContext() const
{
    UWorld* Context = mWeakCurrentContext.Get();
    if (Context)
        return Context;

    Context = ComputePlaybackContext();
    check(Context);
    mWeakCurrentContext = Context;
    return Context;
}

UWorld*
FEposSequenceEditorPlaybackContext::ComputePlaybackContext()
{
    const bool isSimulatingInEditor = GEditor && GEditor->bIsSimulatingInEditor;

    UWorld* editorWorld = nullptr;

    for (const FWorldContext& context : GEngine->GetWorldContexts())
    {
        if( context.WorldType == EWorldType::PIE )
        {
            UWorld* thisWorld = context.World();
            if( thisWorld )
            {
                editorWorld = thisWorld;
                break;
            }
        }
        else if( context.WorldType == EWorldType::Editor )
        {
            editorWorld = context.World();
        }
    }

    check(editorWorld);
    return editorWorld;
}

#undef LOCTEXT_NAMESPACE
