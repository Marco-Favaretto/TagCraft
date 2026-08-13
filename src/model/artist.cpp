#include "artist.h"

Artist::Artist() 
    : m_id(-1) 
{}

Artist::Artist(int id, const QString &name) 
    : m_id(id), m_name(name) 
{}

int Artist::id() const {
    return m_id;
}

QString Artist::name() const {
    return m_name;
}

void Artist::setId(int id) {
    m_id = id;
}

void Artist::setName(const QString& name) {
    m_name = name;
}