// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "OdysseyPaintEngine.h"
#include "MeshPaintTypes.h" 

class UOdysseyBrushAssetBase;

class ODYSSEYPAINTENGINE_API FOdysseyPaintEngine3D
    : public FOdysseyPaintEngine
{
public:
    // Construction / Destruction
    ~FOdysseyPaintEngine3D();
    FOdysseyPaintEngine3D(FOdysseyUndoHistory* iUndoHistoryRef = 0);

public:
    // Public API
    void Tick() override;
    void Block(FOdysseyBlock* iBlock) override;

    void BlendStrokeBlockInPreviewBlock(TArray<::ul3::FRect>& iRects) override;


public: 
    // Paint engine 3D specific
    FOdysseyBlock* StrokeBlock3D();
    void UpdateTmpTileMapFromTriangles( TArray<FTexturePaintTriangleInfo>& iTriangles );

private:
    FOdysseyBlock* mStrokeBlock3D;

};
