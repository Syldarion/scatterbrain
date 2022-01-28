#ifndef SETTINGSWINDOW_H
#define SETTINGSWINDOW_H

#include "usersettings.h"

#include <QWidget>

namespace Ui {
class SettingsWindow;
}

class SettingsWindow : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWindow(QWidget *parent = nullptr);
    ~SettingsWindow();
private slots:
    void selectProjectDirectory();
    void toggleRequiredTasks(bool checked);
    void setRequiredTasks(int count);
    void applySettings();
    void cancelSettings();
private:
    Ui::SettingsWindow *ui;
};

#endif // SETTINGSWINDOW_H
