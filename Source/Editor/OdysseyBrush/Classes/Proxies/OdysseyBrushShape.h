// IDDN.FR.001.250001.006.S.P.2019.000.00000
// ILIAD is subject to copyright laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2022

#pragma once

#include "CoreTypes.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "UObject/ObjectMacros.h"
#include "OdysseyBrushShape.generated.h"

UCLASS(meta = (ScriptName = "OdysseyBrushShape"))
class ODYSSEYBRUSH_API UOdysseyBrushShape : public UBlueprintFunctionLibrary
{
    GENERATED_BODY()

public:
    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateLinePoints(UPARAM(DisplayName = "Start") FVector2D StartPoint,
            UPARAM(DisplayName = "End") FVector2D  EndPoint,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateArcPoints(UPARAM(DisplayName = "Center") FVector2D Center,
            UPARAM(DisplayName = "Radius") int Radius,
            UPARAM(DisplayName = "Start Angle (Degree)") int StartDegree,
            UPARAM(DisplayName = "End Angle (Degree)") int EndDegree,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateCirclePoints(UPARAM(DisplayName = "Center") FVector2D Center,
            UPARAM(DisplayName = "Radius") int Radius,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateEllipsePoints(UPARAM(DisplayName = "Center") FVector2D Center,
            UPARAM(DisplayName = "A Radius") int A,
            UPARAM(DisplayName = "B Radius") int B,
            UPARAM(DisplayName = "Rotation (degrees)") int Rotation,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateQuadraticBezierPoints(UPARAM(DisplayName = "Start") FVector2D Start,
            UPARAM(DisplayName = "Control Point") FVector2D ControlPoint,
            UPARAM(DisplayName = "End") FVector2D End,
            UPARAM(DisplayName = "Weight") float iWeight,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);
    
    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GenerateRectanglePoints(UPARAM(DisplayName = "Top Left Corner") FVector2D TopLeft,
            UPARAM(DisplayName = "Bottom Right Corner") FVector2D BottomRight,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

    //Node 
    UFUNCTION(BlueprintPure, Category = "Odyssey|Shape")
        static  void  GeneratePolygonPoints(UPARAM(DisplayName = "Polygon Points") TArray<FVector2D> PolygonPoints,
            UPARAM(DisplayName = "Generated Points") TArray<FVector2D>& GeneratedPoints);

};