#include "genre.h"

Genre::Genre() 
    : m_id(-1) 
{}

Genre::Genre(int id, const QString& name) 
    : m_id(id), m_name(name) 
{}

int Genre::id() const {
    return m_id;
}

QString Genre::name() const {
    return m_name;
}

void Genre::setId(int id) {
    m_id = id;
}

void Genre::setName(const QString& name) {
    m_name = name;
}

QString Genre::toString() const {
    return QString("Genre{id=%1, name='%2'}")
        .arg(m_id)
        .arg(m_name);
}