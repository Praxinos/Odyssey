// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#include "MovieScene.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrack.h"
#include "MovieScene.h"
#include "IMovieScenePlayer.h"
#include "Camera/CameraComponent.h"
#include "Evaluation/MovieSceneEvaluationTemplateInstance.h"
#include "Evaluation/MovieSceneEvalTemplate.h"
#include "EntitySystem/MovieSceneEntityManager.h"
#include "EntitySystem/MovieSceneEntityBuilder.h"
#include "EntitySystem/BuiltInComponentTypes.h"
#include "EntitySystem/Interrogation/MovieSceneInterrogationLinker.h"
#include "EntitySystem/Interrogation/MovieSceneInterrogatedPropertyInstantiator.h"
#include "EntitySystem/TrackInstance/MovieSceneTrackInstanceSystem.h"
#include "Systems/MovieSceneComponentTransformSystem.h"
#include "SingleCameraCutTrack/MovieSceneSingleCameraCutTrackInstance.h"
#include "Tracks/MovieScene3DTransformTrack.h"
#include "Tracks/MovieSceneTransformTrack.h"
#include "UObject/LinkerLoad.h"

/* UMovieSceneCameraCutSection interface
 *****************************************************************************/

UMovieSceneSingleCameraCutSection::UMovieSceneSingleCameraCutSection(const FObjectInitializer& Init)
    : Super(Init)
{
    EvalOptions.EnableAndSetCompletionMode( EMovieSceneCompletionMode::ProjectDefault );

    SetBlendType(EMovieSceneBlendType::Absolute);

    //bSupportsInfiniteRange = true;
}

void UMovieSceneSingleCameraCutSection::OnBindingIDsUpdated( const TMap<UE::MovieScene::FFixedObjectBindingID, UE::MovieScene::FFixedObjectBindingID>& OldFixedToNewFixedMap, FMovieSceneSequenceID LocalSequenceID, const FMovieSceneSequenceHierarchy* Hierarchy, IMovieScenePlayer& Player )
{
    UE::MovieScene::FFixedObjectBindingID FixedBindingID = CameraBindingID.ResolveToFixed( LocalSequenceID, Player );

    if( OldFixedToNewFixedMap.Contains( FixedBindingID ) )
    {
        Modify();

        CameraBindingID = OldFixedToNewFixedMap[FixedBindingID].ConvertToRelative( LocalSequenceID, Hierarchy );
    }
}

void UMovieSceneSingleCameraCutSection::GetReferencedBindings(TArray<FGuid>& OutBindings)
{
    OutBindings.Add(CameraBindingID.GetGuid());
}

void UMovieSceneSingleCameraCutSection::SetRange( const TRange<FFrameNumber>& NewRange )
{
    // Skip TryModify for objects that still need initialization (i.e. we're in the object's constructor), because modifying objects in their constructor can lead to non-deterministic cook issues.
    bool bCanSetRange = true;
    if( !HasAnyFlags( RF_NeedInitialization ) )
    {
        bCanSetRange = TryModify();
    }

    check( NewRange.GetLowerBound().IsOpen() || NewRange.GetUpperBound().IsOpen() || NewRange.GetLowerBoundValue() <= NewRange.GetUpperBoundValue() );
    SectionRange.Value = NewRange;
    TRangeBound<FFrameNumber> bound( TRangeBound<FFrameNumber>::Inclusive( 0 ) );
    SectionRange.Value.SetLowerBound( bound );
    //SectionRange.Value = TRange<FFrameNumber>::All();
}

void UMovieSceneSingleCameraCutSection::SetStartFrame( TRangeBound<FFrameNumber> NewEndFrame )
{
    SetStartFrameAuto();
}

void UMovieSceneSingleCameraCutSection::SetStartFrameAuto()
{
    if( !TryModify() )
        return;

    TRangeBound<FFrameNumber> bound( TRangeBound<FFrameNumber>::Inclusive( 0 ) );
    SectionRange.Value.SetLowerBound( bound );
}

UMovieSceneSection* UMovieSceneSingleCameraCutSection::SplitSection( FQualifiedFrameTime SplitTime, bool bDeleteKeys )
{
    return nullptr;
}

void UMovieSceneSingleCameraCutSection::TrimSection( FQualifiedFrameTime TrimTime, bool bTrimLeft, bool bDeleteKeys )
{
    if( !bTrimLeft )
        Super::TrimSection( TrimTime, bTrimLeft, bDeleteKeys );
}

