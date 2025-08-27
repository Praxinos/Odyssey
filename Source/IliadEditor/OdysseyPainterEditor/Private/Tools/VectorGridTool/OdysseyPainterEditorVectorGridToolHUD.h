// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorBaseTool/OdysseyPainterEditorVectorBaseToolHUD.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"
#include "OdysseyVectorPoint.h"

typedef struct _FGridPoint
{
    FOdysseyVectorObject* mDeformedObject;
    FOdysseyVectorPoint* mPoint;
    double s;
    double t;
}
FGridPoint;

class ODYSSEYPAINTEREDITOR_API FGridNode : public FOdysseyVectorPoint
{
    public:
        ~FGridNode();
        FGridNode();

        void SetSelected( bool iSelected );
        bool IsSelected();

    private:
        bool mSelected;
};

typedef struct _FGridCell
{
    FGridNode *mNode[4];
    std::vector<FGridPoint> mPointArray;
}
FGridCell;

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorGridToolHUD : public FOdysseyPainterEditorVectorBaseToolHUD
{
    public:
        static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorGridToolHUD();
        FOdysseyPainterEditorVectorGridToolHUD(  UOdysseyPainterEditorVectorGridTool* iGridTool );

        virtual void DrawHUD( const FOdysseyHUDElement::FDrawHUDParams& iParams ) override;
        virtual void Draw( BLContext* iBLContext ) override;
        virtual void Reset( ) override;
        virtual void Load() override;

        void PickNodes( double iWorldX
                      , double iWorldY
                      , double iWorldRadius
                      , std::vector<FGridNode*>& oNodeArray  );
        void PickNodes( ::ULIS::FRectD& iWorldRect, std::vector<FGridNode*>& oNodeArray );
        void ClearSelection();
        void GetSelection( std::vector<FGridNode*>& oNodeArray );
        void MakeGrid( uint64 iHUDFlags );
        void Deform();
        void StartSelectionRectangle( double iWorldX, double iWorldY );
        void DragSelectionRectangle( double iWorldX, double iWorldY );
        void EndSelectionRectangle( std::vector<FGridNode*>& oNodeArray );
        void Export( std::vector<FOdysseyVectorPoint*>& oPointArray );

    protected:
        uint32 MapPoint( FOdysseyVectorObject* iDeformedObject, FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY );
        uint32 MapObjectRecurse( FOdysseyVectorObject* iObject );
        uint32 MapObjectNoRecurse( FOdysseyVectorObject* iObject );
        uint32 MapPath( FOdysseyVectorPath* iPath, BLMatrix2D& iInverseGridMatrix );
        uint32 MapPaintGroup( FOdysseyVectorGroupPaint* iPaintGroup, BLMatrix2D& iInverseGridMatrix );
        uint32 MapPaintGroupBuckets( FOdysseyVectorGroupPaint* iPaintGroup, BLMatrix2D& iInverseGridMatrix );
        void MakeNodes();
        void MakeCells();
        void Map();
        void DeformCell( FGridCell& iCell );
        void UnselectNodes();
        void DrawSelectionRectangle( const FOdysseyHUDElement::FDrawHUDParams& iParams
                                   , FLinearColor& iHighlightColor );

    private:
        UOdysseyPainterEditorVectorGridTool* mGridTool;
        ::ULIS::FVec2D mWorldSelStart; // selection rectangle is in world coordinates (to be aligned with world axis)
        ::ULIS::FVec2D mWorldSelDrag; // selection rectangle is in world coordinates (to be aligned with world axis)
        std::vector<FGridNode> mNodeArray;
        std::vector<FGridCell> mCellArray;
        uint32 mNodeCountX;
        uint32 mNodeCountY;
        double mCellSizeX;
        double mCellSizeY;
        uint32 mPointCount;
};
