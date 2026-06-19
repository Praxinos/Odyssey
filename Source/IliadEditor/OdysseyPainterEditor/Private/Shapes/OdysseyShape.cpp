// IDDN.FR.001.060015.015.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#include "OdysseyShape.h"
#include "OdysseyHUDElement.h"
#include "DetailWidgetRow.h"
#include "Widgets/Input/SSegmentedControl.h"
#include "OdysseyStyle.h"
#include "IDetailChildrenBuilder.h"

class FOdysseyShapesDetailCustomization : public IPropertyTypeCustomization
{
public:
    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FOdysseyShapes* GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const;

    EOdysseyShapeType GetSelectedShape() const;
    void OnShapeSelected(EOdysseyShapeType iShape, ECheckBoxState iState);
    const FSlateBrush* GetShapeIcon(EOdysseyShapeType iShape) const;

private:
    FOdysseyShapes* mShapes = nullptr;
    TWeakPtr<IPropertyHandle> mShapesPropertyHandle;
};

void
FOdysseyShapesDetailCustomization::CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) //override
{
    // No header needed (to avoid the collapsing)
}

void
FOdysseyShapesDetailCustomization::CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils )
{
    mShapesPropertyHandle = iStructPropertyHandle;
    mShapes = GetEditStruct( iStructPropertyHandle );

    TSharedPtr<SSegmentedControl<EOdysseyShapeType>> segmentedControl = SNew(SSegmentedControl<EOdysseyShapeType>)
        .Value(this, &FOdysseyShapesDetailCustomization::GetSelectedShape)
        .OnValueChecked(this, &FOdysseyShapesDetailCustomization::OnShapeSelected);

    TArray<EOdysseyShapeType> shapeTypes = mShapes->GetActiveShapeTypes();
    for (EOdysseyShapeType shapeType : shapeTypes)
    {
        FText shapeName = UEnum::GetDisplayValueAsText(shapeType);

        segmentedControl->AddSlot(shapeType)
        .Icon(GetShapeIcon(shapeType))
        .ToolTip(shapeName);
    }

    ioChildBuilder.AddCustomRow( FText() )
    [
        segmentedControl.ToSharedRef()
    ];

    const TMap<EOdysseyShapeType, UOdysseyShape*>& shapes = mShapes->GetShapes();
    for (auto element : shapes)
    {
        EOdysseyShapeType shapeType = element.Key;
        UOdysseyShape* shape = element.Value;

        FAddPropertyParams params;
        params.AllowChildren(true);
        params.CreateCategoryNodes(false);
        params.HideRootObjectNode(true);
        IDetailPropertyRow* row = ioChildBuilder.AddExternalObjects( { shape }, params );

        TAttribute<EVisibility> visibility = MakeAttributeLambda(
            [this, shapeType]() -> EVisibility
            {
                return mShapes->GetActiveShapeType() == shapeType ? EVisibility::Visible : EVisibility::Collapsed;
            }
        );
        row->Visibility(visibility);
    }
}

FOdysseyShapes*
FOdysseyShapesDetailCustomization::GetEditStruct( TSharedRef<IPropertyHandle> iStructPropertyHandle ) const
{
    TArray<FOdysseyShapes*> editStruct;

    if( iStructPropertyHandle->IsValidHandle() )
        iStructPropertyHandle->AccessRawData( reinterpret_cast<TArray<void*>&>( editStruct ) );

    if( editStruct.Num() == 1 )
        return editStruct[0];

    return nullptr;
}

const FSlateBrush*
FOdysseyShapesDetailCustomization::GetShapeIcon(EOdysseyShapeType iShape) const
{
    switch (iShape)
    {
        case EOdysseyShapeType::kNone: return FOdysseyStyle::GetBrush( "Shapes.None" );
        case EOdysseyShapeType::kLine: return FOdysseyStyle::GetBrush( "Shapes.Line" );
        case EOdysseyShapeType::kFreehand: return FOdysseyStyle::GetBrush( "Shapes.Freehand_Filled" );
        case EOdysseyShapeType::kRectangle: return FOdysseyStyle::GetBrush( "Shapes.Rectangle_Filled" );
        case EOdysseyShapeType::kPolygon: return FOdysseyStyle::GetBrush( "Shapes.Polygon_Filled" );
        case EOdysseyShapeType::kEllipse: return FOdysseyStyle::GetBrush( "Shapes.Ellipse_Filled" );
        case EOdysseyShapeType::kBezier: return FOdysseyStyle::GetBrush( "Shapes.Bezier_3pts_Filled" );
    }

    return nullptr;
}

EOdysseyShapeType
FOdysseyShapesDetailCustomization::GetSelectedShape() const
{
    return mShapes->GetActiveShapeType();
}

void
FOdysseyShapesDetailCustomization::OnShapeSelected(EOdysseyShapeType iShape, ECheckBoxState iState)
{
    if (iState != ECheckBoxState::Checked)
        return;

    TSharedPtr<IPropertyHandle> shapesPropertyHandle = mShapesPropertyHandle.Pin();
    if (!shapesPropertyHandle)
        return;

    TSharedPtr<IPropertyHandle> activeShapeTypeHandle = shapesPropertyHandle->GetChildHandle( GET_MEMBER_NAME_CHECKED(FOdysseyShapes, ActiveShapeType) );
    if (!activeShapeTypeHandle)
        return;

    FOdysseyShapes* shapes = GetEditStruct( shapesPropertyHandle.ToSharedRef() );
    if (shapes)
    {
        UOdysseyShape* activeshape = shapes->GetActiveShape();
        if (activeshape)
            activeshape->Abort();
    }

    activeShapeTypeHandle->SetValue((uint8)iShape, EPropertyValueSetFlags::NotTransactable);
}

