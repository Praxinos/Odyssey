#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"
#include "OdysseyVectorObject.h"
#include "OdysseyVectorPath.h"
#include "OdysseyVectorGroupPaint.h"
#include "OdysseyVectorEllipse.h"

typedef struct _FPropertiesRecord
{
    FOdysseyVectorObject* mObject;
    FObjectParam mObjectParam;
    FGroupPaintParam mGroupPaintParam;
    FPathParam mPathParam;
}
FPropertiesRecord;

class ODYSSEYVECTOR_API FOdysseyVectorUndoPropertyChanged : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoPropertyChanged();
        FOdysseyVectorUndoPropertyChanged( FOdysseyVectorGroupPaint* iScene, std::list<FOdysseyVectorObject*>& iObjectList );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        void SwapParam();

    private:
        std::vector<FPropertiesRecord> mPropertiesRecordArray;
};
