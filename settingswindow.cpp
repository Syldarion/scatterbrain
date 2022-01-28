#include "settingswindow.h"
#include "ui_settingswindow.h"

SettingsWindow::SettingsWindow(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::SettingsWindow)
{
    ui->setupUi(this);

    ui->projectLocationInput->setText(UserSettings::getInstance()->getProjectDirectory());
    ui->requiredTasksCheckBox->setChecked(UserSettings::getInstance()->getTaskRequirement());
    ui->requiredTasksSpinBox->setValue(UserSettings::getInstance()->getRequiredTasks());
    ui->requiredTasksSpinBox->setEnabled(UserSettings::getInstance()->getTaskRequirement());

    connect(ui->projectLocationButton, &QPushButton::clicked, this, &SettingsWindow::selectProjectDirectory);
    connect(ui->requiredTasksCheckBox, &QCheckBox::toggled, this, &SettingsWindow::toggleRequiredTasks);
    connect(ui->requiredTasksSpinBox, &QSpinBox::valueChanged, this, &SettingsWindow::setRequiredTasks);
    connect(ui->applySettingsButton, &QPushButton::clicked, this, &SettingsWindow::applySettings);
    connect(ui->cancelSettingsButton, &QPushButton::clicked, this, &SettingsWindow::cancelSettings);
}

SettingsWindow::~SettingsWindow()
{
    delete ui;
}

void SettingsWindow::selectProjectDirectory()
{

}

void SettingsWindow::toggleRequiredTasks(bool checked)
{
    UserSettings::getInstance()->modifyTaskRequirement(checked);
    ui->requiredTasksSpinBox->setEnabled(checked);
}

void SettingsWindow::setRequiredTasks(int count)
{
    UserSettings::getInstance()->modifyRequiredTasks(count);
}

void SettingsWindow::applySettings()
{
    UserSettings::getInstance()->applyModifications();
    UserSettings::getInstance()->saveSettings();
    this->close();
}

void SettingsWindow::cancelSettings()
{
    UserSettings::getInstance()->discardModifications();
    this->close();
}
