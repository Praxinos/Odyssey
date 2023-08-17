// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "PainterEditor/OdysseyPainterEditorSource.h"

class UOdysseyAnimationTexture;

/**
 * Base class for an Editor Source
 */
class ODYSSEYANIMATIONEDITOR_API FOdysseyAnimationEditorSource
    : public FOdysseyPainterEditorSource
{
public:
    // Construction / Destruction
    virtual ~FOdysseyAnimationEditorSource();
    FOdysseyAnimationEditorSource(UOdysseyAnimation* iAnimation);

public:
    static const FGuid& StaticId();

public:
    virtual const FGuid& Id() const;
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual UTexture* DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;

public:
    UOdysseyAnimation* GetAnimation() const;
    virtual UOdysseyAnimationLayerStack* GetLayerStack() const override;
    UOdysseyAnimationTexture* GetAnimationTexture() const;

public:
    virtual void AddReferencedObjects(FReferenceCollector& Collector);

public:
    UOdysseyAnimation* mAnimation;
    UOdysseyAnimationTexture* mTexture;
};
