// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseyBlock.h"
#include "OdysseySurfaceEditable.h"
#include "OdysseyStrokeOptions.h"
#include "OdysseySmoothingTypes.h"
#include "OdysseyTransactionnable.h"
#include "OdysseyBrushBlueprint.h"
#include <ULIS3>
#include <queue>
#include <functional>

class UOdysseyBrushAssetBase;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine 
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
public:
    // Events Declaration
    DECLARE_MULTICAST_DELEGATE(FOnStrokeBegin);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeStep);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrokeWillEnd, const TArray<::ul3::FRect>& iChangedTiles);
	DECLARE_MULTICAST_DELEGATE_OneParam(FOnStrokeEnd, const TArray<::ul3::FRect>& iChangedTiles);
	DECLARE_MULTICAST_DELEGATE(FOnStrokeAbort);

    /* 
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPreviewBlockTilesChanged, const TArray<::ul3::FRect>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditedBlockTilesWillChange, const TArray<::ul3::FRect>&);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnEditedBlockTilesChanged, const TArray<::ul3::FRect>&);
    */

protected:
    //type defining how to deal with tile invalidation
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    virtual ~FOdysseyPaintEngine();
    FOdysseyPaintEngine( FOdysseyUndoHistory* iUndoHistoryRef = 0 );

public:
    // Paint Engine API
    // Drawing
    void BeginStroke( const FOdysseyStrokePoint& iPoint, const FOdysseyStrokePoint& iPreviousPoint );
    void PushStroke( const FOdysseyStrokePoint& iPoint );
    void EndStroke();
    void AbortStroke();

    // Paint Operations
    void Clear();
    void Fill();
    
    // Misc
    void SetCurrentStrokePoint(const FOdysseyStrokePoint& iPoint); //TODO: Rename,  it allows us to set what is the current cursor position when hovering the canvas
    void UpdateStrokeOptions(); //TODO: Replace by a TAttribute<FStrokeOptions>
    void TriggerStateChanged(); //TODO: Rename, it allows us to rebuild the BrushInstance when the brush parameters changes
    // void InterruptStrokeAndStampInPlace(); //TODO: Remove, allows to end the stroke and flush everything

public:
    // Setters
    virtual void Block(FOdysseyBlock* iBlock);
    void Brush(UOdysseyBrush* iBrush);
    void BrushInstance(UOdysseyBrushAssetBase* iBrushInstance, bool iApplyOverrides);
    void IsLocked(TAttribute<bool> iIsLocked);
    void SetColor( const ::ul3::FPixelValue& iColor );

    //TODO: Replace by a TAttribute<FPaintModifiers>
    void SetSizeModifier( float iValue );
    void SetOpacityModifier( float iValue );
    void SetFlowModifier( float iValue );
    void SetBlendingModeModifier( ::ul3::eBlendingMode iValue );
    void SetAlphaModeModifier( ::ul3::eAlphaMode iValue );

public:
    // Getters
	// FOdysseyBlock* PreviewBlock(); //TODO: Replace with new system OriginalBlock/EditedBlock/StrokeBlock
	FOdysseyBlock* StrokeBlock();
    FOdysseyBlock* EditedBlock();

    UOdysseyBrush* Brush() const;
    UOdysseyBrushAssetBase* BrushInstance() const;
    bool IsLocked() const;
    const ::ul3::FPixelValue& GetColor() const;

    // TODO: Have a TAttribute system for modifiers like TAttribute<FPaintModifiers>
    float GetSizeModifier() const;
    float GetOpacityModifier() const;
    float GetFlowModifier() const;
    ::ul3::eBlendingMode GetBlendingModeModifier() const;
    ::ul3::eAlphaMode GetAlphaModeModifier() const;
    
    // TODO: Have a TAttribute system for StrokeOptions like TAttribute<FStrokeOptions>
    FOdysseyStrokeOptions* StrokeOptions();
    bool GetSmoothingCatchUp() const;

public:
    // PaintEnginge Ticks

    // The main Tick entry
    virtual void Tick();
    
    // Applies the CatchUp if needed
    void SmoothingCatchUpTick();

    // Applies the smoothing if enabled and not in realtime
    void SmoothingEndStroke();

private:
    // DrawingQueue

    // Executes the Drawing Queue
    void ExecuteDrawingQueue(long long iMaxTimeMs);
    void ClearDrawingQueue();

private:
    // Blocks Management

    // Internal Methods
    void ClearStrokeBlock();

    // Copies EditedBlock Rects to Original Block
    void UpdateOriginalBlock(bool iForceRefresh = false);

    // Blends StrokeBlock on OriginalBlock and stores the result in EditedBlock
    void UpdateEditedBlock();

private:
    // Listeners

    // Called when the current brush is has been compiled
    void OnBrushCompiled(UBlueprint* iBrush);

    // Updates the Brush Instance with the new parameters (paint modifiers, stroke Options etc...)
    void UpdateBrushInstance();

