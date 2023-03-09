#pragma once

#include "CoreMinimal.h"

#include <blend2d.h>
#include <ULIS>
#include "OdysseyVectorVertexCubic.h"
#include "OdysseyVectorSegmentCubic.h"
#include "OdysseyVectorPathCubic.h"

#include "OdysseyVectorEllipse.generated.h"

UCLASS()
class ODYSSEYVECTOR_API UOdysseyVectorEllipse : public UOdysseyVectorPathCubic
{
    public:
        GENERATED_BODY()

    public:
       /**
         * @brief Static function to allocate a new ellipse. Note: this is the proper way to allocate a new ellipse as we don't
         * use the constructor to set parameters because UOBJECTs must have empty constructors.
         * @param iName object's name
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        static UOdysseyVectorEllipse* New( std::string iName, double iRadiusX, double iRadiusY );

       /**
         * @brief destructor.
         */
        ~UOdysseyVectorEllipse();

       /**
         * @brief constructor.
         */
         UOdysseyVectorEllipse();

       /**
         * @brief Init an ellipse.
         * @param iName object's name
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        void Init( std::string iName, double iRadiusX, double iRadiusY );

       /**
         * @brief Set ellipse radius on both X and Y axis.
         * @param iRadius the radius.
         */
        void SetRadius( double iRadius );

       /**
         * @brief Set ellipse radius.
         * @param iRadiusX radius on X axis
         * @param iRadiusY radius on Y axis
         */
        void SetRadius( double iRadiusX, double iRadiusY );

       /**
         * @brief Get ellipse's radius on X axis.
         * @return radius on X axis
         */
        double GetRadiusX();

       /**
         * @brief Get ellipse's radius on Y axis.
         * @return radius on Y axis
         */
        double GetRadiusY();

       /**
         * @brief Convert this ellipse to cubic path.
         * @return a newly allocated cubic path that looks the same as this ellipse.
         */
        UOdysseyVectorPathCubic* Convert();

       /**
         * @brief Get object type
         * @return VECTORELLIPSETYPE.
         */
        virtual uint32 GetType() override;

    protected:
       /**
         * @brief Copy this ellipse (for copy-paste operations).
         * @return a newly allocated ellipse that looks the same as this ellipse.
         */
        virtual UOdysseyVectorObject* CopyShape() override;

        virtual void DrawShape( ::ULIS::FRectD &iRoi, uint64 iFlags ) override;
        virtual bool PickShape( ::ULIS::FRectD& iRoi, uint32 iSelectionFlags ) override;
        virtual void UpdateShape( uint32 iUpdateFlags ) override;

    protected :
        FOdysseyVectorVertexCubic* mCubicVertex[4];
        FOdysseyVectorSegmentCubic* mCubicSegment[4];
        double mRadiusX; 
        double mRadiusY; 
        double mStrokeWidth;


 
};
