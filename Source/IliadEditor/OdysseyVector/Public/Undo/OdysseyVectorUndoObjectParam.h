// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectParam : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectParam();
         FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                      , FOdysseyVectorObject* iObject
                                      , uint64 iReturnFlags );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                      , const std::vector<FOdysseyVectorObject*>& iObjectArray
                                      , uint64 iReturnFlags );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                      , const std::list<FOdysseyVectorObject*>& iObjectList
                                      , uint64 iReturnFlags );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    protected:
        FSnapshotObject* CreateObjectSnapshot( FOdysseyVectorObject* iObject );

    private:
        std::vector<FSnapshotObject*> mObjectSnapshotArray;
};
