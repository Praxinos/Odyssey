// IDDN FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyStrokeOptions.h"
#include "OdysseySmoothingTypes.h"
#include "OdysseyBrushBlueprint.h"
#include <ULIS>
#include <queue>
#include <functional>
#include <chrono>

class UOdysseyBrushAssetBase;
class FOdysseyDrawingState;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine 
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
private:
    enum ePaintState
    {
        kIDLE = 0,
        kDrawingAction,
        kDrawingStroke,
        kDrawingTick
    };

public:
    // Events Declaration

    // User Stroke delegates
    DECLARE_MULTICAST_DELEGATE(FOnStrokeBegin);
    DECLARE_MULTICAST_DELEGATE(FOnStrokeStep);
    DECLARE_MULTICAST_DELEGATE(FOnStrokeEnd);
    DECLARE_MULTICAST_DELEGATE(FOnStrokeAbort);

    
    // Any Paint delegates (even Ticks)
    DECLARE_MULTICAST_DELEGATE(FOnPaintBegin);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPaintStep, const TArray<::ULIS::FRectI>& iChangedTiles);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPaintEnd, const TArray<::ULIS::FRectI>& iChangedTiles);
    DECLARE_MULTICAST_DELEGATE_OneParam(FOnPaintAbort, const TArray<::ULIS::FRectI>& iChangedTiles);

protected:
    //type defining how to deal with tile invalidation
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    virtual ~FOdysseyPaintEngine();
    FOdysseyPaintEngine();

public:
    // Paint Engine API
    // Drawing

    //Moves the current position to iPoint, not drawing (usefull to make Tick() draw at the right position)
    void MoveTo(const FOdysseyStrokePoint& iPoint); //TODO: Rename,  it allows us to set what is the current cursor position when hovering the canvas

    //Begins a stroke at iPoint
    //Some value are computed from the last call to MoveTo(), like direction for example
    void BeginStroke( const FOdysseyStrokePoint& iPoint );

    //Draws a Stroke from the last position to iPoint
    void StrokeTo( const FOdysseyStrokePoint& iPoint );

    //Ends the stroke
    void EndStroke();

    //Aborts the stroke
    void AbortStroke();

    // Will end safely what the PaintEngine is doing (like drawing in ticks for example)
    void Flush();

    // Paint Operations
    //Clear the paint block
    void Clear();

    //Clear the paint block with the current color
    void Fill();

private:
    // Misc
    void UpdateStrokeOptions();
    void TriggerStateChanged(); //TODO: Rename, it allows us to rebuild the BrushInstance when the brush parameters changes

public:
    // Setters
    virtual void Block(::ULIS::FBlock* iBlock);
    // void Brush(UOdysseyBrush* iBrush);
    void BrushInstance(UOdysseyBrushAssetBase* iBrushInstance, /*bool iApplyOverrides*/);
    void IsLocked(TAttribute<bool> iIsLocked); //???
    void SetColor( const ::ULIS::FColor& iColor ); //MOVE TO BruchOptions ?

    //MOVE TO A UOBJECT/USTRUCT CALLED UBrushOptions
    void SetSizeModifier( float iValue );
    void SetOpacityModifier( float iValue );
    void SetFlowModifier( float iValue );
    void SetBlendingModeModifier( ::ULIS::eBlendMode iValue );
    void SetAlphaModeModifier( ::ULIS::eAlphaMode iValue );

public:
    // Getters
    // ::ULIS::FBlock* PreviewBlock(); //TODO: Replace with new system OriginalBlock/EditedBlock/PaintBlock
    ::ULIS::FBlock* PaintBlock();
    ::ULIS::FBlock* EditedBlock();
    ::ULIS::FBlock* OriginalBlock();

    // UOdysseyBrush* Brush() const;
    UOdysseyBrushAssetBase* BrushInstance() const;
    bool IsLocked() const; //???
    
    const ::ULIS::FColor& GetColor() const; //MOVE TO BruchOptions ?

    //MOVE TO A UOBJECT/USTRUCT CALLED UBrushOptions
    float GetSizeModifier() const;
    float GetOpacityModifier() const;
    float GetFlowModifier() const;
    ::ULIS::eBlendMode GetBlendingModeModifier() const;
    ::ULIS::eAlphaMode GetAlphaModeModifier() const;
    
    //MOVE OUTSIDE THIS CLASS AND CREATE A SETTER
    FOdysseyStrokeOptions* StrokeOptions();

public:
    // PaintEnginge Ticks

    // The main Tick entry
    //TODO: Make it through FTickableObject
    virtual void Tick();
    
    // Applies the CatchUp if needed
    void SmoothingCatchUpTick();

    // Applies the smoothing if enabled and not in realtime
    void SmoothingEndStroke();

private:
    bool PaintInitialize(ePaintState iPaintState);
    bool PaintCheck();
    void PaintStep(bool iForceFinish);
    void PaintFinalize();
    void PaintAbort();

private:
    // DrawingQueue

    // Executes the Drawing Queue
    void ExecuteDrawingQueue(long long iMaxTimeMs);
    void ClearDrawingQueue();

