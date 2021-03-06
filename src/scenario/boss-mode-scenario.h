#ifndef BOSSCHALLENGE_H
#define BOSSCHALLENGE_H

#include "scenario.h"

class ImpasseScenario : public Scenario{
    Q_OBJECT

public:
    explicit ImpasseScenario();

    virtual bool exposeRoles() const;
    virtual void assign(QStringList &generals, QStringList &roles) const;
    virtual int getPlayerCount() const;
    virtual QString getRoles() const;
    virtual void onTagSet(Room *room, const QString &key) const;
    virtual bool generalSelection() const;
};

#endif // BOSSCHALLENGE_H
