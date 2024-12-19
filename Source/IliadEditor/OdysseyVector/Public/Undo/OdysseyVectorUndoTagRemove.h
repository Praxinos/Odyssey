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

class ODYSSEYVECTOR_API FOdysseyVectorUndoTagRemove : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoTagRemove();
        FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                   , FOdysseyVectorTag* iTag
                                   , uint64 iReturnFlags );
        FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                   , const std::vector<FOdysseyVectorTag*>& iTagArray
                                   , uint64 iReturnFlags );
        FOdysseyVectorUndoTagRemove( FOdysseyVectorGroupPaint* iScene
                                   , const std::list<FOdysseyVectorTag*>& iTagList
                                   , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorTag*> mTagArray;
};
