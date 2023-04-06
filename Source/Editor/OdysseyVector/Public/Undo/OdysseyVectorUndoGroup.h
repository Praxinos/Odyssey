#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>
#include "Undo/OdysseyVectorUndo.h"

struct FGroupRecord
{
    FOdysseyVectorObject *object;
    FOdysseyVectorObject *parent;

    FGroupRecord( FOdysseyVectorObject *iObject )
    {
        object = iObject;
        parent = iObject->GetParent();
    }
};

class ODYSSEYVECTOR_API FOdysseyVectorUndoGroup : public FOdysseyVectorUndo
{
    public:
        ~FOdysseyVectorUndoGroup();
        FOdysseyVectorUndoGroup( FOdysseyVectorScene* iScene, bool iSwitchSpace );

        void RecordBefore( std::list<FOdysseyVectorObject*>& iGroupedObjectList );
        void RecordAfter( FOdysseyVectorGroup* iAddedGroup, std::vector<FOdysseyVectorGroup*>& iRemovedGroupArray );
        void RecordAfter( FOdysseyVectorGroup* iAddedGroup );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        std::vector<FOdysseyVectorGroup*> mRemovedGroupArray;
        std::vector<FGroupRecord> mGroupRecordArray;
        FOdysseyVectorGroup* mAddedGroup;
        FOdysseyVectorScene* mScene;
        bool mSwitchSpace;
};
