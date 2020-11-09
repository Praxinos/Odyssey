// IDDN FR.001.250001.004.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#include "Misc/EposEditorPlaybackContext.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "EposEditorPlaybackContext"

UObject*
FEposEditorPlaybackContext::GetPlaybackContext() const
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
FEposEditorPlaybackContext::ComputePlaybackContext()
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
