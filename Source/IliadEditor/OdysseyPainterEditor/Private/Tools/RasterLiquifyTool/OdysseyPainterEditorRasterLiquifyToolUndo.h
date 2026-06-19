// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "Misc/Change.h"
#include "Misc/ITransaction.h"
#include "OdysseyPainterEditorRasterLiquifyTool.h"

class ODYSSEYPAINTEREDITOR_API FOdysseyPainterEditorRasterLiquifyToolUndo : public FCommandChange
{
    public:
        ~FOdysseyPainterEditorRasterLiquifyToolUndo();
        FOdysseyPainterEditorRasterLiquifyToolUndo( TObjectPtr<UOdysseyPainterEditorRasterLiquifyTool> iLiquifyTool );

        /** Called when redoing */
        virtual void Apply( UObject* iIgnored ) override;

        /** called when undoing */
        virtual void Revert( UObject* iIgnored ) override;

        /** Describes this change (for debugging) */
        virtual FString ToString() const override;

    private:
        TObjectPtr<UOdysseyPainterEditorRasterLiquifyTool> mLiquifyTool;
        UOdysseyPainterEditorRasterLiquifyTool::FFlowMap mPartialFlowMap;
        uint32 mPositionX;
        uint32 mPositionY;
};
