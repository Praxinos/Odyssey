// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyPainterEditorRasterLiquifyToolUndo.h"
#include "Misc/OdysseyUndoDelegates.h"

FOdysseyPainterEditorRasterLiquifyToolUndo::~FOdysseyPainterEditorRasterLiquifyToolUndo()
{
/*
    if( mApplied )
    {
        // nothing to do
    }
    else
    {
        // nothing to do
    }
*/
}

FOdysseyPainterEditorRasterLiquifyToolUndo::FOdysseyPainterEditorRasterLiquifyToolUndo( TObjectPtr<UOdysseyPainterEditorRasterLiquifyTool> iLiquifyTool )
    : mLiquifyTool ( iLiquifyTool )
{
}

void
FOdysseyPainterEditorRasterLiquifyToolUndo::Apply( UObject* iIgnored )
{
    FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
        [this]( bool iIsRedo )
        {
            if( mLiquifyTool->IsValidLowLevel() )
            {
                mLiquifyTool->Init();
            }
        }
    );
}

void
FOdysseyPainterEditorRasterLiquifyToolUndo::Revert( UObject* iIgnored )
{
    FOdysseyUndoDelegates::Get().OnAfterUndoRedo().AddLambda(
        [this]( bool iIsRedo )
        {
            if( mLiquifyTool->IsValidLowLevel() )
            {
                mLiquifyTool->Init();
            }
        }
    );
}

/** Describes this change (for debugging) */
FString
FOdysseyPainterEditorRasterLiquifyToolUndo::ToString() const
{
    return FString("FOdysseyPainterEditorRasterLiquifyToolUndo");
}
