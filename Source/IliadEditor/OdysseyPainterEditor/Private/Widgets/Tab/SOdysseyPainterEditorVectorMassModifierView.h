// IDDN.FR.001.060015.013.S.X.2019.000.00000
// ODYSSEY is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc - Year of publishing 2019

#pragma once

#include "CoreMinimal.h"
#include "IDetailPropertyExtensionHandler.h"
#include "IDetailChildrenBuilder.h"
#include "IDetailCustomization.h"
#include "DetailWidgetRow.h"
#include "Misc/WildcardString.h"

#include "SOdysseyPainterEditorVectorMassModifierView.generated.h"

class UOdysseyPainterEditorVectorObjectView;
class UOdysseyPainterEditorVectorPathView;
class UOdysseyPainterEditorVectorGroupView;
class UOdysseyPainterEditorVectorGroupPaintView;
class UOdysseyPainterEditorVectorTagInbetweenerView;
class FOdysseyVectorGroupPaint;
class FOdysseyPainterEditor;
class FOdysseyVectorLayer;
class UOdysseyLayerStack;
struct FPropertyChangedEvent;
class SOdysseyPainterEditorVectorMassModifierView;

UENUM()
enum class EMassModifierApplyTo : uint8
{
    AllTypes    = 0 UMETA( ToolTip = "All Types" ),
    Paths       = 1 UMETA( ToolTip = "Paths" ),
    Groups      = 2 UMETA( ToolTip = "Groups" ),
    PaintGroups = 3 UMETA( ToolTip = "Paint Groups" ),
};

UCLASS()
class UOdysseyPainterEditorVectorMassModifierOptionsView : public UObject
{
    public:
        GENERATED_BODY()


    public:
        ~UOdysseyPainterEditorVectorMassModifierOptionsView(){};
        UOdysseyPainterEditorVectorMassModifierOptionsView()
        : ApplyTo ( EMassModifierApplyTo::AllTypes )
        , Filter( "" )
        , MatchCase( true )
        {
        };

        virtual void PostEditChangeProperty( FPropertyChangedEvent& PropertyChangedEvent ) override;

        void SetMassModifierView( SOdysseyPainterEditorVectorMassModifierView* iModifierView )
        {
            mModifierView = iModifierView;
        }

    protected:
        SOdysseyPainterEditorVectorMassModifierView* mModifierView;

    public:
        UPROPERTY( EditAnywhere
                 , Category = Options
                 , meta = ( ToolTip = "Apply To" ) )
        EMassModifierApplyTo ApplyTo;

        UPROPERTY( EditAnywhere
                 , Category = Options
                 , meta = ( ToolTip = "Filter" ) )
        FString Filter;

        UPROPERTY( EditAnywhere
                 , Category = Options
                 , meta = ( ToolTip = "Case-Sensitivity" ) )
        bool MatchCase;
};


/**
 * Implements the Scene Tree View Widget
 */
class ODYSSEYPAINTEREDITOR_API SOdysseyPainterEditorVectorMassModifierView
    : public SCompoundWidget
    , public FGCObject
    , public IDetailPropertyExtensionHandler
{
    SLATE_DECLARE_WIDGET(SOdysseyPainterEditorVectorMassModifierView, SCompoundWidget)

    // we create a nested class to handle IDetailCustomization interface otherwise there are some multiple inheritance issues
    // thrown by the compiler. We only need it to collapse the categories.
    class DetailCustomizationHandler : public IDetailCustomization
    {
        public:
            ~DetailCustomizationHandler(){};
            DetailCustomizationHandler( TObjectPtr<UOdysseyPainterEditorVectorMassModifierOptionsView> iMassModifierOptionsView )
            : mMassModifierOptionsView ( iMassModifierOptionsView )
            {
            };

        protected:
            // implements IDetailCustomization::CustomizeDetails
           virtual void CustomizeDetails( IDetailLayoutBuilder& DetailBuilder ) override;

        protected:
            TObjectPtr<UOdysseyPainterEditorVectorMassModifierOptionsView> mMassModifierOptionsView;
    };

    public:
        SLATE_BEGIN_ARGS(SOdysseyPainterEditorVectorMassModifierView)
            {}
            SLATE_ARGUMENT(TSharedPtr<FOdysseyVectorLayer>, VectorLayer)
            SLATE_ARGUMENT(TArray<FOdysseyVectorGroupPaint*>, SceneArray)
        SLATE_END_ARGS()

    public:
        // Construction / Destruction
        ~SOdysseyPainterEditorVectorMassModifierView();
        SOdysseyPainterEditorVectorMassModifierView();

        void Construct( const FArguments& InArgs );
        void ValidateProperties();

    // implements IDetailPropertyExtensionHandler
    public:
        virtual void ExtendWidgetRow ( FDetailWidgetRow& InWidgetRow
                                     , const IDetailLayoutBuilder& InDetailBuilder
                                     , const UClass* InObjectClass
                                     , TSharedPtr< IPropertyHandle > PropertyHandle ) override;
        virtual bool IsPropertyExtendable( const UClass* InObjectClass
                                         , const IPropertyHandle& PropertyHandle) const override;
        void ObjectTypeSelectionChanged( EMassModifierApplyTo NewValue );
        bool HasAnyPropertyBit();
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> GetCurrentObjectView();

    protected:
        virtual void AddReferencedObjects(FReferenceCollector& Collector) override;
        virtual FString GetReferencerName() const override;
        void PropertyValueChanged( const FPropertyChangedEvent& iEvent );

        TSharedRef<SWidget> MakeWidgetForOption( TSharedPtr<FString> InOption );
        void PropertyCheckStateChanged( ECheckBoxState iState
                                      , TSharedPtr<IPropertyHandle> iPropertyHandle
                                      , TSharedRef<SWidget> NameWidget
                                      , TSharedRef<SWidget> ValueWidget );
        ECheckBoxState GetPropertyCheckState( TSharedPtr<IPropertyHandle> iPropertyHandle ) const;

    protected:
        TSharedPtr<IDetailsView> CreateViewPanel();
        TSharedRef<IDetailCustomization> GetCustomizationInstance();


    protected:
        TArray<FOdysseyVectorGroupPaint*> mSceneArray;
        TSharedPtr<FOdysseyVectorLayer> mVectorLayer;

        TSharedPtr<IDetailsView> mOptionsDetailsView;
        TObjectPtr<UOdysseyPainterEditorVectorMassModifierOptionsView> mOptionsView;

        TSharedPtr<IDetailsView> mObjectDetailsView;
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mCurrentObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorObjectView> mObjectView;
        TObjectPtr<UOdysseyPainterEditorVectorPathView> mPathView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupView> mGroupView;
        TObjectPtr<UOdysseyPainterEditorVectorGroupPaintView> mGroupPaintView;
};
