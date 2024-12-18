// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2023

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorGroupPaint;
class FOdysseyVectorTag;
class FOdysseyVectorTagInbetweener;

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerParam : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerParam();
        FOdysseyVectorUndoTagInbetweenerParam( FOdysseyVectorGroupPaint* iScene
                                             , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                             , uint64 iReturnFlagss );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotArray;
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerWithThickness : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerWithThickness();
        FOdysseyVectorUndoTagInbetweenerWithThickness( FOdysseyVectorGroupPaint* iScene
                                                     , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                     , uint64 iReturnFlags  );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerConstantWidth : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerConstantWidth();
        FOdysseyVectorUndoTagInbetweenerConstantWidth( FOdysseyVectorGroupPaint* iScene
                                                     , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                     , uint64 iReturnFlags  );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerGridSize : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerGridSize();
        FOdysseyVectorUndoTagInbetweenerGridSize( FOdysseyVectorGroupPaint* iScene
                                                , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                , uint64 iReturnFlags  );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerGridType : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerGridType();
        FOdysseyVectorUndoTagInbetweenerGridType( FOdysseyVectorGroupPaint* iScene
                                                , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                , uint64 iReturnFlags );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerSquare : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerSquare();
        FOdysseyVectorUndoTagInbetweenerSquare( FOdysseyVectorGroupPaint* iScene
                                                , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                , uint64 iReturnFlags );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerInterpolationType : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerInterpolationType();
        FOdysseyVectorUndoTagInbetweenerInterpolationType( FOdysseyVectorGroupPaint* iScene
                                                         , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                         , uint64 iReturnFlags );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerColor : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerColor();
        FOdysseyVectorUndoTagInbetweenerColor( FOdysseyVectorGroupPaint* iScene
                                             , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                             , uint64 iReturnFlags );

};

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagInbetweenerMapAsPolyline : public FOdysseyVectorUndoTagInbetweenerParam
{
    public:
        ~FOdysseyVectorUndoTagInbetweenerMapAsPolyline();
        FOdysseyVectorUndoTagInbetweenerMapAsPolyline( FOdysseyVectorGroupPaint* iScene
                                                     , const std::vector<FOdysseyVectorTagInbetweener*>& iInbetweenerTagArray
                                                     , uint64 iReturnFlags );

};
