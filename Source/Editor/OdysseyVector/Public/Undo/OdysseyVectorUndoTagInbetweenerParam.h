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
                                             , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        std::vector<FSnapshotTagInbetweener> mInbetweenerTagSnapshotArray;
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
