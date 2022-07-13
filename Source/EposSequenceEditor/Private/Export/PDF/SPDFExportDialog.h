// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class IDetailsView;
class ISequencer;
class UExportPDFSettings;
class UExportPDFSheetWidget;
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

    void Construct( const FArguments& InArgs, TWeakPtr<ISequencer> iSequencer, UMovieSceneSequence* iCurrentSequence );

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

private:
    TWeakPtr<ISequencer>        mSequencer;
    UMovieSceneSequence*        mRootSequence { nullptr };
    UMovieSceneSequence*        mCurrentSequence { nullptr };

    TSharedPtr<IDetailsView>    mDetailsViewExportPDFSettings;

    UExportPDFSettings*         mExportPDFSettings { nullptr };

    UExportPDFSheetWidget*      mPDFSheetWidget { nullptr };
    SVerticalBox::FSlot*        mPDFSlot { nullptr };
};
