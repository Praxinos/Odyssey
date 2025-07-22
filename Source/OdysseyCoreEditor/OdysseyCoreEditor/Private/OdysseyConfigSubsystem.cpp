// IDDN.FR.001.060015.014.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyConfigSubsystem.h"
#include "CoreGlobals.h"
#include "Misc/ConfigCacheIni.h"

bool
UOdysseyConfigSubsystem::GetConfigString( const FString& Section, const FString& Key, FString& Value, const FString& Filename) const
{
    return GConfig->GetString(*Section, *Key, Value, Filename);
}

bool
UOdysseyConfigSubsystem::GetConfigText( const FString& Section, const FString& Key, FText& Value, const FString& Filename) const
{
    return GConfig->GetText(*Section, *Key, Value, Filename);
}

bool
UOdysseyConfigSubsystem::GetConfigInt(const FString& Section, const FString& Key, int32& Value, const FString& Filename) const
{
    return GConfig->GetInt(*Section, *Key, Value, Filename);
}

bool
UOdysseyConfigSubsystem::GetConfigFloat(const FString& Section, const FString& Key, float& Value, const FString& Filename) const
{
    return GConfig->GetFloat(*Section, *Key, Value, Filename);
}

bool
UOdysseyConfigSubsystem::GetConfigDouble(const FString& Section, const FString& Key, double& Value, const FString& Filename) const
{
    return GConfig->GetDouble(*Section, *Key, Value, Filename);
}

bool
UOdysseyConfigSubsystem::GetConfigBool( const FString& Section, const FString& Key, bool& Value, const FString& Filename) const
{
    return GConfig->GetBool(*Section, *Key, Value, Filename);
}

int32
UOdysseyConfigSubsystem::GetConfigArray(const FString& Section, const FString& Key, TArray<FString>& Value, const FString& Filename) const
{
    return GConfig->GetArray(*Section, *Key, Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigString(const FString& Section, const FString& Key, const FString& Value, const FString& Filename)
{
    GConfig->SetString(*Section, *Key, *Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigText(const FString& Section, const FString& Key, const FText& Value, const FString& Filename)
{
    GConfig->SetText(*Section, *Key, Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigFloat(const FString& Section, const FString& Key, float Value, const FString& Filename)
{
    GConfig->SetFloat(*Section, *Key, Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigDouble(const FString& Section, const FString& Key, double Value, const FString& Filename)
{
    GConfig->SetDouble(*Section, *Key, Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigBool(const FString& Section, const FString& Key, bool Value, const FString& Filename)
{
    GConfig->SetBool(*Section, *Key, Value, Filename);
}

void
UOdysseyConfigSubsystem::SetConfigArray(const FString& Section, const FString& Key, const TArray<FString>& Value, const FString& Filename)
{
    GConfig->SetArray(*Section, *Key, Value, Filename);
}