void UMovieSceneSingleCameraCutSection::PostLoad()
{
    Super::PostLoad();
}

UCameraComponent* UMovieSceneSingleCameraCutSection::GetFirstCamera(IMovieScenePlayer& Player, FMovieSceneSequenceID SequenceID) const
{
    for( TWeakObjectPtr<> WeakObject : CameraBindingID.ResolveBoundObjects( SequenceID, Player ) )
    {
        if (UObject* Object = WeakObject .Get())
        {
            UCameraComponent* Camera = MovieSceneHelpers::CameraComponentFromRuntimeObject(Object);
            if (Camera)
            {
                return Camera;
            }
        }
    }

    return nullptr;
}

#if WITH_EDITOR

void UMovieSceneSingleCameraCutSection::PostEditChangeProperty(FPropertyChangedEvent& PropertyChangedEvent)
{
    Super::PostEditChangeProperty(PropertyChangedEvent);

    const FName PropertyName = PropertyChangedEvent.GetPropertyName();
    if (PropertyName == GET_MEMBER_NAME_CHECKED(UMovieSceneSingleCameraCutSection, SectionRange))
    {
        TRangeBound<FFrameNumber> bound( TRangeBound<FFrameNumber>::Inclusive( 0 ) );
        SectionRange.Value.SetLowerBound( bound );

        if (UMovieSceneSingleCameraCutTrack* Track = GetTypedOuter<UMovieSceneSingleCameraCutTrack>())
        {
            Track->OnSectionMoved(*this, EPropertyChangeType::ValueSet);
        }
    }
}

#endif

void UMovieSceneSingleCameraCutSection::ImportEntityImpl(UMovieSceneEntitySystemLinker* EntityLinker, const FEntityImportParams& Params, FImportedEntity* OutImportedEntity)
{
    using namespace UE::MovieScene;

    FMovieSceneTrackInstanceComponent TrackInstance { decltype( FMovieSceneTrackInstanceComponent::Owner )( this ), UMovieSceneSingleCameraCutTrackInstance::StaticClass() };

    OutImportedEntity->AddBuilder(
        FEntityBuilder()
        .AddTag(FBuiltInComponentTypes::Get()->Tags.Master)
        .Add(FBuiltInComponentTypes::Get()->TrackInstance, TrackInstance)
    );
}

void UMovieSceneSingleCameraCutSection::ComputeInitialCameraCutTransform()
{
    using namespace UE::MovieScene;

    // Clear the compiled transform value.
    bHasInitialCameraCutTransform = false;

    // Is there even an initial time for us to compute a transform?
    if (!GetRange().HasLowerBound())
    {
        return;
    }

    // Find the transform track for our bound camera.
    UMovieScene3DTransformTrack* CameraTransformTrack = nullptr;
    if (CameraBindingID.IsValid())
    {
        UMovieScene* MovieScene = GetTypedOuter<UMovieScene>();
        check(MovieScene);

        for (const FMovieSceneBinding& Binding : MovieScene->GetBindings())
        {
            if (Binding.GetObjectGuid() == CameraBindingID.GetGuid())
            {
                for (UMovieSceneTrack* Track : Binding.GetTracks())
                {
                    CameraTransformTrack = Cast<UMovieScene3DTransformTrack>(Track);
                    if (CameraTransformTrack)
                    {
                        break;
                    }
                }
            }
        }
    }

    // Does the bound camera have a transform track?
    if (CameraTransformTrack == nullptr)
    {
        return;
    }

    // Ok, let's evaluate the transform track at our start time.
    FSystemInterrogator Interrogator;

    TGuardValue<FEntityManager*> DebugVizGuard(GEntityManagerForDebuggingVisualizers, &Interrogator.GetLinker()->EntityManager);

    Interrogator.ImportTrack(CameraTransformTrack, FInterrogationChannel::Default());

    Interrogator.AddInterrogation(GetTrueRange().GetLowerBoundValue());

    Interrogator.Update();

    TArray<FTransform> TempTransforms;
    Interrogator.QueryWorldSpaceTransforms(FInterrogationChannel::Default(), TempTransforms);

    if (TempTransforms.Num() > 0)
    {
        // Store it so we can place it on our imported entities.
        InitialCameraCutTransform = TempTransforms[0];
        bHasInitialCameraCutTransform = true;
    }
}
