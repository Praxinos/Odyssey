// IDDN.FR.001.060015.008.S.X.2019.000.00000
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
    virtual bool FormatName( const UObject* iContext, FString& oPatternFormatted ) PURE_VIRTUAL( UNamingFormatter::FormatName, return false; );
    virtual bool FormatName( const UObject* iContext, const FString& iPattern, FString& oPatternFormatted ) PURE_VIRTUAL( UNamingFormatter::FormatName, return false; );
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
