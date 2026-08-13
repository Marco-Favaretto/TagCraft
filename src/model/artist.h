#ifndef ARTIST_H
#define ARTIST_H

#include <QString>

class Artist {
public:
    Artist();
    Artist(int id, const QString &name);

    // Costruttore e assegnazione di copia, default perché non ho puntatori
    Artist(const Artist& other) = default;
    Artist& operator=(const Artist &other) = default;

    // Distruttore, default perché non ho puntatori
    ~Artist() = default;

    int id() const;
    QString name() const;

    void setId(int id);
    void setName(const QString& name);

private:
    int m_id;
    QString m_name;
};

#endif // ARTIST_H