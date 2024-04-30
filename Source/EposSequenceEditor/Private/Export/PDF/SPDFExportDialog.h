// IDDN.FR.001.060015.008.S.X.2019.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

class IDetailsView;
class ISequencer;
class UEposMovieSceneSequence;
class UExportPDFSettings;
class UPDFDocExportWidget;
class UMovieSceneSequence;

//---

class SExportPDFSettings
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_BEGIN_ARGS( SExportPDFSettings )
        {}
    SLATE_END_ARGS()

    ~SExportPDFSettings();

    void Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, FMovieSceneSequenceIDRef iSequenceId );

public:
    virtual void AddReferencedObjects( FReferenceCollector& Collector ) override;
    virtual FString GetReferencerName() const override;

public:
    FText GetFullPath() const;
    FText GetErrorText() const;
    FText GetWarningText() const;

    bool CanExportStoryboard() const;
    void ExportStoryboard();

private:
    void GlobalSettingsChanged( const FPropertyChangedEvent& iEvent );

    FOptionalSize GetPageRatio() const;

private:
    TWeakPtr<ISequencer>        mSequencer;
    UEposMovieSceneSequence*    mRootEposSequence { nullptr };
    FMovieSceneSequenceID       mRootEposSequenceId;
    UEposMovieSceneSequence*    mCurrentEposSequence { nullptr };
    UMovieSceneSequence*        mCurrentSequence { nullptr };
    FMovieSceneSequenceID       mCurrentSequenceId;

    TSharedPtr<IDetailsView>    mDetailsViewExportPDFSettings;

    TObjectPtr<UExportPDFSettings>  mExportPDFSettings { nullptr };

    UPDFDocExportWidget*        mPDFDocWidget { nullptr };
    //SHorizontalBox::FSlot*      mPDFSlot { nullptr };
    SVerticalBox::FSlot*        mPDFSlot { nullptr };
};
