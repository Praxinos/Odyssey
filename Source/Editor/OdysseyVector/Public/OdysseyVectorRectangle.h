#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include "OdysseyVectorObject.h"
#include "OdysseyVectorSegment.h"

#include "OdysseyVectorRectangle.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorRectangle : public UOdysseyVectorObject
{
    public:
        GENERATED_BODY()

    private:
        void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags );
        bool PickShape( ::ULIS::FRectD &iRoi, uint32 iSelectionFlags );
        UOdysseyVectorObject* CopyShape();
        void UpdateShape() {};

    protected :
        double mWidth;
        double mHeight; 
        double mStrokeWidth;

    public:
        ~UOdysseyVectorRectangle();
        UOdysseyVectorRectangle();
        void Init( std::string iName, double iWidth, double iHeight );
        void SetSize( double iWidth, double iHeight );
        double GetWidth();
        double GetHeight();
};