void
FOdysseyShapes::RegisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.RegisterCustomPropertyTypeLayout(
        FOdysseyShapes::StaticStruct()->GetFName(),
        FOnGetPropertyTypeCustomizationInstance::CreateLambda(
            []()
            {
                return MakeShareable( new FOdysseyShapesDetailCustomization() );
            }
        )
    );
}

void
FOdysseyShapes::UnregisterDetailCustomization()
{
    FPropertyEditorModule& PropertyModule = FModuleManager::LoadModuleChecked<FPropertyEditorModule>("PropertyEditor");
    PropertyModule.UnregisterCustomPropertyTypeLayout( FOdysseyShapes::StaticStruct()->GetFName() );
}

FOdysseyShapes::FOdysseyShapes()
    : ActiveShapeType(EOdysseyShapeType::kNone)
{
}

UOdysseyShape*
FOdysseyShapes::GetActiveShape() const
{
    if (!Shapes.Contains(ActiveShapeType))
        return nullptr;

    return Shapes[ActiveShapeType];
}

const TMap<EOdysseyShapeType, UOdysseyShape*>&
FOdysseyShapes::GetShapes() const
{
    return Shapes;
}

EOdysseyShapeType
FOdysseyShapes::GetActiveShapeType() const
{
    return ActiveShapeType;
}

void
FOdysseyShapes::SetActiveShapeType(EOdysseyShapeType iType)
{
    UOdysseyShape* activeShape = GetActiveShape();
    if (activeShape)
        activeShape->Abort();

    if (!Shapes.Contains(iType))
    {
        ActiveShapeType = EOdysseyShapeType::kNone;
        return;
    }

    ActiveShapeType = iType;
}

TArray<EOdysseyShapeType>
FOdysseyShapes::GetActiveShapeTypes() const
{
    TArray<EOdysseyShapeType> types;
    Shapes.GenerateKeyArray( types );
    return types;
}

void
FOdysseyShapes::AddShapeType(EOdysseyShapeType iType, UOdysseyShape* iShape)
{
    Shapes.Add(iType, iShape);
}

void
FOdysseyShapes::RemoveShapeType(EOdysseyShapeType iType)
{
    Shapes.Remove(iType);
}

UOdysseyShape::~UOdysseyShape()
{
}

bool
UOdysseyShape::OnMouseDown(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

bool
UOdysseyShape::OnMouseUp(const FOdysseyPoint& iPointInTexture, const FKey& iKey)
{
    return false;
}

void
UOdysseyShape::OnMouseHover(const FOdysseyPoint& iPointInTexture)
{

}

void
UOdysseyShape::OnMouseDrag(const FOdysseyPoint& iPointInTexture)
{

}

bool
UOdysseyShape::OnKeyDown(const FKey& iKey)
{
    return false;
}

bool
UOdysseyShape::OnKeyUp(const FKey& iKey)
{
    return false;
}

void
UOdysseyShape::Tick(float iDeltaTime)
{

}

void
UOdysseyShape::ApplyOverrides(const TMap< TObjectPtr<UClass>, TObjectPtr<UObject>>& iOverrides)
{

}

void
UOdysseyShape::SetHUD(TSharedPtr<FOdysseyHUDElement> iHUD)
{
    mHUD = iHUD;
}

bool
UOdysseyShape::IsProgressive() const
{
    return mIsProgressive;
}

void
UOdysseyShape::Abort()
{
}

TArray<FOdysseyPoint>
UOdysseyShape::GeneratePointsFromFunction(TFunction<FVector2D(float)> iFunction) const
{
    float first = 0.f;
    float last = 1.0f;

    FVector2D firstPos = iFunction(first);
    FVector2D lastPos = iFunction(last);

    TArray<FOdysseyPoint> points = { FOdysseyPoint(firstPos.X, firstPos.Y) };

    struct FCandidate
    {
        float mMin;
        float mMid;
        float mMax;
        bool mCheckMin = true;
    };

    TArray<FCandidate> candidates;
    candidates.Add(
        {
            first,
            first + (last - first) * 0.5f,
            last
        }
    );

    while(!candidates.IsEmpty())
    {
        FCandidate& candidate = candidates.Last();
        FVector2D midPos = iFunction(candidate.mMid);

        if ( candidate.mCheckMin )
        {
            candidate.mCheckMin = false;

            FVector2D minPos = iFunction(candidate.mMin);
            float dist = FVector2D::DistSquared(minPos, midPos);
            if ( dist >= 1 )
            {
                candidates.Add(
                    {
                        candidate.mMin,
                        candidate.mMin + (candidate.mMid - candidate.mMin) * 0.5f,
                        candidate.mMid
                    }
                );
                continue;
            };
        }

        points.Add(FOdysseyPoint(midPos.X, midPos.Y));
        FVector2D maxPos = iFunction(candidate.mMax);
        float dist = FVector2D::DistSquared(midPos, maxPos);
        candidates.Pop(EAllowShrinking::No); //EAllowShrinking::No does not allow shrinking the array, so we keep memory available

        if (dist >= 1)
        {
            candidates.Add(
                {
                    candidate.mMid,
                    candidate.mMid + (candidate.mMax - candidate.mMid) * 0.5f,
                    candidate.mMax
                }
            );
        }
    }

    points.Add(FOdysseyPoint(lastPos.X, lastPos.Y));

    return points;
}
