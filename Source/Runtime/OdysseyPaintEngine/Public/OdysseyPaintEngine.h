// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlock.h"
#include "OdysseyStrokeOptions.h"
#include "OdysseySmoothingTypes.h"
#include "IOdysseyLayer.h"
#include "OdysseyTransactionnable.h"
#include <ULIS_CCOLOR>
#include <ULIS_BLENDINGMODES>
#include <queue>
#include <functional>

class FOdysseyLayerStack;
class UOdysseyBrushAssetBase;

namespace ULIS { class FThreadPool; }

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine : public FOdysseyTransactionnable
{
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    ~FOdysseyPaintEngine();
    FOdysseyPaintEngine( FOdysseyUndoHistory* InUndoHistoryRef = 0 );


public:
    // Public API
    void InterruptDelay();
    void Tick();
    void SetLayerStack(         FOdysseyLayerStack* iLayerStack );
    void SetBrushInstance(      UOdysseyBrushAssetBase* iBrushInstance );
    void SetColor(              const ::ULIS::CColor& iColor );
    void SetSizeModifier(       float iValue );
    void SetOpacityModifier(    float iValue );
    void SetFlowModifier(       float iValue );
    void SetBlendingModeModifier( ::ULIS::eBlendingMode iValue );

    void  SetStrokeStep         ( int32 iValue );
    void  SetStrokeAdaptative   ( bool  iValue );
    void  SetStrokePaintOnTick  ( bool  iValue );
    void  SetInterpolationType  ( EOdysseyInterpolationType iValue );
    void  SetSmoothingMethod    ( EOdysseySmoothingMethod   iValue );
    void  SetSmoothingStrength  ( int32 iValue );
    void  SetSmoothingEnabled   ( bool  iValue );
    void  SetSmoothingRealTime  ( bool  iValue );
    void  SetSmoothingCatchUp   ( bool  iValue );

    bool GetStokePaintOnTick() const;
    bool GetSmoothingCatchUp() const;

    void PushStroke(            const  FOdysseyStrokePoint&  iPoint, bool first = false );
    void EndStroke();
    void AbortStroke();
    void TriggerStateChanged();

    const  ::ULIS::CColor&   GetColor() const;

private:
    // Private API
    void CheckReallocTempBuffer();
    void ReallocInvalidMaps();
    void UpdateBrushInstance();

private:
    // Private Data Members
    UOdysseyBrushAssetBase*                     brush_instance;

    FOdysseyLayerStack*                         layer_stack;
    int width;
    int height;
    int nTileX;
    int nTileY;

    TArray< FOdysseyStrokePoint >               raw_stroke;
    TArray< FOdysseyStrokePoint >               result_stroke;

    FOdysseyBlock*                              temp_buffer;
    InvalidTileMap                              tmpInvalidTileMap;
    InvalidTileMap                              strokeInvalidTileMap;

    ::ULIS::CColor                               color;

    float                                       size_modifier;
    float                                       opacity_modifier;
    float                                       flow_modifier;
    ::ULIS::eBlendingMode                       mBlendingModeModifier;
    float                                       mStepValue;

    IOdysseyInterpolation*                      interpolator;
    IOdysseySmoothing*                          smoother;

    bool                                        bSmoothingEnabled;
    bool                                        bRealTime;
    bool                                        bCatchUp;
    bool                                        bAdaptativeStep;
    bool                                        bPaintOnTick;

    bool                                        bPendingEndStroke;

    ::ULIS::FThreadPool*                        mTileThreadPool;
    std::queue< std::function<void() > >        mDelayQueue;
};
