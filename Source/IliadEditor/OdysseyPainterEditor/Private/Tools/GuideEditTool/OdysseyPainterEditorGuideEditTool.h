// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2026

#pragma once

#include "Tools/OdysseyPainterEditorTool.h"
#include "OdysseyPainterEditorGuideCollection.h"
#include "ScopedTransaction.h"
#include "OdysseyPainterEditorGuideEditTool.generated.h"

/** Temporary tool for editing the selected guide's geometry through viewport handles. */
UCLASS()
class ODYSSEYPAINTEREDITOR_API UOdysseyPainterEditorGuideEditTool : public UOdysseyPainterEditorTool
{
    GENERATED_BODY()

public:
    UOdysseyPainterEditorGuideEditTool();

    void SetGuide(UOdysseyPainterEditorGuide* InGuide);
    UOdysseyPainterEditorGuide* GetGuide() const;

    /** The tool can activate only when its guide is visible, unlocked, and has an editor source. */
    virtual bool IsActivable() const override;
    virtual void Inactivate() override;
    virtual FText GetTooltip() const override;

protected:
    //OdysseyPainterEditorTool overrides
    virtual bool OnKeyDown(const FKey& Key) override;

private:
    friend class FOdysseyPainterEditorGuideEditHUD;

    /** Starts, updates, and commits a drag initiated by one of the HUD handles. */
    void BeginHandleDrag(int32 HandleIndex);
    void DragHandle(const FVector2D& TexturePosition);
    void EndHandleDrag();

    /** Guide held while this temporary tool is active. */
    UPROPERTY(Transient)
    TObjectPtr<UOdysseyPainterEditorGuide> Guide;

    /** Index of the handle currently being dragged, or INDEX_NONE when idle. */
    int32 ActiveHandle = INDEX_NONE;

    /** Transaction kept open for the duration of a handle drag. */
    TUniquePtr<FScopedTransaction> Transaction;
};
