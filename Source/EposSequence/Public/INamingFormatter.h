// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "INamingFormatter.generated.h"

UCLASS(abstract)
class UNamingFormatter
    : public UObject
{
    GENERATED_BODY()

public:
    virtual FString FormatName( const UObject* iContext ) PURE_VIRTUAL( UNamingFormatter::FormatName, return TEXT("no-name"); );
};

UCLASS(abstract)
class EPOSSEQUENCE_API UNamingFormatterBoard
    : public UNamingFormatter
{
    GENERATED_BODY()

};

UCLASS(abstract)
class EPOSSEQUENCE_API UNamingFormatterShot
    : public UNamingFormatter
{
    GENERATED_BODY()

};
