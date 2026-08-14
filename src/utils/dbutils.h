#ifndef DBUTILS_H
#define DBUTILS_H

#include <optional>
#include <QVariant>
#include <QMetaType>

class DbUtils {
public:
    DbUtils() = delete; // Classe non istanziabile, metodi statici

    // std::optional<T> -> QVariant.
    // Se l'optional non ha valore, restituisce un QVariant null per il db.
    template <typename T>
    static QVariant optionalToVariant(const std::optional<T>& opt) {
        if (opt.has_value()) {
            return QVariant::fromValue(opt.value());
        }
        return QVariant(QMetaType::fromType<T>());
    }

    // QVariant letto da QSqlQuery -> std::optional<T> 
    // Se il valore nel DB è NULL o invalido, restituisce std::nullopt.
    template <typename T>
    static std::optional<T> variantToOptional(const QVariant& variant) {
        if (variant.isNull() || !variant.isValid()) {
            return std::nullopt;
        }
        return variant.value<T>();
    }
};

#endif // DBUTILS_H