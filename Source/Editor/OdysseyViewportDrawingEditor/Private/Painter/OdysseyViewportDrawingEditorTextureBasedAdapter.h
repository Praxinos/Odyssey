// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "IOdysseyViewportDrawingEditorAdapter.h"

/** Painting adapter for the painter. Describes the method of texture based painting in the viewport*/
class FOdysseyViewportDrawingEditorTextureBasedAdapter : public IOdysseyViewportDrawingEditorAdapter
{
public:
	/** destructor */
	~FOdysseyViewportDrawingEditorTextureBasedAdapter();

	/** constructor */
	FOdysseyViewportDrawingEditorTextureBasedAdapter(TSharedPtr<FOdysseyViewportDrawingEditor> iEditor);

public:      
    virtual void PrepareAdapterForPainting() override;
    virtual void StartPainting() override;
	virtual void Paint() override;
    virtual void FinishPainting() override;

    virtual void Tick() override;
};
