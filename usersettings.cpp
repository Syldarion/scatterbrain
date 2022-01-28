#include "usersettings.h"

#include <QDir>
#include <QStandardPaths>
#include <QJsonObject>
#include <QJsonDocument>

UserSettings* UserSettings::_instance = nullptr;

UserSettings::UserSettings()
{
    taskRequirement = modifiedTaskRequirement = false;
    requiredTasks = modifiedRequiredTasks = 0;

    QString docPath = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    QDir docDir(docPath);

    if (!docDir.cd("Scatterbrain"))
    {
        if (!docDir.mkdir("Scatterbrain"))
        {
            qWarning("Could not create Scatterbrain Documents folder.");
            return;
        }

        docDir.cd("Scatterbrain");
    }

    projectDirectory = modifiedProjectDirectory = docDir.absolutePath();
}

QString UserSettings::getProjectDirectory()
{
    return projectDirectory;
}

bool UserSettings::getTaskRequirement()
{
    return taskRequirement;
}

int UserSettings::getRequiredTasks()
{
    return requiredTasks;
}

void UserSettings::modifyProjectDirectory(QString directory)
{
    modifiedProjectDirectory = directory;
}

void UserSettings::modifyTaskRequirement(bool required)
{
    modifiedTaskRequirement = required;
}

void UserSettings::modifyRequiredTasks(int count)
{
    modifiedRequiredTasks = count;
}

void UserSettings::applyModifications()
{
    projectDirectory = modifiedProjectDirectory;
    taskRequirement = modifiedTaskRequirement;
    requiredTasks = modifiedRequiredTasks;
}

void UserSettings::discardModifications()
{
    modifiedProjectDirectory = projectDirectory;
    modifiedTaskRequirement = taskRequirement;
    modifiedRequiredTasks = requiredTasks;
}

void UserSettings::read(const QJsonObject &json)
{
    if (json.contains("projectDirectory") && json["projectDirectory"].isString())
        projectDirectory = json["projectDirectory"].toString();
    if (json.contains("taskRequirement") && json["taskRequirement"].isBool())
        taskRequirement = json["taskRequirement"].toBool();
    if (json.contains("requiredTasks") && json["requiredTasks"].isDouble())
        requiredTasks = json["requiredTasks"].toInt();
}

void UserSettings::write(QJsonObject &json) const
{
    json["projectDirectory"] = projectDirectory;
    json["taskRequirement"] = taskRequirement;
    json["requiredTasks"] = requiredTasks;
}

void UserSettings::saveSettings()
{
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (settingsPath.isEmpty())
        // throw error?
        return;

    QDir d{settingsPath};

    if (d.mkpath(d.absolutePath()) && QDir::setCurrent(d.absolutePath()))
    {
        QFile f{"userSettings.json"};

        if (!f.open(QIODevice::WriteOnly))
        {
            qWarning("Could not open file.");
            return;
        }

        QJsonObject settingsObject;
        write(settingsObject);

        f.write(QJsonDocument(settingsObject).toJson());
        f.close();
    }
}

void UserSettings::loadSettings()
{
    QString settingsPath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    if (settingsPath.isEmpty())
        // throw error?
        return;

    QDir d{settingsPath};

    if (d.mkpath(d.absolutePath()) && QDir::setCurrent(d.absolutePath()))
    {
        QFile f{"userSettings.json"};

        if (!f.open(QIODevice::ReadOnly))
        {
            qWarning("Could not open file.");
            return;
        }

        QByteArray settingsData = f.readAll();
        f.close();

        QJsonDocument settingsDoc = QJsonDocument::fromJson(settingsData);
        read(settingsDoc.object());
    }
}

UserSettings* UserSettings::getInstance()
{
    if (_instance == nullptr)
        _instance = new UserSettings();
    return _instance;
}
