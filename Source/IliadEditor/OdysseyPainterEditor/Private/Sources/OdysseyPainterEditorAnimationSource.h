// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "OdysseyPainterEditorSource.h"
#include "LayerStack/OdysseyAnimationLayerStack.h"

//class UOdysseyAnimationTexture;
class UOdysseyAnimationPlayer;

/**
 * Base class for an Editor Source
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
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual UTexture* DisplayTexture() const override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;

    virtual void Clear() override;
    virtual void ClearFromCopyBlock(TSharedPtr<::ULIS::FBlock> iCopyBlock) override;
    virtual void PasteBlockToCurrentLayer(TSharedPtr<::ULIS::FBlock> iBlock) override;
    virtual void PasteBlockToNewLayer( TSharedPtr<::ULIS::FBlock> iBlock ) override;

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

    void OnCurrentFrameChanged(UOdysseyAnimation* iAnimation);
    void OnPlayerPlay();
    void OnPlayerStop();

public:
    UOdysseyAnimation* mAnimation;
    //TObjectPtr<UOdysseyAnimationTexture> mTexture;
    TObjectPtr<UOdysseyAnimationPlayer> mPlayer;
    TObjectPtr<UOdysseyAnimationPlayer> mExternalPlayer;
};
