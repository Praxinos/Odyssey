// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "OdysseyPainterEditorSource.h"
#include "OdysseyAnimationLayerStack.h"

//class UOdysseyAnimationTexture;
class FOdysseyRasterEditPreview;
class UOdysseyAnimationPlayer;
class UTexture;

/**
 * The source of the painter editor for an animation: the layer stack, the current frame and the players of its views.
 * It owns an FOdysseyRasterEditPreview, created when a player is activated and given to every animation player it shows,
 * so that the views show the active raster edit sessions.
 */
class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorAnimationSource
    : public FOdysseyPainterEditorSource
{
public:
    // Construction / Destruction
    virtual ~FOdysseyPainterEditorAnimationSource();
    FOdysseyPainterEditorAnimationSource(UOdysseyAnimation* iAnimation);

public:
    static const FGuid& StaticId();

public:
    virtual const FGuid& Id() const;
    virtual int Width() const override;
    virtual int Height() const override;
    virtual void Activate(FOdysseyPainterEditor* iEditor) override;
    virtual void Inactivate() override;
    virtual UTexture* DisplayTexture() const override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;

    virtual void Clear() override;
    virtual void ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock) override;
    virtual void PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock) override;
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock ) override;
    virtual bool IsValid() const override;

    virtual void RecordCurrentFrameUndo() const override;

public:

    void SetExternalPlayer(UOdysseyAnimationPlayer* iPlayer);

    UOdysseyAnimation* GetAnimation() const;
    virtual UOdysseyAnimationLayerStack* GetLayerStack() const override;
    //UOdysseyAnimationTexture* GetAnimationTexture() const;
    UOdysseyAnimationPlayer* GetAnimationPlayer() const;

public:
    virtual void AddReferencedObjects(FReferenceCollector& Collector);

private:
    void ActivatePlayer( UOdysseyAnimationPlayer* iPlayer );
    void InactivatePlayer( UOdysseyAnimationPlayer* iPlayer );

    void OnCurrentFrameChanged();
    void OnRenderingChanged(const FOdysseyRenderingChangedEvent& iEvent);

public:
    UOdysseyAnimation* mAnimation;
    //TObjectPtr<UOdysseyAnimationTexture> mTexture;
    TObjectPtr<UOdysseyAnimationPlayer> mPlayer;
    TObjectPtr<UOdysseyAnimationPlayer> mExternalPlayer;
    TArray<FGuid> mImageRenderingComposition;
    FOdysseyPainterEditor* mEditor;

private:
    /** Shows the uncommitted raster edits in the players of this source. */
    TSharedPtr<FOdysseyRasterEditPreview> mRasterEditPreview;
};