protected:
    // Tiles Management

    // Updates the invalid tiles structure from the brush instance invalid rects
    void UpdateInvalidMaps();

    // Returns the Sub Stroke Invalid Tiles structure 
    TArray<::ul3::FRect> GetSubStrokeInvalidTiles();

    // Returns the Stroke Invalid Tiles structure 
    TArray<::ul3::FRect> GetStrokeInvalidTiles();

    // Clear the Sub Stroke Invalid Tiles structure
    // void ClearSubStrokeInvalidMap();

    // Clear the Stroke Invalid Tiles structure
    // void ClearStrokeInvalidMap();

    // Reallocates the Invalid Maps to match EditedBlock
    void ReallocInvalidMaps();

    // MISC - Deallocates the given InvalidMap
    void DeallocInvalidMap( InvalidTileMap& ioMap );

    // MISC - Allocates the given InvalidMap
    void AllocInvalidMap( InvalidTileMap& ioMap );

    // MISC - Clears the given InvalidMap
    void ClearInvalidMap( InvalidTileMap ioMap );

    // MISC - Copies an InvalidMap to another one
    void CopyInvalidMap(InvalidTileMap iSrcMap, InvalidTileMap ioDstMap);
    
    // MISC - Retrieve the Rect from a specific Tile
    ::ul3::FRect MakeTileRect( int iTileX, int iTileY );

    // MISC - Set the InvalidMap tiles value from the given rect
    void SetMapWithRect( InvalidTileMap ioMap, const ::ul3::FRect& iRect, bool iValue );

    //--- OLD ---
    // void InterruptDrawing();

    // virtual void BlendStrokeBlockInPreviewBlock(TArray<::ul3::FRect>& iRects);
	// void CopyPreviewBlockInEditedBlock(TArray<::ul3::FRect>& iRects);
    // void CopyEditedBlockInPreviewBlock(TArray<::ul3::FRect>& iRects);
	// void CopyEditedBlockInPreviewBlock();
    // void UpdatePreviewBlockTiles();
    // void UpdateEditedBlockTiles();
    // void UpdateBrushCursorPreview();

    // void OnBrushCompiled(UBlueprint* iBrush);

private:
    // Stroke Helpers
    TArray< FOdysseyStrokePoint > ComputeInterpolation();
    void ComputePointRelativeParameters(FOdysseyStrokePoint& ioPoint, const FOdysseyStrokePoint& iPreviousPoint);
    void AddResultPoints(const TArray< FOdysseyStrokePoint >& iPoints); 
    void ResetStroke();

private:
    // Stroke Options Management

    // TODO: Have a TAttribute system for StrokeOptions like TAttribute<FStrokeOptions>
    void UpdateStrokeStep();
    void UpdateStrokeAdaptative();
    void UpdateStrokePaintOnTick();
    void UpdateInterpolationType();
    void UpdateSmoothingMethod();
    void UpdateSmoothingStrength();
    void UpdateSmoothingEnabled();
    void UpdateSmoothingRealTime();
    void UpdateSmoothingCatchUp();

public:
    // Delegates
    FOnStrokeBegin& OnStrokeBegin() { return mOnStrokeBeginDelegate; }
	FOnStrokeStep& OnStrokeStep() { return mOnStrokeStepDelegate; }
    FOnStrokeWillEnd& OnStrokeWillEnd() { return mOnStrokeWillEndDelegate; }
	FOnStrokeEnd& OnStrokeEnd() { return mOnStrokeEndDelegate; }
	FOnStrokeAbort& OnStrokeAbort() { return mOnStrokeAbortDelegate; }

    /* FOnPreviewBlockTilesChanged& OnPreviewBlockTilesChanged() { return mOnPreviewBlockTilesChangedDelegate; }
    FOnEditedBlockTilesWillChange& OnEditedBlockTilesWillChange() { return mOnEditedBlockTilesWillChangeDelegate; }
    FOnEditedBlockTilesChanged& OnEditedBlockTilesChanged() { return mOnEditedBlockTilesChangedDelegate; } */

protected:
    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

protected:
    // protected Data Members
    UOdysseyBrush*                      mBrush;
    TAttribute<bool>                    mIsLocked;
    FOdysseyStrokeOptions               mStrokeOptions;

	FOdysseyBlock*                      mEditedBlock; // Holds th original block to edit
	FOdysseyBlock*                      mStrokeBlock; //Holds the stroke tiles
    FOdysseyBlock*                      mOriginalBlock; //Holds the stroke tiles

    UOdysseyBrushAssetBase*             mBrushInstance;

    int                                 mWidth;
    int                                 mHeight;
    int                                 mCountTileX;
    int                                 mCountTileY;

    TArray< FOdysseyStrokePoint >       mRawStroke;
    TArray< FOdysseyStrokePoint >       mResultStroke;

    InvalidTileMap                      mSubStrokeInvalidMap;
    InvalidTileMap                      mStrokeInvalidMap;

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

    std::queue<std::function<void()>>   mDrawingQueue;

    FOnStrokeBegin                      mOnStrokeBeginDelegate;
	FOnStrokeStep                       mOnStrokeStepDelegate;
    FOnStrokeWillEnd                        mOnStrokeWillEndDelegate;
	FOnStrokeEnd                        mOnStrokeEndDelegate;
	FOnStrokeAbort                      mOnStrokeAbortDelegate;

    // FOnPreviewBlockTilesChanged         mOnPreviewBlockTilesChangedDelegate;
    // FOnEditedBlockTilesWillChange       mOnEditedBlockTilesWillChangeDelegate; //TODO: Remove once the undo will be moved from Layerstack to PaintEngine
    // FOnEditedBlockTilesChanged          mOnEditedBlockTilesChangedDelegate;

    std::chrono::steady_clock::time_point mLastStrokeTimePoint;

public:
    FOdysseySurfaceEditable*            mBrushCursorPreviewSurface;
    FVector2D                           mBrushCursorPreviewShift;
    long long                           mLastBrushCursorComputationTime;
    bool                                mBrushCursorInvalid;
};
