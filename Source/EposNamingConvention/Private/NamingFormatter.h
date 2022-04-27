// IDDN.FR.001.220036.001.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreMinimal.h"

#include "INamingFormatter.h"

#include "NamingFormatter.generated.h"

//---

UCLASS()
class UDefaultNamingFormatterBoard
    : public UNamingFormatterBoard
{
    GENERATED_BODY()

public:
    virtual FString FormatName( const UObject* iContext );
};

//---

UCLASS()
class UDefaultNamingFormatterShot
    : public UNamingFormatterShot
{
    GENERATED_BODY()

public:
    virtual FString FormatName( const UObject* iContext );
};
