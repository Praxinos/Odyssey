// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "SceneTypes.h"
#include "Toolkits/AssetEditorToolkit.h"

class UTexture;
class FOdysseyPaintEngine;

class FOdysseyPaintEngine;
class FOdysseyLayerStack;
namespace ULIS { class CColor; }

/**
 * Interface for odyssey painter editor tool kits.
 */
class IOdysseyPainterEditorToolkit
    : public FAssetEditorToolkit
{
public:
    virtual void BeginTransaction(const FText& SessionName) = 0;
    virtual void MarkTransactionAsDirty() = 0;
    virtual void EndTransaction()  = 0;

    virtual FOdysseyPaintEngine* PaintEngine() = 0;
    virtual FOdysseyLayerStack* LayerStack() = 0;
    virtual void SetColor( const ::ULIS::CColor& iColor ) = 0;
};
