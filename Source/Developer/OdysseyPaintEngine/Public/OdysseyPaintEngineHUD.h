// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "OdysseySurfaceTexture2DEditable.h"
#include "OdysseyStrokeOptions.h"
#include "OdysseySmoothingTypes.h"
#include "OdysseyTransactionnable.h"
#include "OdysseyBrushBlueprint.h"
#include <ULIS>
#include <functional>
#include <chrono>

class UOdysseyBrushAssetBase;
class FOdysseyDrawingState;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngineHUD 
    : public FGCObject //Allows us to register External UObject in Garbage Collector
{
protected:
    //type defining how to deal with tile invalidation
    typedef bool** InvalidTileMap;

public:
    // Construction / Destruction
    virtual ~FOdysseyPaintEngineHUD();
    FOdysseyPaintEngineHUD( FOdysseyUndoHistory* iUndoHistoryRef = 0 );

public:    
    // Misc
    void TriggerStateChanged(); //TODO: Rename, it allows us to rebuild the BrushInstance when the brush parameters changes

public:
    // Setters
    virtual void SetHUDBlock(::ULIS::FBlock* iBlock);
    void SetColor( const ::ULIS::FColor& iColor );

public:
    // Getters
    ::ULIS::FBlock* GetHUDBlock();
    const ::ULIS::FColor& GetColor() const;

public:
    // The main Tick entry
    virtual void Tick();

protected:
    // Tiles Management

    // Updates the invalid tiles structure from the brush instance invalid rects
    void UpdateInvalidMaps();

    // Updates the invalid tiles structure from the given Rect
    void UpdateInvalidMaps(::ULIS::FRectI iRect);

    // Returns the Sub Stroke Invalid Tiles structure 
    TArray<::ULIS::FRectI> GetHUDBlockInvalidTiles();

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

protected:
    // FGCObject interface
    virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
    virtual FString GetReferencerName() const override;

protected:
    ::ULIS::FBlock*                      mHUDBlock; // Holds the block in which we draw the HUD

    int                                 mWidth;
    int                                 mHeight;
    int                                 mCountTileX;
    int                                 mCountTileY;

    InvalidTileMap                      mHUDBlockInvalidMap;

    ::ULIS::FColor                      mColor;
};
