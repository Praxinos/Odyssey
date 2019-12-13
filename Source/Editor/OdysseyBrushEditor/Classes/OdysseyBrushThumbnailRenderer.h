// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "ThumbnailRendering/DefaultSizedThumbnailRenderer.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushThumbnailRenderer.generated.h"

class FCanvas;
class FRenderTarget;

UCLASS()
class UOdysseyBrushThumbnailRenderer : public UDefaultSizedThumbnailRenderer
{
    GENERATED_UCLASS_BODY()

    virtual  void  Draw( UObject* Object, int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas )  override;

protected:
    void  DrawDefaultThumbnail( int32 X, int32 Y, uint32 Width, uint32 Height, FRenderTarget* RenderTarget, FCanvas* Canvas );

};
