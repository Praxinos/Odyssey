// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "SceneTypes.h"
#include "Toolkits/AssetEditorToolkit.h"

class FOdysseyLayerStack;
class FOdysseyPaintEngine;
namespace ULIS { class CColor; }

/**
 * Interface for odyssey painter editor tool kits.
 */
class IOdysseyPainterEditorToolkit
    : public FAssetEditorToolkit
{
public:
    virtual void BeginTransaction( const FText& iSessionName ) = 0;
    virtual void MarkTransactionAsDirty() = 0;
    virtual void EndTransaction() = 0;

    virtual FOdysseyPaintEngine* PaintEngine() = 0;
    virtual FOdysseyLayerStack* LayerStack() = 0;
    virtual void SetColor( const ::ULIS::CColor& iColor ) = 0;
};
