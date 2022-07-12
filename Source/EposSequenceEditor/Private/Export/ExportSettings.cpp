// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#include "Export/ExportSettings.h"

#include "Camera/CameraComponent.h"
#include "ISequencer.h"
#include "MovieSceneSequenceVisitor.h"

#include "SingleCameraCutTrack/MovieSceneSingleCameraCutSection.h"

#define LOCTEXT_NAMESPACE "ExportSettings"

//---

float
GetMostRelevantCameraAspectRatio( ISequencer* iSequencer, UMovieSceneSequence* iSequence )
{
    struct FSequenceCameraVisitor
        : UE::MovieScene::ISequenceVisitor
    {
        virtual void VisitSection( UMovieSceneTrack* iTrack, UMovieSceneSection* iSection, const FGuid& iGuid, const UE::MovieScene::FSubSequenceSpace& iLocalSpace )
        {
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
        TMap<float, int32>  mAspectRatios;
    };

    //---

    UE::MovieScene::FSequenceVisitParams params;
    params.bVisitSections = true;
    params.bVisitMasterTracks = true;
    params.bVisitSubSequences = true;

    FSequenceCameraVisitor camera_visitor;
    camera_visitor.mSequencer = iSequencer;

    // Visit all notes
    VisitSequence( iSequence, params, camera_visitor );

    //---

    camera_visitor.mAspectRatios.ValueSort( TGreater<int32>() );

    TArray<float> keys;
    camera_visitor.mAspectRatios.GetKeys( keys );

    return keys.Num() ? keys[0] : 1.77777f;
}

//---

#undef LOCTEXT_NAMESPACE
