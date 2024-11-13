#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectAdd : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectAdd();
        FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                   , FOdysseyVectorObject* iObject
                                   , uint64 iReturnFlags);
        FOdysseyVectorUndoObjectAdd( FOdysseyVectorGroupPaint* iScene
                                   , std::list<FOdysseyVectorObject*>& iObjectList
                                   , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorObject*> mObjectArray;
        std::vector<FOdysseyVectorObject*> mParentArray;
};
