// Copyright 2018-2019 Praxinos, Inc. All Rights Reserved.
// Copyright 1998-2018 Epic Games, Inc. All Rights Reserved.

#pragma once

#include "CoreTypes.h"
#include "Templates/UniquePtr.h"
#include "Containers/ArrayView.h"
#include "Containers/Array.h"
#include "UObject/NameTypes.h"

struct FAssetData;

class UBlueprint;
class UObject;
class UClass;

/**
 * Interface used to define how to interact with a blueprint within an asset
 */
class IOdysseyBrushAssetHandler
{
public:
    virtual ~IOdysseyBrushAssetHandler() {}

    /**
     * Retrieve the blueprint from the specified asset object
     *
     *@param InAsset         The asset object to retrieve the blueprint from
     *@return The blueprint contained within the specified asset, or nullptr if non exists
     */
    virtual UBlueprint* RetrieveOdysseyBrush(UObject* InAsset) const = 0;


    /**
     * Check whether the specified asset registry data contains a blueprint
     *
     *@param InAssetData     The asset object to retrieve the blueprint from
     *@return True if the asset contains a blueprint, false otherwise
     */
    virtual bool AssetContainsOdysseyBrush(const FAssetData& InAssetData) const = 0;


    /**
     * Check whether the specified asset supports nativization
     *
     *@param InAsset         The asset that is being queired for nativization support
     *@param InOdysseyBrush     The blueprint that is contained within InAsset
     *@param OutReason       (Optional) An optional failure text to set
     *@return true if the specified asset supports nativization, false otherwise
     */
    virtual bool SupportsNativization(const UObject* InAsset, const UBlueprint* InOdysseyBrush, FText* OutReason) const { return true; }
};


/**
 * Singleton class that marshals different blueprint asset handlers for different asset class types
 */
class FOdysseyBrushAssetHandler
{
public:

    /**
     * Retrieve the singleton instance of this class
     */
    ODYSSEYBRUSHEDITOR_API static FOdysseyBrushAssetHandler& Get();


    /**
     * Get all the currently registered class names
     */
    TArrayView<const FName> GetRegisteredClassNames() const
    {
        return ClassNames;
    }


    /**
     * Register an asset for the specified class name
     * @note: Any assets whose class is a child of the specified class will use this handler (unless there is a more specific handler registered)
     *
     * @param ClassName      The name of the class this handler relates to.
     */
    template<typename HandlerType>
    void RegisterHandler(FName ClassName)
    {
        RegisterHandler(ClassName, MakeUnique<HandlerType>());
    }


    /**
     * Register an asset for the specified class name
     * @note: Any assets whose class is a child of the specified class will use this handler (unless there is a more specific handler registered)
     *
     * @param ClassName      The name of the class this handler relates to.
     * @param InHandler      The implementation of the handler to use
     */
    ODYSSEYBRUSHEDITOR_API void RegisterHandler(FName ClassName, TUniquePtr<IOdysseyBrushAssetHandler>&& InHandler);


    /**
     * Find a handler that applies to the specified class
     *
     * @param InClass        The class to find a handler for
     * @return A valid asset handler, or nullptr if none exists for this class
     */
    ODYSSEYBRUSHEDITOR_API const IOdysseyBrushAssetHandler* FindHandler(const UClass* InClass) const;


private:

    /** Private constructor - use singleton accessor (::Get) */
    FOdysseyBrushAssetHandler();

    /** Unsorted array of class names, one per-handler below */
    TArray<FName> ClassNames;

    /** Array of handlers that relate to the class names above */
    TArray<TUniquePtr<IOdysseyBrushAssetHandler>> Handlers;
};
