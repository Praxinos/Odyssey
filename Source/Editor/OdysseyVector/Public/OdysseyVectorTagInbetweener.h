#pragma once

#include "CoreMinimal.h"

#include <ULIS>
#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

#include "OdysseyVectorTag.h"

struct FInbetweenerVertex
{
    ::ULIS::FVec2D position;
};

struct FInbetweenerCell
{
    FInbetweenerVertex* vertex[4];
};

class FInbetweenerGrid
{
    public:
        ~FInbetweenerGrid();
        FInbetweenerGrid( uint32 iNumCellX
                        , uint32 iNumCellY
                        , const ::ULIS::FRectD& iBoundingBox );
        void Reset( uint32 iNumCellX
                  , uint32 iNumCellY
                  , const ::ULIS::FRectD& iBoundingBox );

        friend class FOdysseyVectorTagInbetweener;

    private:
        std::vector<FInbetweenerVertex> mVertexBuffer;
        std::vector<FInbetweenerCell> mCellBuffer;
};

class ODYSSEYVECTOR_API FOdysseyVectorTagInbetweener : public FOdysseyVectorTag
{
    public:
        virtual ~FOdysseyVectorTagInbetweener();
        FOdysseyVectorTagInbetweener( FOdysseyVectorObject* iOwnerObject
                                    , uint32 iNumCellX
                                    , uint32 iNumCellY );
        virtual void Reset() override;
        virtual void Draw( BLContext* iBLContext
                         , const ::ULIS::FRectD& iInvalidationArea
                         , double iAncestorsOpacity
                         , uint64 iDrawingFlags ) override;

    protected:
        void DrawGrid( BLContext* iBLContext
                     , const ::ULIS::FRectD& iInvalidationArea
                     , double iAncestorsOpacity
                     , uint64 iDrawingFlags );

    protected:
        FInbetweenerGrid mGrid;
        uint32 mNumCellX;
        uint32 mNumCellY;
};
