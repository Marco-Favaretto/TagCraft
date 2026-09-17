#ifndef UNDOACTION_H
#define UNDOACTION_H

#include <QString>
#include <QSet>

struct UndoAction {
    QString description;
    QSet<QString> affectedPaths;
    std::function<void()> undo;
};

#endif // UNDOACTION_H