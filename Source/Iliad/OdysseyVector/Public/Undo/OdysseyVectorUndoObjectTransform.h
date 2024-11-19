// IDDN.FR.001.250001.005.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class FOdysseyVectorObject;
class FOdysseyVectorGroupPaint;

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectTransform : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectTransform();
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene, FOdysseyVectorObject* iObject );
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iObjectList );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FSnapshotObject> mObjectSnapshotArray;
};
