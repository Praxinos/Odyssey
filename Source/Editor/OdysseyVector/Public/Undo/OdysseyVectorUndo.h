#pragma once

#include "CoreMinimal.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"

#include <blend2d.h>
#include <Core/Core.h>
#include <Image/Block.h>

class ODYSSEYVECTOR_API FOdysseyVectorUndo : public FCommandChange
{
    public:
        DECLARE_MULTICAST_DELEGATE_OneParam(FRefreshDelegate,FOdysseyVectorScene*)
        FRefreshDelegate mRefreshDelegate;

    public:
        ~FOdysseyVectorUndo();
        FOdysseyVectorUndo();

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        //virtual FString ToString() const override;

    protected:
        bool mApplied;
};
