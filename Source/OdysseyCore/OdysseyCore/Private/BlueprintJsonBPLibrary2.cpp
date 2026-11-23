// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2025

#include "BlueprintJsonBPLibrary2.h"

#include "Dom/JsonValue.h"
#include "Dom/JsonObject.h"
#include "JsonObjectWrapper.h"

UBlueprintJsonBPLibrary2::UBlueprintJsonBPLibrary2(const FObjectInitializer& ObjectInitializer)
    : Super(ObjectInitializer)
{
}

bool UBlueprintJsonBPLibrary2::GetFieldAsArrayArrayFloat( const FJsonObjectWrapper& JsonObject, const FString& FieldName, UPARAM( DisplayName = "Value" ) TArray<FJsonFloatArray>& OutValue )
{
    OutValue.Reset();

    // Make sure the wrapper contains a valid JSON object.
    if( !JsonObject.JsonObject.IsValid() )
        return false;

    const TArray<TSharedPtr<FJsonValue>>* OuterArray = nullptr;

    if( !JsonObject.JsonObject->TryGetArrayField( FieldName, OuterArray ) )
        return false;

    for( const TSharedPtr<FJsonValue>& RowValue : *OuterArray )
    {
        if( !RowValue.IsValid()
            || RowValue->Type != EJson::Array )
        {
            OutValue.Reset();
            return false;
        }

        const TArray<TSharedPtr<FJsonValue>>* InnerArray = &RowValue->AsArray();

        FJsonFloatArray Row;

        for( const TSharedPtr<FJsonValue>& Value : *InnerArray )
        {
            if( !Value.IsValid()
                || Value->Type != EJson::Number )
            {
                OutValue.Reset();
                return false;
            }

            Row.Values.Add( static_cast<float>( Value->AsNumber() ) );
        }

        OutValue.Add( MoveTemp( Row ) );
    }

    return true;
}
