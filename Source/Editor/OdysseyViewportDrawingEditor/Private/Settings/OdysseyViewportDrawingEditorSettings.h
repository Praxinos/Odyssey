// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "MeshPaintTypes.h"
#include "MeshPaintSettings.h"

#include "OdysseyViewportDrawingEditorSettings.generated.h"

/** Texture painting settings structure */
USTRUCT()
struct FOdysseyViewportDrawingEditorTexturePaintSettings
{
	GENERATED_BODY()
public:
	FOdysseyViewportDrawingEditorTexturePaintSettings()
		: mPaintTexture(nullptr) {}

	/** Texture to which Painting should be Applied */
	UPROPERTY(EditAnywhere, Category = "Odyssey Viewport Drawing Editor", meta=(DisplayThumbnail="true"))
	UTexture2D* mPaintTexture;
};

/** Paint mode settings class derives from base mesh painting settings */
UCLASS(Config=EditorPerProjectUserSettings)
class UOdysseyViewportDrawingEditorSettings : public UMeshPaintSettings
{
	GENERATED_UCLASS_BODY()
public:

	static UOdysseyViewportDrawingEditorSettings* Get();

	UPROPERTY(EditAnywhere, Category = "Odyssey Viewport Drawing Editor", Config, meta=(ShowOnlyInnerProperties))
	FOdysseyViewportDrawingEditorTexturePaintSettings mTexturePaintSettings;
};
