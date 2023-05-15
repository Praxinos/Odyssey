#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorScene.h"

struct FObjectTransform
{
    FOdysseyVectorObject* object;
    double translationX;
    double translationY;
    double rotation;
    double scalingX;
    double scalingY;

    FObjectTransform( FOdysseyVectorObject* iObject )
    {
        object = iObject;

        iObject->GetTransform( translationX, translationY, rotation, scalingX, scalingY );
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoObjectTransform : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoObjectTransform();
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene, FOdysseyVectorObject* iObject );
        FOdysseyVectorUndoObjectTransform( FOdysseyVectorScene* iScene, std::list<FOdysseyVectorObject*>& iObjectList );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FObjectTransform> objectTransformArray;
};
