// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "PainterEditor/OdysseyPainterEditorSource.h"

#include "LayerStack/OdysseyTextureLayerStack.h"

class UOdysseyTextureLayerStackUserData;

/**
 * Base class for an Editor Source
 */
class ODYSSEYTEXTUREEDITOR_API FOdysseyTextureEditorSource
    : public FOdysseyPainterEditorSource
{
public:
    // Construction / Destruction
    virtual ~FOdysseyTextureEditorSource();
    FOdysseyTextureEditorSource(UTexture2D* iTexture);
    
public:
    static const FGuid& StaticId();

public:
    virtual const FGuid& Id() const override;
    virtual void Activate() override;
    virtual void Inactivate() override;
    virtual UTexture* DisplayTexture() const override;
    virtual TSharedPtr<::ULIS::FBlock, ESPMode::ThreadSafe> GetDisplayBlock() override;
    virtual FOdysseyMediaProvider GetCurrentMediaProvider() override;
    virtual void Clear() override;

public:
    UTexture2D* GetTexture() const;
    virtual UOdysseyTextureLayerStack* GetLayerStack() const override;
    UOdysseyTextureLayerStackUserData* TextureUserData() const;
    void InitTextureUserData();

public:
    UTexture2D* mTexture;
};
