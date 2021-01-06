// Copyright © 2018-2020 Praxinos, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#pragma once

#include "CoreMinimal.h"

class ODYSSEYTEXTURE_API SOdysseyTextureDetails : public SCompoundWidget
{
public:
    SLATE_BEGIN_ARGS(SOdysseyTextureDetails)
	{}
		SLATE_ARGUMENT(UTexture*, Texture)
	SLATE_END_ARGS()
    

	void Construct(const FArguments& InArgs);

public:
    void SetTexture(UTexture* iTexture);

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
    UTexture* mTexture;
    TSharedPtr<class IDetailsView> mTexturePropertiesWidget;
};
