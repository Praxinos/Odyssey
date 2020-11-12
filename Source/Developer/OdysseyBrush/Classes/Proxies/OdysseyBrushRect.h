// IDDN FR.001.250001.004.S.X.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc
#pragma once
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include <ULIS3>

#include "OdysseyBrushRect.generated.h"

/////////////////////////////////////////////////////
// Odyssey Brush Rectangle
USTRUCT(BlueprintType, meta = (DisplayName="Odyssey Rectangle"))
struct ODYSSEYBRUSH_API FOdysseyBrushRect
{
public:
    GENERATED_BODY()

    FOdysseyBrushRect();
    FOdysseyBrushRect( int X, int Y, int Width, int Height );
    FOdysseyBrushRect( const ::ul3::FRect& Rect );
	FOdysseyBrushRect( ::ul3::FRect&& iVal );
    FOdysseyBrushRect( const FOdysseyBrushRect& iOther );
    FOdysseyBrushRect( FOdysseyBrushRect&& iOther );
    FOdysseyBrushRect& operator=( const FOdysseyBrushRect& iOther );

    const ::ul3::FRect& GetValue() const;
    void SetValue(const ::ul3::FRect&);
    void SetValue(int iX, int iY, int iW, int iH);
    bool IsInitialized() const;
    
	int X() const;
	int Y() const;
	int Width() const;
	int Height() const;

    static  FOdysseyBrushRect FromTemp( ::ul3::FRect&& iVal );
private:
    ::ul3::FRect m;
	bool mIsInitialized;
};

/////////////////////////////////////////////////////
// UOdysseyBrushRectFunctionLibrary
UCLASS(meta=(ScriptName="OdysseyBrushRectLibrary"))
class ODYSSEYBRUSH_API UOdysseyBrushRectFunctionLibrary : public UBlueprintFunctionLibrary
{
    GENERATED_UCLASS_BODY()

    //Create an Odyssey Rectangle from its X, Y, W and H components.
    UFUNCTION(BlueprintPure, Category="Odyssey|Math", meta = (DisplayName="Make Odyssey Rectangle"))
    static FOdysseyBrushRect MakeFromXYWH(int X, int Y, int Width, int Height);

    //Create an Odyssey Rectangle from its X, Y, W and H components.
    UFUNCTION(BlueprintPure, Category="Odyssey|Math", meta = (DisplayName="Break Odyssey Rectangle"))
    static void BreakToXYWH(FOdysseyBrushRect Rectangle, int& X, int& Y, int& Width, int& Height);
};

