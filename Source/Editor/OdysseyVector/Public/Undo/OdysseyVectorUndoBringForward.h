#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoBringForward : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoBringForward();
        FOdysseyVectorUndoBringForward( FOdysseyVectorScene* iScene
                                      , FOdysseyVectorObject* iObject );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorScene* mScene;
        FOdysseyVectorObject* mObject;
};
