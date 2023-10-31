#pragma once

#include <blend2d.h>
#include <ULIS>
#include "Tools/VectorSelectionTool/OdysseyPainterEditorVectorSelectionToolHUD.h"
#include "Tools/VectorGridTool/OdysseyPainterEditorVectorGridTool.h"

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

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorVectorGridToolHUD : public FOdysseyPainterEditorVectorSelectionToolHUD
{
    public:
        static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyPainterEditorVectorGridToolHUD();
        FOdysseyPainterEditorVectorGridToolHUD(  UOdysseyPainterEditorVectorGridTool* iGridTool );

        virtual void Draw( BLContext* iBLContext, FOdysseyVectorScene* iScene ) override;
        virtual void Reset( FOdysseyVectorScene* iScene ) override;
        virtual bool IsObjectDisplayed( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorObject* iObject
                                      , uint64 iHUDFlags ) override;
        virtual bool IsObjectAltered( FOdysseyVectorScene* iScene
                                    , FOdysseyVectorObject* iObject
                                    , uint64 iHUDFlags ) override;

        bool PickNodes( double iWorldX, double iWorldY, double iWorldRadius, bool iClearSelection );
        bool PickNodes( ::ULIS::FRectD& iWorldRect, bool iClearSelection );
        void ClearSelection();
        void GetSelection( std::vector<FGridNode*>& oNodeArray );
        void MakeGrid( FOdysseyVectorScene* iScene, uint64 iHUDFlags );
        void Deform();
        void StartSelectionRectangle( double iWorldX, double iWorldY );
        void DragSelectionRectangle( double iWorldX, double iWorldY );
        void EndSelectionRectangle( bool iClearSelection );
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
        void Map( FOdysseyVectorScene* iScene );
        void DeformCell( FGridCell& iCell );
        void UnselectNodes();
        void DrawSelectionRectangle( BLContext* iBLContext, BLRgba32& iHighlightColor );

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
