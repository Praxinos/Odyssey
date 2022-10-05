// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022
#pragma once
#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include <ULIS>

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
    FOdysseyBrushRect( const ::ULIS::FRectI& Rect );
	FOdysseyBrushRect( ::ULIS::FRectI&& iVal );
    FOdysseyBrushRect( const FOdysseyBrushRect& iOther );
    FOdysseyBrushRect( FOdysseyBrushRect&& iOther );
    FOdysseyBrushRect& operator=( const FOdysseyBrushRect& iOther );

    const ::ULIS::FRectI& GetValue() const;
    void SetValue(const ::ULIS::FRectI&);
    void SetValue(int iX, int iY, int iW, int iH);
    bool IsInitialized() const;
    
	int X() const;
	int Y() const;
	int Width() const;
	int Height() const;

    static  FOdysseyBrushRect FromTemp( ::ULIS::FRectI&& iVal );
private:
    ::ULIS::FRectI m;
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

