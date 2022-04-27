// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"
#include "Layout/Margin.h"

#include "KeyThumbnail/KeyThumbnailSection.h"

class AActor;
class FMenuBuilder;
class FSequencerSectionPainter;
class FTrackEditorThumbnailPool;

/**
 * CameraCut section, which paints and ticks the appropriate section.
 */
class FSingleCameraCutSection
    : public FKeyThumbnailSection
{
public:

    /** Create and initialize a new instance. */
    FSingleCameraCutSection(TSharedPtr<ISequencer> InSequencer, TSharedPtr<FTrackEditorThumbnailPool> InThumbnailPool, UMovieSceneSection& InSection);

    /** Virtual destructor. */
    virtual ~FSingleCameraCutSection();

public:

    // ISequencerSection interface
    virtual void    Tick(const FGeometry& AllottedGeometry, const FGeometry& ClippedGeometry, const double InCurrentTime, const float InDeltaTime) override;
    virtual void    BuildSectionContextMenu(FMenuBuilder& MenuBuilder, const FGuid& ObjectBinding) override;
    virtual FText   GetSectionTitle() const override;
    virtual float   GetSectionHeight() const override;
    virtual int32   OnPaintSection(FSequencerSectionPainter& InPainter) const override;
    virtual FMargin GetContentPadding() const override;

    virtual void    BeginResizeSection();
    virtual void    ResizeSection( ESequencerSectionResizeMode ResizeMode, FFrameNumber ResizeFrameNumber );

    virtual void    BeginSlipSection();
    virtual void    SlipSection( FFrameNumber SlipTime );

    virtual void    BeginDilateSection();
    virtual void    DilateSection( const TRange<FFrameNumber>& NewRange, float DilationFactor );

    // FThumbnailSection interface
    virtual void SetSingleTime(double GlobalTime) override;
    virtual FText HandleThumbnailTextBlockText() const override;

    // FKeyThumbnailSection interface
    virtual void BuildThumbnailKeys() override;
    virtual TArray<double> GetThumbnailKeys() const override;

    // IViewportThumbnailClient interface
    virtual UCameraComponent* GetViewCamera() override;

private:

    /** Get a representative camera for the given time */
    AActor* GetCameraForFrame(FFrameNumber Time) const;

    /** Callback for executing a "Select Camera" menu entry in the context menu. */
    void HandleSelectCameraMenuEntryExecute(AActor* InCamera);

    /** Callback for executing a "Set Camera" menu entry in the context menu. */
    void HandleSetCameraMenuEntryExecute(AActor* InCamera);

private:
    TArray<double> mKeys;
};
