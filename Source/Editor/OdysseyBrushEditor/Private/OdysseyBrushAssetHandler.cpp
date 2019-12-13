// Copyright © 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.
// IDDN FR.001.250001.002.S.P.2019.000.00000

#include "OdysseyBrushAssetHandler.h"
#include "Engine/World.h"
#include "Engine/LevelScriptBlueprint.h"
#include "AssetData.h"

class FLevelOdysseyBrushAssetHandler : public IOdysseyBrushAssetHandler
{
    virtual UBlueprint* RetrieveOdysseyBrush(UObject* InObject) const override
    {
        UWorld* World = CastChecked<UWorld>(InObject);

        const bool bCreateLevelScript = false;
        return World->PersistentLevel ? World->PersistentLevel->GetLevelScriptBlueprint(bCreateLevelScript) : nullptr;
    }

    virtual bool AssetContainsOdysseyBrush(const FAssetData& InAssetData) const
    {
        static FName FiB("FiB");
        // Worlds are only considered to contain a blueprint if they have FiB data
        //ODYSSEY: PATCH
        //return InAssetData.TagsAndValues.Find(FiB) || InAssetData.TagsAndValues.Find(FBlueprintTags::FindInBlueprintsData);
        return InAssetData.TagsAndValues.FindTag(FiB).IsSet() || InAssetData.TagsAndValues.FindTag(FBlueprintTags::FindInBlueprintsData).IsSet();
    }
};


class FOdysseyBrushAssetTypeHandler : public IOdysseyBrushAssetHandler
{
    virtual UBlueprint* RetrieveOdysseyBrush(UObject* InObject) const override
    {
        // The object is the blueprint for UBlueprint (and derived) assets
        return CastChecked<UBlueprint>(InObject);
    }

    virtual bool AssetContainsOdysseyBrush(const FAssetData& InAssetData) const
    {
        return true;
    }
};

FOdysseyBrushAssetHandler::FOdysseyBrushAssetHandler()
{
    // Register default handlers
    RegisterHandler<FLevelOdysseyBrushAssetHandler>(UWorld::StaticClass()->GetFName());
    RegisterHandler<FOdysseyBrushAssetTypeHandler>(UBlueprint::StaticClass()->GetFName());
}

FOdysseyBrushAssetHandler& FOdysseyBrushAssetHandler::Get()
{
    static FOdysseyBrushAssetHandler Singleton;
    return Singleton;
}

void FOdysseyBrushAssetHandler::RegisterHandler(FName EligibleClass, TUniquePtr<IOdysseyBrushAssetHandler>&& InHandler)
{
    ClassNames.Add(EligibleClass);
    Handlers.Add(MoveTemp(InHandler));
}

const IOdysseyBrushAssetHandler* FOdysseyBrushAssetHandler::FindHandler(const UClass* InClass) const
{
    UClass* StopAtClass = UObject::StaticClass();
    while (InClass && InClass != StopAtClass)
    {
        int32 Index = ClassNames.IndexOfByKey(InClass->GetFName());
        if (Index != INDEX_NONE)
        {
            return Handlers[Index].Get();
        }

        InClass = InClass->GetSuperClass();
    }

    return nullptr;
}
