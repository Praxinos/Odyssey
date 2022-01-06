// IDDN.FR.001.220036.000.S.P.2021.000.00000
// EPOS is subject to copyright © laws and is the legal and intellectual property of Praxinos,Inc

#pragma once

#include "CoreMinimal.h"
#include "IPropertyTypeCustomization.h"

//---

class FDetailWidgetRow;
class IDetailChildrenBuilder;
class IPropertyHandle;
class IPropertyTypeCustomizationUtils;
class SEditableTextBox;

struct FNamingConventionPlaneCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetPatternText() const;

    void OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo );
    void OnPatternTextChanged( const FText& iNewText );

    bool CheckPatternValidity( const FString& iPattern );

private:
    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
    /** Custom widget to manage pattern text */
    TSharedPtr<SEditableTextBox> mPatternWidget;
};

//---

struct FNamingConventionCameraCustomization
    : public IPropertyTypeCustomization
{
public:
    static TSharedRef<IPropertyTypeCustomization> MakeInstance();

    /** IPropertyTypeCustomization interface */
    virtual void CustomizeHeader( TSharedRef<IPropertyHandle> iStructPropertyHandle, FDetailWidgetRow& ioHeaderRow, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;
    virtual void CustomizeChildren( TSharedRef<IPropertyHandle> iStructPropertyHandle, IDetailChildrenBuilder& ioChildBuilder, IPropertyTypeCustomizationUtils& ioStructCustomizationUtils ) override;

private:
    FText GetPatternText() const;

    void OnPatternTextCommited( const FText& iNewText, ETextCommit::Type iCommitInfo );
    void OnPatternTextChanged( const FText& iNewText );

    bool CheckPatternValidity( const FString& iPattern );

private:
    /** Property handles of the properties we're editing */
    TSharedPtr<IPropertyHandle> mPatternHandle;
    /** Custom widget to manage pattern text */
    TSharedPtr<SEditableTextBox> mPatternWidget;
};
