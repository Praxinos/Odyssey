// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

class ODYSSEYVECTOR_API FOdysseyVectorUndoUngroup : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoUngroup();
        FOdysseyVectorUndoUngroup( FOdysseyVectorGroupPaint* iScene
                                 , FOdysseyVectorGroup* iGroup );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::list<FOdysseyVectorObject*> mUngroupedObjectList;
        FOdysseyVectorGroup* mGroup;
        FOdysseyVectorGroupPaint* mScene;
};
