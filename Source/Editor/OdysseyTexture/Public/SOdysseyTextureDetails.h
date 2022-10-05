// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

class ODYSSEYTEXTURE_API SOdysseyTextureDetails : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTextureDetails)
	{}
		SLATE_ATTRIBUTE(UTexture*, Texture)
	SLATE_END_ARGS()
    

	void Construct(const FArguments& InArgs);

private:
    void PopulateQuickInfo();

    FText GetImportedText() const;
    FText GetCurrentText() const;
    FText GetMaxInGameText() const;
	FText GetSizeText() const;
	FText GetMethodText() const;
	FText GetFormatText() const;
	FText GetLODBiasText() const;
	FText GetHasAlphaChannelText() const;
	FText GetNumMipsText() const;

    EVisibility GetHasAlphaChannelVisibility() const;

private:
	virtual void Tick(const FGeometry& AllottedGeometry, const double InCurrentTime, const float InDeltaTime) override;

private:
    TAttribute<UTexture*> mTexture;
	UTexture* mCurrentTexture;
    TSharedPtr<class IDetailsView> mTexturePropertiesWidget;
};
