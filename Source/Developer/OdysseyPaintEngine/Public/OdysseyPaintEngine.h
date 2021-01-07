// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceEditable.h"
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
    DECLARE_MULTICAST_DELEGATE(FOnStrokeBegin);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeStep);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeEnd);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeAbort);

    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreviewBlockTilesChanged, const TArray<::ul3::FRect>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditedBlockTilesWillChange, const TArray<::ul3::FRect>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditedBlockTilesChanged, const TArray<::ul3::FRect>&);

protected:
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    virtual ~FOdysseyPaintEngine();
    FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryRef = 0 );

public:
    // Public API
    void Flush();
    void SetLock(bool iValue);

    void InterruptDelay();
    virtual void Tick();
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

    void BeginStroke( const FOdysseyStrokePoint& iPoint, const FOdysseyStrokePoint& iPreviousPoint );
    void PushStroke( const FOdysseyStrokePoint& iPoint );
    void SetCurrentStrokePoint(const FOdysseyStrokePoint& iPoint);
    void EndStroke();
    void AbortStroke();
    void TriggerStateChanged();
    void InterruptStrokeAndStampInPlace();

    const ::ul3::FPixelValue& GetColor() const;
	virtual void Block(FOdysseyBlock* iBlock);
	FOdysseyBlock* PreviewBlock();
	FOdysseyBlock* StrokeBlock();

	virtual void BlendStrokeBlockInPreviewBlock(TArray<::ul3::FRect>& iRects);
	void CopyPreviewBlockInEditedBlock(TArray<::ul3::FRect>& iRects);
    void CopyEditedBlockInPreviewBlock(TArray<::ul3::FRect>& iRects);
	void CopyEditedBlockInPreviewBlock();
    void UpdatePreviewBlockTiles();
    void UpdateEditedBlockTiles();

    void UpdateBrushCursorPreview();

public: //DELEGATES
    FOnStrokeBegin& OnStrokeBegin() { return mOnStrokeBeginDelegate; }
	FOnStrokeStep& OnStrokeStep() { return mOnStrokeStepDelegate; }
	FOnStrokeEnd& OnStrokeEnd() { return mOnStrokeEndDelegate; }
	FOnStrokeAbort& OnStrokeAbort() { return mOnStrokeAbortDelegate; }

    FOnPreviewBlockTilesChanged& OnPreviewBlockTilesChanged() { return mOnPreviewBlockTilesChangedDelegate; }
    FOnEditedBlockTilesWillChange& OnEditedBlockTilesWillChange() { return mOnEditedBlockTilesWillChangeDelegate; }
    FOnEditedBlockTilesChanged& OnEditedBlockTilesChanged() { return mOnEditedBlockTilesChangedDelegate; }

protected:
    // Protected API
    void SmoothingCatchUpTick();
    void ExecuteDelayQueue();
    TArray<::ul3::FRect> GetTmpInvalidTiles();
    TArray<::ul3::FRect> GetStrokeInvalidTiles();
    void ClearStrokeBlock();
    void ResetStroke();
    void UpdateInvalidMaps();
    void EndStrokeTick();
    void SmoothingEndStroke();

    // void CheckReallocTempBuffer();
    void ReallocInvalidMaps();
    void UpdateBrushInstance();

    void DeallocInvalidTileMap( InvalidTileMap& ioMap );
    void ReallocInvalidTileMap( InvalidTileMap& ioMap );
    void ClearInvalidTileMap( InvalidTileMap ioMap );
    void CopyInvalidTileMap(InvalidTileMap iSrcMap, InvalidTileMap ioDstMap);

    ::ul3::FRect MakeTileRect( int iTileX, int iTileY );
    void SetMapWithRect( InvalidTileMap ioMap, const ::ul3::FRect& iRect, bool iValue );

    TArray< FOdysseyStrokePoint > ComputeInterpolation();
    void ComputePointRelativeParameters(FOdysseyStrokePoint& ioPoint, const FOdysseyStrokePoint& iPreviousPoint);
    void AddResultPoints(const TArray< FOdysseyStrokePoint >& iPoints);

protected:
    // protected Data Members
    bool                                mIsLocked;

	FOdysseyBlock*                      mEditedBlock; // Holds th original block to edit
	FOdysseyBlock*                      mStrokeBlock; //Holds the stroke tiles
    FOdysseyBlock*                      mPreviewBlock; //Holds the stroke tiles

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
    FOdysseySmoothingParameters*        mSmoothingParameters;
    IOdysseySmoothing*                  mSmoother;
    
    bool                                mIsSmoothingEnabled;
    bool                                mIsRealTime;
    //bool                                mIsCatchUp;
    bool                                mIsAdaptativeStep;
    bool                                mIsPaintOnTick;

    bool                                mIsPendingEndStroke;

    std::queue<std::function<void()>>   mDelayQueue;

    FOnStrokeBegin                      mOnStrokeBeginDelegate;
	FOnStrokeStep                       mOnStrokeStepDelegate;
	FOnStrokeEnd                        mOnStrokeEndDelegate;
	FOnStrokeAbort                      mOnStrokeAbortDelegate;

    FOnPreviewBlockTilesChanged         mOnPreviewBlockTilesChangedDelegate;
    FOnEditedBlockTilesWillChange       mOnEditedBlockTilesWillChangeDelegate; //TODO: Remove once the undo will be moved from Layerstack to PaintEngine
    FOnEditedBlockTilesChanged          mOnEditedBlockTilesChangedDelegate;

    std::chrono::steady_clock::time_point mLastStrokeTimePoint;

public:
    FOdysseySurfaceEditable*            mBrushCursorPreviewSurface;
    FVector2D                           mBrushCursorPreviewShift;
    long long                           mLastBrushCursorComputationTime;
    bool                                mBrushCursorInvalid;
};
