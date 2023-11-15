#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorGroupPaint.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoEngineClear : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoEngineClear();
        FOdysseyVectorUndoEngineClear( FOdysseyVectorEngine* iEngine );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        FOdysseyVectorEngine* mEngine;
};
