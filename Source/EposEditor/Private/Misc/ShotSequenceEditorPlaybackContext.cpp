// Copyright Epic Games, Inc. All Rights Reserved.

#include "Misc/ShotSequenceEditorPlaybackContext.h"
#include "Editor.h"

#define LOCTEXT_NAMESPACE "ShotSequenceEditorPlaybackContext"

UObject*
FShotSequenceEditorPlaybackContext::GetPlaybackContext() const
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
FShotSequenceEditorPlaybackContext::ComputePlaybackContext()
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
