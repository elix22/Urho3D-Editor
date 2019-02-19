#include "OptionsDialog.h"
#include "Configuration.h"
#include "Core/Core.h"
#include "Core/GlobalVariable.h"
#include <Urho3D/Math/MathDefs.h>
#include <QCheckBox>
#include <QComboBox>
#include <QDoubleValidator>
#include <QFormLayout>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLineEdit>
#include <QListWidget>
#include <QPushButton>
#include <QScrollArea>
#include <QPlainTextEdit>
#include <QVBoxLayout>

namespace Urho3DEditor
{

class ConfigurationVariableImpl
{
public:
    /// Get widget.
    virtual QWidget* GetWidget() const = 0;
    /// Get value.
    virtual QVariant GetValue() const = 0;
    /// Set value.
    virtual void SetValue(const QVariant& value) const = 0;
};

class VoidVariableImpl : public ConfigurationVariableImpl
{
public:
    /// Get widget.
    virtual QWidget* GetWidget() const override { return nullptr; }
    /// Get value.
    virtual QVariant GetValue() const override { return QVariant(); }
    /// Set value.
    virtual void SetValue(const QVariant& /*value*/) const override {}
};

class BoolVariableImpl : public ConfigurationVariableImpl
{
public:
    /// Construct.
    BoolVariableImpl() : widget_(new QCheckBox()) { }

    /// Get widget.
    virtual QWidget* GetWidget() const override { return widget_.data(); }
    /// Get value.
    virtual QVariant GetValue() const override { return widget_->isChecked(); }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override { widget_->setChecked(value.toBool()); }

private:
    /// Widget.
    QScopedPointer<QCheckBox> widget_;

};

class StringVariableImpl : public ConfigurationVariableImpl
{
public:
    /// Construct.
    StringVariableImpl(QValidator* validator = nullptr)
        : widget_(new QLineEdit())
    {
        if (validator)
        {
            widget_->setValidator(validator);
            validator->setParent(widget_.data());
        }
        else
        {
            widget_->setMinimumWidth(200);
        }
    }

    /// Get widget.
    virtual QWidget* GetWidget() const override { return widget_.data(); }
    /// Get value.
    virtual QVariant GetValue() const override { return widget_->text(); }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override { widget_->setText(value.toString()); }

protected:
    /// Widget.
    QScopedPointer<QLineEdit> widget_;

};

class IntegerVariableImpl : public StringVariableImpl
{
public:
    /// Construct.
    IntegerVariableImpl(bool isSigned)
        : StringVariableImpl(isSigned
            ? new QIntValidator(Urho3D::M_MIN_INT, Urho3D::M_MAX_INT)
            : new QIntValidator(0, Urho3D::M_MAX_INT))
    {}

    /// Get value.
    virtual QVariant GetValue() const override
    {
        return widget_->text().toInt();
    }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override
    {
        widget_->setText(QString::number(value.toInt()));
    }

};

class DoubleVariableImpl : public StringVariableImpl
{
public:
    /// Construct.
    DoubleVariableImpl() : StringVariableImpl(new QDoubleValidator()) {}

    /// Get value.
    virtual QVariant GetValue() const override
    {
        return widget_->text().toDouble();
    }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override
    {
        widget_->setText(QString::number(value.toDouble()));
    }

};

class EnumVariableImpl : public ConfigurationVariableImpl
{
public:
    /// Construct.
    EnumVariableImpl(const QVariant& decoration)
        : widget_(new QComboBox)
    {
        widget_->addItems(decoration.toStringList());
    }

    /// Get widget.
    virtual QWidget* GetWidget() const override { return widget_.data(); }
    /// Get value.
    virtual QVariant GetValue() const override { return widget_->currentIndex(); }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override { widget_->setCurrentIndex(value.toInt()); }

private:
    /// Widget.
    QScopedPointer<QComboBox> widget_;

};

class StringListVariableImpl : public ConfigurationVariableImpl
{
public:
    /// Construct.
    StringListVariableImpl()
        : widget_(new QPlainTextEdit())
    {
    }

