// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

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
                                      , FOdysseyVectorObject* iObject );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                      , const std::vector<FOdysseyVectorObject*>& iObjectArray );
         FOdysseyVectorUndoObjectParam( FOdysseyVectorGroupPaint* iScene
                                      , const std::list<FOdysseyVectorObject*>& iObjectList );

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