private:
    // Blocks Management

    // Internal Methods
    void ClearPaintBlock();

    // Copies EditedBlock Rects to Original Block
    void UpdateOriginalBlock();

    // Blends PaintBlock on OriginalBlock and stores the result in EditedBlock
    void UpdateEditedBlock(bool iForceFinish);

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

    // Updates the invalid tiles structure from the given Rect
    void UpdateInvalidMaps(::ULIS::FRectI iRect);

    // Returns the Sub Stroke Invalid Tiles structure 
    TArray<::ULIS::FRectI> GetPaintBlockInvalidTiles();

    // Returns the Stroke Invalid Tiles structure 
    TArray<::ULIS::FRectI> GetEditedBlockInvalidTiles();

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
    ::ULIS::FRectI MakeTileRect( int iTileX, int iTileY );

    // MISC - Set the InvalidMap tiles value from the given rect
    void SetMapWithRect( InvalidTileMap ioMap, const ::ULIS::FRectI& iRect, bool iValue );

private:
    // Stroke Helpers
    TArray< FOdysseyStrokePoint > ComputeInterpolation();
    void ComputePointRelativeParameters(FOdysseyStrokePoint& ioPoint, const FOdysseyStrokePoint& iPreviousPoint);
    void AddResultPoints(const TArray< FOdysseyStrokePoint >& iPoints); 
    void ResetStroke();

private:
    // Stroke Options Management

    void UpdateStrokeStep();
    void UpdateStrokeAdaptative();
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
    FOnStrokeEnd& OnStrokeEnd() { return mOnStrokeEndDelegate; }
    FOnStrokeAbort& OnStrokeAbort() { return mOnStrokeAbortDelegate; }

    
    FOnPaintBegin& OnPaintBegin() { return mOnPaintBeginDelegate; }
    FOnPaintStep& OnPaintStep() { return mOnPaintStepDelegate; }
    FOnPaintEnd& OnPaintEnd() { return mOnPaintEndDelegate; }
    FOnPaintAbort& OnPaintAbort() { return mOnPaintAbortDelegate; }

    /* FOnPreviewBlockTilesChanged& OnPreviewBlockTilesChanged() { return mOnPreviewBlockTilesChangedDelegate; }
    FOnEditedBlockTilesWillChange& OnEditedBlockTilesWillChange() { return mOnEditedBlockTilesWillChangeDelegate; }
    FOnEditedBlockTilesChanged& OnEditedBlockTilesChanged() { return mOnEditedBlockTilesChangedDelegate; } */

public:
    // Drawing States
    void AddDrawingState(FOdysseyDrawingState* iDrawingState);

protected:
    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

protected:
    // protected Data Members
    ePaintState                         mPaintState;

    UOdysseyBrush*                      mBrush;
    TAttribute<bool>                    mIsLocked;
    FOdysseyStrokeOptions               mStrokeOptions;

    ::ULIS::FBlock*                      mEditedBlock; // Holds the original block to edit
    ::ULIS::FBlock*                      mPaintBlock; //Holds the stroke tiles
    ::ULIS::FBlock*                      mOriginalBlock; //Holds the stroke tiles // ? Unclear

    UOdysseyBrushAssetBase*             mBrushInstance;

    int                                 mWidth;
    int                                 mHeight;
    int                                 mCountTileX;
    int                                 mCountTileY;

    TArray< FOdysseyStrokePoint >       mRawStroke;
    TArray< FOdysseyStrokePoint >       mResultStroke;

    InvalidTileMap                      mPaintBlockInvalidMap;
    InvalidTileMap                      mEditedBlockInvalidMap;

    ::ULIS::FColor                      mColor;

    float                               mSizeModifier;
    float                               mOpacityModifier;
    float                               mFlowModifier;
    ::ULIS::eBlendMode                  mBlendingModeModifier;
    ::ULIS::eAlphaMode                  mAlphaModeModifier;
    float                               mStepValue;

    IOdysseyInterpolation*              mInterpolator;
    FOdysseySmoothingParameters*        mSmoothingParameters;
    IOdysseySmoothing*                  mSmoother;
    
    bool                                mIsSmoothingEnabled;
    bool                                mIsRealTime;
    bool                                mIsAdaptativeStep;

    std::queue<std::function<void()>>   mDrawingQueue;

    FOnStrokeBegin                      mOnStrokeBeginDelegate;
    FOnStrokeStep                       mOnStrokeStepDelegate;
    FOnStrokeEnd                        mOnStrokeEndDelegate;
    FOnStrokeAbort                      mOnStrokeAbortDelegate;

    FOnPaintBegin                       mOnPaintBeginDelegate;
    FOnPaintStep                        mOnPaintStepDelegate;
    FOnPaintEnd                         mOnPaintEndDelegate;
    FOnPaintAbort                       mOnPaintAbortDelegate;

    std::chrono::steady_clock::time_point mLastStrokeTimePoint;

    TArray<FOdysseyDrawingState*>       mDrawingStates;

public:
    FOdysseySurfaceTexture2DEditable*   mBrushCursorPreviewSurface;
    FVector2D                           mBrushCursorPreviewShift;
    long long                           mLastBrushCursorComputationTime;
    bool                                mBrushCursorInvalid;
};
