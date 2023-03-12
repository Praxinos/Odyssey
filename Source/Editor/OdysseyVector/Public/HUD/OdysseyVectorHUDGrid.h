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

typedef struct _FGridCell
{
    FOdysseyVectorPoint *mNode[4];
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
        FOdysseyVectorPoint *PickNode( double iWorldX, double iWorldY );

        void MakeGrid( UOdysseyVectorScene& iScene, uint32 iDivisionX, uint32 iDivisionY );
        void Deform();

    protected:
        void MapPoint( UOdysseyVectorObject* iDeformedObject, FOdysseyVectorPoint* iPoint, double iSpaceX, double iSpaceY );
        void MapObject( UOdysseyVectorObject* iObject );
        void MakeNodes( uint32 iCellCountX, uint32 iCellCountY );
        void MakeCells( uint32 iCellCountX, uint32 iCellCountY );
        void Map( UOdysseyVectorScene& iScene );
        void DeformCell( FGridCell& iCell );

    private:
        std::vector<FOdysseyVectorPoint> mNodeArray;
        std::vector<FGridCell> mCellArray;
        uint32 mCellCountX;
        uint32 mCellCountY;
        uint32 mNodeCountX;
        uint32 mNodeCountY;
        double mCellSizeX;
        double mCellSizeY;
};
