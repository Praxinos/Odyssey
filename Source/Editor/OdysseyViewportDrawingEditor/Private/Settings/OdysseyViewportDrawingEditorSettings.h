// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc

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
