#ifndef GENRE_H
#define GENRE_H

#include <QString>

class Genre {
public:
    Genre();
    Genre(int id, const QString& name);

    // Costruttore e assegnazione di copia, default perché non ho puntatori
    Genre(const Genre& other) = default;
    Genre& operator=(const Genre& other) = default;

    // Distruttore, default perché non ho puntatori
    ~Genre() = default;

    int id() const;
    QString name() const;

    void setId(int id);
    void setName(const QString& name);

private:
    int m_id;
    QString m_name;
};

#endif // GENRE_H