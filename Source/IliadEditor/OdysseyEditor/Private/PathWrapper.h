// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"

#include "PathWrapper.generated.h"

//---

// https://udn.unrealengine.com/s/question/0D5QP00000Ozf9p0AB/editor-utility-widget-how-to-use-a-widget-that-allow-to-select-a-path-in-the-contentbrowser
/**
 * A directory path wrapper to be able to choose a directory path relative to the content browser root
 */
UCLASS( BlueprintType )
class UDirectoryPathInContent:
    public UObject
{
    GENERATED_BODY()

public:
    /**
     * A path relative to content browser
     */
    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Path", Meta=(ContentDir) )
    FDirectoryPath Path;
};


//// This should be easier to use in BP (as it is a basic struct)
//// BUT the FDirectoryPathStructCustomization class must be duplicated/registered to manage the same GUI
///**
// * A directory path wrapper to be able to choose a directory path relative to the content browser root
// */
//USTRUCT( BlueprintType )
//struct FDirectoryPathInContent
//{
//    GENERATED_BODY()
//
//public:
//    /**
//     * A path relative to content browser
//     */
//    UPROPERTY( EditAnywhere, BlueprintReadWrite, Category="Path", Meta=(ContentDir) )
//    FDirectoryPath Path;
//};
