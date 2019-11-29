// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlock.h"
#include "OdysseySurface.h"
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

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine 
{
private:
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    ~FOdysseyPaintEngine();
    FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryRef = 0 );

public:
    // Public API
    void InterruptDelay();
    void Tick();
    void SetTextureSourceFormat( ETextureSourceFormat iTextureSourceFormat );
    void SetLayerStack( FOdysseyLayerStack* iLayerStack );
    void SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance );
    void SetColor( const ::ULIS::CColor& iColor );
    void SetSizeModifier( float iValue );
    void SetOpacityModifier( float iValue );
    void SetFlowModifier( float iValue );
    void SetBlendingModeModifier( ::ULIS::eBlendingMode iValue );
    void SetAlphaModeModifier( ::ULIS::eAlphaMode iValue );

    void SetStrokeStep( int32 iValue );
    void SetStrokeAdaptative( bool iValue );
    void SetStrokePaintOnTick( bool iValue );
    void SetInterpolationType( EOdysseyInterpolationType iValue );
    void SetSmoothingMethod( EOdysseySmoothingMethod iValue );
    void SetSmoothingStrength( int32 iValue );
    void SetSmoothingEnabled( bool iValue );
    void SetSmoothingRealTime( bool iValue );
    void SetSmoothingCatchUp( bool iValue );

    bool GetStokePaintOnTick() const;
    bool GetSmoothingCatchUp() const;

    void PushStroke( const FOdysseyStrokePoint& iPoint, bool iFirst = false );
    void EndStroke();
    void AbortStroke();
    void TriggerStateChanged();
    void InterruptStrokeAndStampInPlace();

    const ::ULIS::CColor& GetColor() const;

    void UpdateBrushCursorPreview();
private:
    // Private API
    void CheckReallocTempBuffer();
    void ReallocInvalidMaps();
    void UpdateBrushInstance();

    void DeallocInvalidTileMap( InvalidTileMap& ioMap );
    void ReallocInvalidTileMap( InvalidTileMap& ioMap );
    void ClearInvalidTileMap( InvalidTileMap ioMap );

    ::ULIS::FRect MakeTileRect( int iTileX, int iTileY );
    void SetMapWithRect( InvalidTileMap ioMap, const ::ULIS::FRect& iRect, bool iValue );

private:
    // Private Data Members
    UOdysseyBrushAssetBase*             mBrushInstance;

    ETextureSourceFormat                mTextureSourceFormat;
    FOdysseyLayerStack*                 mLayerStack;
    int                                 mWidth;
    int                                 mHeight;
    int                                 mCountTileX;
    int                                 mCountTileY;

    TArray< FOdysseyStrokePoint >       mRawStroke;
    TArray< FOdysseyStrokePoint >       mResultStroke;

    FOdysseyBlock*                      mTempBuffer;
    InvalidTileMap                      mTmpInvalidTileMap;
    InvalidTileMap                      mStrokeInvalidTileMap;

    ::ULIS::CColor                      mColor;

    float                               mSizeModifier;
    float                               mOpacityModifier;
    float                               mFlowModifier;
    ::ULIS::eBlendingMode               mBlendingModeModifier;
    ::ULIS::eAlphaMode                  mAlphaModeModifier;
    float                               mStepValue;

    IOdysseyInterpolation*              mInterpolator;
    IOdysseySmoothing*                  mSmoother;

    bool                                mIsSmoothingEnabled;
    bool                                mIsRealTime;
    bool                                mIsCatchUp;
    bool                                mIsAdaptativeStep;
    bool                                mIsPaintOnTick;

    bool                                mIsPendingEndStroke;

    ::ULIS::FThreadPool*                mTileThreadPool;
    std::queue<std::function<void()>>   mDelayQueue;

public:
    FOdysseySurface*                    mBrushCursorPreviewSurface;
    FVector2D                           mBrushCursorPreviewShift;
    long long                           mLastBrushCursorComputationTime;
    bool                                mBrushCursorInvalid;
};
