// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Containers/UnrealString.h"
#include "HAL/Platform.h"

class UMovieScene;
class UMovieSceneSequence;

namespace UE::EposSequenceEditor::Private
{
    /**
     * Reports a scripting failure on the executing script frame, or logs it as a script error when no
     * script is running. Does not unwind: callers must return immediately afterwards.
     */
    void RaiseScriptingError(const FString& Message);

    /**
     * The movie scene of Sequence when scripting may read it, or edit it when bRequireWritable is set.
     * The movie scene must exist and, for edits, not be read-only. The asset that owns the sequence (the sequence
     * itself, or the asset it is embedded in) must pass the editor's path filter, be visible, and, for edits, not
     * be read-only in this editor; a sequence in a saved package with no asset outer is judged by its own path.
     * Only sequences in the transient package, such as scratch sequences built by scripts, skip this policy.
     * A hidden sequence fails with the same error whatever its own state.
     *
     * @param SequenceDescription How error messages name the sequence, such as "focused epos sequence".
     * @return The movie scene, or null after raising a scripting error.
     */
    UMovieScene* GetScriptableMovieScene(UMovieSceneSequence* Sequence, bool bRequireWritable, const TCHAR* SequenceDescription);

    /**
     * Applies the same checks as GetScriptableMovieScene without raising, for callers that report failure
     * another way, such as an async result's error.
     *
     * @param OutError Receives the failure message when the movie scene is not accessible.
     * @return The movie scene, or null when OutError was set.
     */
    UMovieScene* FindScriptableMovieScene(UMovieSceneSequence* Sequence, bool bRequireWritable, const TCHAR* SequenceDescription, FString& OutError);
}
