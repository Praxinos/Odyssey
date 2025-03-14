// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "MovieSceneSequenceID.h"

class IDetailsView;
class ISequencer;
template<typename ItemType> class STileView;
class UEposMovieSceneSequence;
class UExportImageSequenceSettings;
class UExportImageSequenceUISettings;
class UMovieSceneSequence;
struct FPanelItem;

//---

class SExportImageSequenceSettings
    : public SCompoundWidget
    , public FGCObject
{
    SLATE_BEGIN_ARGS( SExportImageSequenceSettings )
        {}
    SLATE_END_ARGS()

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

    void MakePanelItems();
    float GetItemScaledWidth() const;
    float GetItemScaledHeight() const;
    int32 GetItemScaleMultiplier() const;
    void SetItemScaleMultiplier( int32 iItemScaleMultiplier );

private:
    TWeakPtr<ISequencer>        mSequencer;
    UEposMovieSceneSequence*    mRootEposSequence;
    FMovieSceneSequenceID       mRootEposSequenceId;
    UEposMovieSceneSequence*    mCurrentEposSequence { nullptr };
    UMovieSceneSequence*        mCurrentSequence { nullptr };
    FMovieSceneSequenceID       mCurrentSequenceId;

    TSharedPtr<IDetailsView>                        mDetailsViewExportImageSequenceSettings;
    TArray<TSharedPtr<FPanelItem>>                  mPanelItemsList;
    TSharedPtr<STileView<TSharedPtr<FPanelItem>>>   mPanelListView;
    float                                           mItemDefaultWidth { 192.f };
    float                                           mItemDefaultHeight { 192.f };

    TObjectPtr<UExportImageSequenceSettings>        mExportImageSequenceSettings;
    TObjectPtr<UExportImageSequenceUISettings>      mExportImageSequenceUISettings;
};
