// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#include "OdysseyViewportDrawingEditorEdMode.h"
#include "EdMode.h"
#include "OdysseyViewportDrawingEditorEdModeToolkit.h"
#include "EditorModeManager.h"

#include "OdysseyViewportDrawingEditorPainter.h"

const FEditorModeID FOdysseyViewportDrawingEditorEdMode::EM_OdysseyViewportDrawingEditorEdModeId = TEXT("EM_OdysseyViewportDrawingEditorEdMode");

void FOdysseyViewportDrawingEditorEdMode::Initialize()
{
	MeshPainter = FOdysseyViewportDrawingEditorPainter::Get();
}

TSharedPtr<class FModeToolkit> FOdysseyViewportDrawingEditorEdMode::GetToolkit()
{
	return MakeShareable(new FOdysseyViewportDrawingEditorEdModeToolkit(this));
}

bool FOdysseyViewportDrawingEditorEdMode::InputKey(FEditorViewportClient* iViewportClient, FViewport* iViewport, FKey iKey, EInputEvent iEvent)
{
	if (!IsEditingEnabled())
	{
		return false;
	}

	return IMeshPaintEdMode::InputKey( iViewportClient, iViewport, iKey, iEvent);
}

bool FOdysseyViewportDrawingEditorEdMode::IsEditingEnabled() const
{
	return GetWorld() ? GetWorld()->FeatureLevel >= ERHIFeatureLevel::SM5 : false;
}

void FOdysseyViewportDrawingEditorEdMode::Enter()
{
	FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeEnter();
	IMeshPaintEdMode::Enter();
}

void FOdysseyViewportDrawingEditorEdMode::Exit()
{
	FOdysseyViewportDrawingEditorPainter::Get()->GetController()->EdModeExit();
	IMeshPaintEdMode::Exit();
}
