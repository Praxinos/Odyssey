// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlock.h"
#include "OdysseySurface.h"
#include "OdysseyStrokeOptions.h"
#include "OdysseySmoothingTypes.h"
#include "OdysseyTransactionnable.h"
#include <ULIS3>
#include <queue>
#include <functional>

class UOdysseyBrushAssetBase;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine 
{
public:
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrokeChanged, const TArray<::ul3::FRect>&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrokeWillEnd, const TArray<::ul3::FRect>&);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrokeEnd, const TArray<::ul3::FRect>&);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeAbort);

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
    void SetBrushInstance( UOdysseyBrushAssetBase* iBrushInstance );
    void SetColor( const ::ul3::FPixelValue& iColor );
    void SetSizeModifier( float iValue );
    void SetOpacityModifier( float iValue );
    void SetFlowModifier( float iValue );
    void SetBlendingModeModifier( ::ul3::eBlendingMode iValue );
    void SetAlphaModeModifier( ::ul3::eAlphaMode iValue );

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
    float GetOpacity()         const;
    ::ul3::eBlendingMode GetBlendingMode() const;
    ::ul3::eAlphaMode GetAlphaMode() const;


    void PushStroke( const FOdysseyStrokePoint& iPoint, bool iFirst = false );
    void EndStroke();
    void AbortStroke();
    void TriggerStateChanged();
    void InterruptStrokeAndStampInPlace();

    const ::ul3::FPixelValue& GetColor() const;
	void Block(FOdysseyBlock* iBlock);
	FOdysseyBlock* TempBlock();
	FOdysseyBlock* TempBuffer();

    void UpdateBrushCursorPreview();

public: //DELEGATES
	FOnStrokeChanged&	OnStrokeChanged()	{ return mOnStrokeChangedDelegate; }
	FOnStrokeWillEnd&   OnStrokeWillEnd()   { return mOnStrokeWillEndDelegate; }
	FOnStrokeEnd&		OnStrokeEnd()		{ return mOnStrokeEndDelegate; }
	FOnStrokeAbort&		OnStrokeAbort()		{ return mOnStrokeAbortDelegate; }

private:
    // Private API
    // void CheckReallocTempBuffer();
    void ReallocInvalidMaps();
    void UpdateBrushInstance();

    void DeallocInvalidTileMap( InvalidTileMap& ioMap );
    void ReallocInvalidTileMap( InvalidTileMap& ioMap );
    void ClearInvalidTileMap( InvalidTileMap ioMap );

    ::ul3::FRect MakeTileRect( int iTileX, int iTileY );
    void SetMapWithRect( InvalidTileMap ioMap, const ::ul3::FRect& iRect, bool iValue );

private:
    // Private Data Members
	FOdysseyBlock*                      mBlock; // Holds th original block to edit
	FOdysseyBlock*                      mTempBuffer; //Holds the stroke tiles
    UOdysseyBrushAssetBase*             mBrushInstance;

    int                                 mWidth;
    int                                 mHeight;
    int                                 mCountTileX;
    int                                 mCountTileY;

    TArray< FOdysseyStrokePoint >       mRawStroke;
    TArray< FOdysseyStrokePoint >       mResultStroke;

    InvalidTileMap                      mTmpInvalidTileMap;
    InvalidTileMap                      mStrokeInvalidTileMap;

    ::ul3::FPixelValue                  mColor;

    float                               mSizeModifier;
    float                               mOpacityModifier;
    float                               mFlowModifier;
    ::ul3::eBlendingMode                mBlendingModeModifier;
    ::ul3::eAlphaMode                   mAlphaModeModifier;
    float                               mStepValue;

    IOdysseyInterpolation*              mInterpolator;
    IOdysseySmoothing*                  mSmoother;
    
    bool                                mIsSmoothingEnabled;
    bool                                mIsRealTime;
    bool                                mIsCatchUp;
    bool                                mIsAdaptativeStep;
    bool                                mIsPaintOnTick;

    bool                                mIsPendingEndStroke;

    std::queue<std::function<void()>>   mDelayQueue;

	FOnStrokeChanged					mOnStrokeChangedDelegate;
	FOnStrokeEnd						mOnStrokeWillEndDelegate;
	FOnStrokeEnd						mOnStrokeEndDelegate;
	FOnStrokeAbort						mOnStrokeAbortDelegate;

public:
    FOdysseySurface*                    mBrushCursorPreviewSurface;
    FVector2D                           mBrushCursorPreviewShift;
    long long                           mLastBrushCursorComputationTime;
    bool                                mBrushCursorInvalid;
};
