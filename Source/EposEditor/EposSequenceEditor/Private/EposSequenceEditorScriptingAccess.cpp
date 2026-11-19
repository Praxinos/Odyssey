// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#include "EposSequenceEditorScriptingAccess.h"

#include "AssetScriptingUtils.h"
#include "Kismet/KismetSystemLibrary.h"
#include "MovieScene.h"
#include "MovieSceneSequence.h"
#include "UObject/Package.h"
#include "UObject/Stack.h"

namespace UE::EposSequenceEditor::Private
{
    void RaiseScriptingError(const FString& Message)
    {
        if (FFrame::GetThreadLocalTopStackFrame())
        {
            UKismetSystemLibrary::RaiseScriptError(Message);
        }
        else
        {
            FFrame::KismetExecutionMessage(*Message, ELogVerbosity::Error);
        }
    }

    UMovieScene* FindScriptableMovieScene(UMovieSceneSequence* Sequence, bool bRequireWritable, const TCHAR* SequenceDescription, FString& OutError)
    {
        UMovieScene* MovieScene = IsValid(Sequence) ? Sequence->GetMovieScene() : nullptr;
        if (!Sequence || !MovieScene || !IsValid(MovieScene))
        {
            OutError = FString::Printf(TEXT("A valid %s is required"), SequenceDescription);
            return nullptr;
        }
        // The editor policy applies to the asset that owns the sequence, which is the sequence itself unless it is
        // embedded in another asset. A sequence in a saved package with no asset outer (not public, or flagged
        // transient) is judged by its own path. Only sequences in the transient package have no policy. The policy
        // is checked first so a hidden sequence reports nothing about its own state.
        const UObject* OwningAsset = nullptr;
        if (Sequence->GetOutermost() != GetTransientPackage())
        {
            OwningAsset = Sequence;
            while (OwningAsset && !OwningAsset->IsAsset())
            {
                OwningAsset = OwningAsset->GetOuter();
            }
            if (!OwningAsset || OwningAsset->IsA<UPackage>())
            {
                OwningAsset = Sequence;
            }
        }
        if (OwningAsset)
        {
            if (AssetScriptingUtils::GetAssetAccess(*OwningAsset, bRequireWritable) != AssetScriptingUtils::EAssetAccess::Allowed)
            {
                OutError = FString::Printf(TEXT("The %s is hidden or read-only in this editor"), SequenceDescription);
                return nullptr;
            }
        }
        if (bRequireWritable && MovieScene->IsReadOnly())
        {
            OutError = FString::Printf(TEXT("The %s is read-only"), SequenceDescription);
            return nullptr;
        }
        return MovieScene;
    }

    UMovieScene* GetScriptableMovieScene(UMovieSceneSequence* Sequence, bool bRequireWritable, const TCHAR* SequenceDescription)
    {
        FString Error;
        UMovieScene* MovieScene = FindScriptableMovieScene(Sequence, bRequireWritable, SequenceDescription, Error);
        if (!MovieScene)
        {
            RaiseScriptingError(Error);
        }
        return MovieScene;
    }
}
