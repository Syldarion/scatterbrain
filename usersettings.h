#ifndef USERSETTINGS_H
#define USERSETTINGS_H

#include <QJsonObject>
#include <QString>

class UserSettings
{
public:
    QString getProjectDirectory();
    bool getTaskRequirement();
    int getRequiredTasks();

    void modifyProjectDirectory(QString directory);
    void modifyTaskRequirement(bool required);
    void modifyRequiredTasks(int count);

    void applyModifications();
    void discardModifications();

    void read(const QJsonObject& json);
    void write(QJsonObject& json) const;
    void saveSettings();
    void loadSettings();

    static UserSettings* getInstance();
protected:
    UserSettings();
    static UserSettings* _instance;
private:
    QString projectDirectory;
    bool taskRequirement;
    int requiredTasks;

    QString modifiedProjectDirectory;
    bool modifiedTaskRequirement;
    int modifiedRequiredTasks;
};

#endif // USERSETTINGS_H