    /// Get widget.
    virtual QWidget* GetWidget() const override { return widget_.data(); }
    /// Get value.
    virtual QVariant GetValue() const override
    {
        QStringList result = widget_->toPlainText().split(QRegExp("[\r\n]"), QString::SkipEmptyParts);
        for (QString& string : result)
            string = string.trimmed();
        return result;
    }
    /// Set value.
    virtual void SetValue(const QVariant& value) const override
    {
        widget_->setPlainText(value.toStringList().join("\n"));
    }

protected:
    /// Widget.
    QScopedPointer<QPlainTextEdit> widget_;

};

ConfigurationVariableImpl* CreateVariable(QVariant::Type type, const QVariant& decoration)
{
    switch (type)
    {
    case QVariant::String:
        return new StringVariableImpl();

    case QVariant::Bool:
        return new BoolVariableImpl();

    case QVariant::Int:
    case QVariant::LongLong:
        if (decoration.type() == QVariant::StringList)
            return new EnumVariableImpl(decoration);
        else
            return new IntegerVariableImpl(true);

    case QVariant::UInt:
    case QVariant::ULongLong:
        if (decoration.type() == QVariant::StringList)
            return new EnumVariableImpl(decoration);
        else
            return new IntegerVariableImpl(false);

    case QVariant::Double:
        return new DoubleVariableImpl();

    case QVariant::StringList:
        return new StringListVariableImpl();

    case QVariant::Invalid:
    case QVariant::Char:
    case QVariant::Map:
    case QVariant::List:
    case QVariant::ByteArray:
    case QVariant::BitArray:
    case QVariant::Date:
    case QVariant::Time:
    case QVariant::DateTime:
    case QVariant::Url:
    case QVariant::Locale:
    case QVariant::Rect:
    case QVariant::RectF:
    case QVariant::Size:
    case QVariant::SizeF:
    case QVariant::Line:
    case QVariant::LineF:
    case QVariant::Point:
    case QVariant::PointF:
    case QVariant::RegExp:
    case QVariant::RegularExpression:
    case QVariant::Hash:
    case QVariant::EasingCurve:
    case QVariant::Uuid:
    case QVariant::ModelIndex:
    case QVariant::PersistentModelIndex:
    case QVariant::Font:
    case QVariant::Pixmap:
    case QVariant::Brush:
    case QVariant::Color:
    case QVariant::Palette:
    case QVariant::Image:
    case QVariant::Polygon:
    case QVariant::Region:
    case QVariant::Bitmap:
    case QVariant::Cursor:
    case QVariant::KeySequence:
    case QVariant::Pen:
    case QVariant::TextLength:
    case QVariant::TextFormat:
    case QVariant::Matrix:
    case QVariant::Transform:
    case QVariant::Matrix4x4:
    case QVariant::Vector2D:
    case QVariant::Vector3D:
    case QVariant::Vector4D:
    case QVariant::Quaternion:
    case QVariant::PolygonF:
    case QVariant::Icon:
    case QVariant::SizePolicy:
    default:
        return new VoidVariableImpl();
    }
}

GlobalVariableFacade::GlobalVariableFacade(GlobalVariable& variable)
    : variable_(variable)
    , impl_(CreateVariable(variable_.GetDefaultValue().type(), variable_.GetDecorationInfo()))
{
    impl_->SetValue(variable_.GetValue());
}

void GlobalVariableFacade::ResetToDefault()
{
    impl_->SetValue(variable_.GetDefaultValue());
}

void GlobalVariableFacade::Save()
{
    variable_.SetValue(impl_->GetValue(), false);
}

const QString GlobalVariableFacade::GetDisplayText() const
{
    return variable_.GetDisplayText().isEmpty() ? variable_.GetName() : variable_.GetDisplayText();
}

QWidget* GlobalVariableFacade::GetWidget()
{
    return impl_->GetWidget();
}

//////////////////////////////////////////////////////////////////////////
OptionsDialog::OptionsDialog(Core& core)
    : QDialog()
    , core_(core)
    , currentSection_(nullptr)
{
    setWindowTitle("Options");
    SetupVariables();
    SetupLayout();
}

void OptionsDialog::Save()
{
    for (auto& section : variables_)
        for (GlobalVariableFacade* variable : section)
            variable->Save();
    core_.SaveGlobalVariables();
}

void OptionsDialog::Reset()
{
    for (auto& section : variables_)
        for (GlobalVariableFacade* variable : section)
            variable->ResetToDefault();
}

void OptionsDialog::ResetSection(const QString& groupName)
{
    if (variables_.contains(groupName))
    {
        for (GlobalVariableFacade* variable : variables_[groupName])
            variable->ResetToDefault();
    }
}

void OptionsDialog::HandleListRowChanged(int row)
{
    for (QWidget* group : sections_)
        group->setVisible(false);

    currentSection_ = (row >= 0 && row < sections_.size()) ? sections_[row] : nullptr;

    if (currentSection_)
        currentSection_->setVisible(true);

    resize(sizeHint());
}

void OptionsDialog::HandleOk()
{
    Save();
    close();
}

void OptionsDialog::HandleApply()
{
    Save();
}

void OptionsDialog::HandleCancel()
{
    close();
}

void OptionsDialog::HandleResetThese()
{
    if (currentSection_)
        ResetSection(currentSection_->objectName());
}

void OptionsDialog::HandleResetAll()
{
    Reset();
}

void OptionsDialog::SetupVariables()
{
    for (GlobalVariable* variable : core_.GetGlobalVariables())
    {
        GlobalVariableFacade* widget = new GlobalVariableFacade(*variable);
        widget->setParent(this);
        variables_[variable->GetSection()].push_back(widget);
    }
}

void OptionsDialog::SetupLayout()
{
    // Add buttons
    QHBoxLayout* buttonsLayout = new QHBoxLayout;
    buttonsLayout->addStretch();

    QPushButton* buttonOk = new QPushButton("OK");
    QPushButton* buttonApply = new QPushButton("Apply");
    QPushButton* buttonCancel = new QPushButton("Cancel");
    QPushButton* buttonResetThese = new QPushButton("Reset These");
    QPushButton* buttonResetAll = new QPushButton("Reset All");

    buttonOk->setFocusPolicy(Qt::TabFocus);
    buttonApply->setFocusPolicy(Qt::TabFocus);
    buttonCancel->setFocusPolicy(Qt::TabFocus);
    buttonResetThese->setFocusPolicy(Qt::TabFocus);
    buttonResetAll->setFocusPolicy(Qt::TabFocus);

    connect(buttonOk, SIGNAL(clicked()), this, SLOT(HandleOk()));
    connect(buttonApply, SIGNAL(clicked()), this, SLOT(HandleApply()));
    connect(buttonCancel, SIGNAL(clicked()), this, SLOT(HandleCancel()));
    connect(buttonResetThese, SIGNAL(clicked()), this, SLOT(HandleResetThese()));
    connect(buttonResetAll, SIGNAL(clicked()), this, SLOT(HandleResetAll()));

    buttonsLayout->addWidget(buttonResetAll);
    buttonsLayout->addWidget(buttonResetThese);
    buttonsLayout->addWidget(buttonCancel);
    buttonsLayout->addWidget(buttonApply);
    buttonsLayout->addWidget(buttonOk);

    // Add list
    QListWidget* groupsList = new QListWidget;
    QStringList groups = variables_.keys();
    groups.sort(Qt::CaseInsensitive);
    groupsList->addItems(groups);
    connect(groupsList, SIGNAL(currentRowChanged(int)), this, SLOT(HandleListRowChanged(int)));

    QHBoxLayout* mainLayout = new QHBoxLayout;
    mainLayout->addWidget(groupsList);

    // Add variables
    for (const QString& group : groups)
    {
        QScrollArea* variablesGroupArea = new QScrollArea;
        variablesGroupArea->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOn);
        variablesGroupArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        variablesGroupArea->setVisible(false);
        variablesGroupArea->setWidgetResizable(true);
        variablesGroupArea->setObjectName(group);
        mainLayout->addWidget(variablesGroupArea);
        mainLayout->setStretchFactor(variablesGroupArea, 1);
        sections_.push_back(variablesGroupArea);

        QWidget* variableGroupWidget = new QWidget;
        QFormLayout* variablesLayout = new QFormLayout;
        variablesLayout->setFieldGrowthPolicy(QFormLayout::AllNonFixedFieldsGrow);
        variableGroupWidget->setLayout(variablesLayout);

        for (GlobalVariableFacade* variable : variables_[group])
            variablesLayout->addRow(variable->GetDisplayText(), variable->GetWidget());

        variablesGroupArea->setWidget(variableGroupWidget);
    }

    // Setup dialog layout
    QVBoxLayout* dialogLayout = new QVBoxLayout;
    dialogLayout->addLayout(mainLayout);
    dialogLayout->addLayout(buttonsLayout);
    setLayout(dialogLayout);

    // Select group
    if (sections_.size() > 0)
    {
        const int currentIndex = qMin(sections_.size() - 1, 1);
        groupsList->setCurrentRow(currentIndex);
        HandleListRowChanged(currentIndex);
    }
}

}
