// Copyright Epic Games, Inc. All Rights Reserved.

#pragma once

#include "Templates/SharedPointer.h"
#include "UObject/WeakObjectPtr.h"

class UWorld;

/**
 * Class that manages the current UWorld context that a level-sequence editor should use for playback
 */
class FEposEditorPlaybackContext : public TSharedFromThis<FEposEditorPlaybackContext>
{
public:

	UObject* GetPlaybackContext() const;

private:

	static UWorld* ComputePlaybackContext();
	
	/** Mutable cached context pointer */
	mutable TWeakObjectPtr<UWorld> mWeakCurrentContext;
};
