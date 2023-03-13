#pragma once

#include <blend2d.h>
#include <ULIS>

#include "HUD/OdysseyVectorHUDSelection.h"

typedef struct _FGridPoint
{
    UOdysseyVectorObject* mDeformedObject;
    FOdysseyVectorPoint* mPoint;
    double s;
    double t;
}
FGridPoint;

class ODYSSEYVECTOR_API FGridNode : public FOdysseyVectorPoint
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

class ODYSSEYVECTOR_API FOdysseyVectorHUDGrid : public FOdysseyVectorHUDSelection
{
    public:
        static const uint32 HANDLE_RADIUS = 5;

        virtual ~FOdysseyVectorHUDGrid();
        FOdysseyVectorHUDGrid();

        void Draw( UOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );
        FGridNode *PickNode( double iWorldX, double iWorldY, double iWorldRadius );
        void PickNodes( ::ULIS::FRectD& iWorldRect, std::vector<FGridNode*>& oNodeArray );
        void MakeGrid( UOdysseyVectorScene& iScene, uint32 iDivisionX, uint32 iDivisionY );
        void Deform();
        void StartSelectionRectangle( double iWorldX, double iWorldY );
        void DragSelectionRectangle( double iWorldX, double iWorldY );
        void EndSelectionRectangle( std::vector<FGridNode*>& oNodeArray );

    protected:
        void MapPoint( UOdysseyVectorObject* iDeformedObject, FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY );
        void MapObject( UOdysseyVectorObject* iObject );
        void MakeNodes( uint32 iCellCountX, uint32 iCellCountY );
        void MakeCells( uint32 iCellCountX, uint32 iCellCountY );
        void Map( UOdysseyVectorScene& iScene );
        void DeformCell( FGridCell& iCell );
        void UnselectNodes();
        void DrawSelectionRectangle( UOdysseyVectorScene& iScene, ::ULIS::FRectD& iRoi, uint64 iFlags );

    private:
        ::ULIS::FVec2D mWorldSelStart; // selection rectangle is in world coordinates (to be aligned with world axis)
        ::ULIS::FVec2D mWorldSelDrag; // selection rectangle is in world coordinates (to be aligned with world axis)
        std::vector<FGridNode> mNodeArray;
        std::vector<FGridCell> mCellArray;
        uint32 mCellCountX;
        uint32 mCellCountY;
        uint32 mNodeCountX;
        uint32 mNodeCountY;
        double mCellSizeX;
        double mCellSizeY;
};
