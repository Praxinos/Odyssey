// Copyright 1998-2019 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "MeshPaintTypes.h"
#include "MeshPaintSettings.h"

#include "OdysseyViewportDrawingEditorSettings.generated.h"

UENUM()
enum class EOdysseyViewportDrawingEditorTextureWeightTypes : uint8
{
	/** Lerp Between Two Textures using Alpha Value */
	AlphaLerp = 2 UMETA(DisplayName = "Alpha (Two Textures)"),

	/** Weighting Three Textures according to Channels*/
	RGB = 3 UMETA(DisplayName = "RGB (Three Textures)"),

	/**  Weighting Four Textures according to Channels*/
	ARGB = 4 UMETA(DisplayName = "ARGB (Four Textures)"),

	/**  Weighting Five Textures according to Channels */
	OneMinusARGB = 5 UMETA(DisplayName = "ARGB - 1 (Five Textures)")
};

UENUM()
enum class EOdysseyViewportDrawingEditorTexturePaintIndex : uint8
{
	TextureOne = 0,
	TextureTwo,
	TextureThree,
	TextureFour,
	TextureFive
};

/** Texture painting settings structure */
USTRUCT()
struct FOdysseyViewportDrawingEditorTexturePaintSettings
{
	GENERATED_BODY()
public:
	FOdysseyViewportDrawingEditorTexturePaintSettings()
		: mPaintTexture(nullptr) {}

	/** Texture to which Painting should be Applied */
	UPROPERTY(EditAnywhere, meta=(DisplayThumbnail="true"))
	UTexture2D* mPaintTexture;
};

/** Paint mode settings class derives from base mesh painting settings */
UCLASS(Config=EditorPerProjectUserSettings)
class UOdysseyViewportDrawingEditorSettings : public UMeshPaintSettings
{
	GENERATED_UCLASS_BODY()
public:

	static UOdysseyViewportDrawingEditorSettings* Get();

	UPROPERTY(EditAnywhere, Config, meta=(ShowOnlyInnerProperties))
	FOdysseyViewportDrawingEditorTexturePaintSettings mTexturePaintSettings;
};
