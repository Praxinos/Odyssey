// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
#pragma once

#include "CoreMinimal.h"
#include "Widgets/SWidget.h"
#include "PropertyPath.h"
#include "OdysseyDiffUtils.h"

class IDetailsView;

class ODYSSEYBRUSHEDITOR_API FDetailsDiff
{
public:
    DECLARE_DELEGATE( FOnDisplayedPropertiesChanged );

    FDetailsDiff( const UObject* InObject, FOnDisplayedPropertiesChanged InOnDisplayedPropertiesChanged );
    ~FDetailsDiff();

    void HighlightProperty( const FPropertySoftPath& PropertyName );
    TSharedRef< SWidget > DetailsWidget();
    TArray<FPropertySoftPath> GetDisplayedProperties() const;

    void DiffAgainst(const FDetailsDiff& Newer, TArray< FSingleObjectDiffEntry > &OutDifferences) const;

private:
    void HandlePropertiesChanged();

    FOnDisplayedPropertiesChanged OnDisplayedPropertiesChanged;

    TArray< FPropertyPath > DifferingProperties;
    const UObject* DisplayedObject;

    TSharedPtr< class IDetailsView > DetailsView;
};
