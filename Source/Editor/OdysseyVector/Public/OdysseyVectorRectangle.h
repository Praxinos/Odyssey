#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorRectangle.generated.h"

UCLASS()
class UOdysseyVectorRectangle : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        UOdysseyVectorObject* PickShape( double iX, double iY, double iRadius );
        UOdysseyVectorObject* CopyShape();
        void UpdateShape() {};

    protected :
        double mWidth;
        double mHeight; 

    public:
        ~UOdysseyVectorRectangle();
        UOdysseyVectorRectangle() {};
        void Init( std::string iName, double iWidth, double iHeight );
        void SetSize( double iWidth, double iHeight );
        double GetWidth();
        double GetHeight();
};
