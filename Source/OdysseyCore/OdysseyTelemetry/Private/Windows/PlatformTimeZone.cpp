// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "PlatformTimeZone.h"

#include "Interfaces/IPluginManager.h"
#include "Misc/FileHelper.h"
#include "Serialization/JsonSerializer.h"
#include "Windows/WindowsHWrapper.h"

FString
FPlatformTimeZone::GetTimeZoneId()
{
    static TMap<FString, TArray<FString>> WindowsIdToIanaIdsMap;
    if( WindowsIdToIanaIdsMap.IsEmpty() )
    {
        TSharedPtr<IPlugin> Plugin = IPluginManager::Get().FindPlugin( UE_PLUGIN_NAME );
        // This file map the windows id (of the timezone) to the iana id ("offical" timezone id and matching the one used on MacOSX/Linux)
        // There can be multiple iana ids (separated by a space) for one windows id!
        // For example:
        // - "Romance Standard Time" -> "Europe/Paris"
        // - "Alaskan Standard Time" -> "America/Anchorage America/Juneau America/Metlakatla America/Nome America/Sitka America/Yakutat"
        //
        // This file should (must?) be updated from time to time (?)
        // https://raw.githubusercontent.com/unicode-org/cldr-json/main/cldr-json/cldr-core/supplemental/windowsZones.json
        FString TimeZoneIdMapPathfile = Plugin->GetBaseDir() / TEXT( "Resources" ) / TEXT( "OdysseyAssetResources" ) / TEXT( "TimeZoneIdMap.json" );

        FString content;
        if( !FFileHelper::LoadFileToString( content, *TimeZoneIdMapPathfile ) )
            return FString();

        TSharedRef<TJsonReader<TCHAR>> JsonReader = TJsonReaderFactory<TCHAR>::Create( content );
        TSharedPtr<FJsonObject> RootObject;

        if( !FJsonSerializer::Deserialize( JsonReader, RootObject ) )
            return FString();
        if( !RootObject )
            return FString();

        // Get the array of all timezones
        // (It assumes that all tags exist!)
        const TArray<TSharedPtr<FJsonValue>>& MapTimezones = RootObject->GetObjectField( TEXT( "supplemental" ) )->GetObjectField( TEXT( "windowsZones" ) )->GetArrayField( TEXT( "mapTimezones" ) );
        for( const TSharedPtr<FJsonValue>& Value : MapTimezones )
        {
            const TSharedPtr<FJsonObject>& MapZone = Value->AsObject()->GetObjectField( TEXT( "mapZone" ) );

            FString WindowsId = MapZone->GetStringField( TEXT( "_other" ) );
            FString IanaIdList = MapZone->GetStringField( TEXT( "_type" ) );
            // Convert the iana ids single string to an array of iana ids
            TArray<FString> IanaIds;
            IanaIdList.ParseIntoArrayWS( IanaIds );

            // Fill the map
            WindowsIdToIanaIdsMap.Add( WindowsId, IanaIds );
        }

        // It would be possible to use the automatic json to UStruct convert,
        // but it implies to expose the UStruct in .h
        // And for a simple json structure like this one, just explore directly via the json hierarchy
        //if( !FJsonObjectConverter::JsonObjectToUStruct<T>( OutObject.ToSharedRef(), &oJsonResponse ) )
        //    return false;
    }

    // Get the current timezone (from windows api)
    DYNAMIC_TIME_ZONE_INFORMATION Tzi;
    GetDynamicTimeZoneInformation( &Tzi );

    // Convert it to iana timezone id
    const FString WindowsId = Tzi.TimeZoneKeyName;
    const TArray<FString>* IanaIds = WindowsIdToIanaIdsMap.Find( WindowsId );
    if( !IanaIds || IanaIds->IsEmpty() )
        return FString();

    // Just return the first id as we can't guess the correct iana id only from the windows id
    return (*IanaIds)[0];
}
