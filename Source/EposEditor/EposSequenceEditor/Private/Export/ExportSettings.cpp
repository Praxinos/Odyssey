// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "Export/ExportSettings.h"

#include "Camera/CameraComponent.h"
#include "Evaluation/MovieSceneRootOverridePath.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ExportSettings"

//---

float
GetMostRelevantCameraAspectRatio( ISequencer* iSequencer, FMovieSceneSequenceIDRef iSequenceId )
{
    if( !iSequencer || iSequenceId == MovieSceneSequenceID::Invalid )
        return 1.77777f;

    struct FSequenceCameraVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace ) override
        {
            UE::MovieScene::FSubSequencePath subsequencepath( iLocalSpace.SequenceID, mSequencer->GetSharedPlaybackState() );

            if( !subsequencepath.Contains( mSequenceId ) )
                return;

            UMovieSceneSingleCameraCutSection* cameracut_section = Cast<UMovieSceneSingleCameraCutSection>( iSection );
            if( !cameracut_section )
                return;

            UCameraComponent* camera_component = cameracut_section->GetFirstCamera( *mSequencer, iLocalSpace.SequenceID );
            if( !camera_component )
                return;

            FMinimalViewInfo ViewInfo;
            camera_component->GetCameraView( FApp::GetDeltaTime(), ViewInfo );

            int32* count = mAspectRatios.Find( ViewInfo.AspectRatio );
            if( count )
                *count = *count + 1;
            else
                mAspectRatios.Add( ViewInfo.AspectRatio, 1 );
        }

        ISequencer* mSequencer;
        FMovieSceneSequenceID mSequenceId;
        TMap<float, int32>  mAspectRatios;
    };

    //---

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitRootTracks = true;
    params.bVisitSubSequences = true;

    FSequenceCameraVisitor camera_visitor;
    camera_visitor.mSequencer = iSequencer;
    camera_visitor.mSequenceId = iSequenceId;

    // Visit all notes
    VisitSequence( iSequencer->GetRootMovieSceneSequence(), params, camera_visitor );

    //---

    camera_visitor.mAspectRatios.ValueSort( TGreater<int32>() );

    TArray<float> keys;
    camera_visitor.mAspectRatios.GetKeys( keys );

    return keys.Num() ? keys[0] : 1.77777f;
}

//---

#undef LOCTEXT_NAMESPACE
