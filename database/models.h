#pragma once

#include <sqlite_orm/sqlite_orm.h>

namespace Database {
    enum class Cabin { F, C, Y };

    // 航班
    struct Flight {
        std::string carrier;
        int flightNo;
        std::string departureDatetime;
        std::string arrivalDatetime;
        std::string departure;
        std::string arrival;
    };

    // 运价
    struct Price {
        std::string carrier;
        std::string departure;
        std::string arrival;
        Cabin cabin;
        int amount;
    };

    // 运价规则
    struct PriceRule {
        int sequenceNo;
        std::string carrier;
        std::optional<std::string> departure;
        std::optional<std::string> arrival;
        std::optional<std::string> nextCarrier;
        std::optional<std::string> agencies;
        int subcharge;
    };

    // 余座
    struct Seat {
        std::string carrier;
        int flightNo;
        std::string departure;
        std::string arrival;
        std::string departureDatetime;
        std::string seatF;
        std::string seatC;
        std::string seatY;
    };
} // namespace Database

// bind enum cabin
inline char CabinToChar(Database::Cabin cabin) {
    switch (cabin) {
    case Database::Cabin::F:
        return 'F';
    case Database::Cabin::C:
        return 'C';
    case Database::Cabin::Y:
        return 'Y';
    }
    throw std::domain_error("Invalid cabin");
}

inline std::unique_ptr<Database::Cabin> CabinFromString(const std::string &cabin) {
    if (cabin == "F") {
        return std::make_unique<Database::Cabin>(Database::Cabin::F);
    } else if (cabin == "C") {
        return std::make_unique<Database::Cabin>(Database::Cabin::C);
    } else if (cabin == "Y") {
        return std::make_unique<Database::Cabin>(Database::Cabin::Y);
    }
    return nullptr;
}

namespace sqlite_orm {
    template <> struct type_printer<Database::Cabin> : public text_printer {};

    template <> struct statement_binder<Database::Cabin> {
        int bind(sqlite3_stmt *stmt, int index, const Database::Cabin &value) {
            return statement_binder<char>().bind(stmt, index, CabinToChar(value));
        }
    };

    template <> struct field_printer<Database::Cabin> {
        char operator()(const Database::Cabin &t) const { return CabinToChar(t); }
    };

    template <> struct row_extractor<Database::Cabin> {
        Database::Cabin extract(const char *row_value) {
            if (auto Cabin = CabinFromString(row_value)) {
                return *Cabin;
            } else {
                throw std::runtime_error("incorrect Cabin type (" + std::string(row_value) + ")");
            }
        }

        Database::Cabin extract(sqlite3_stmt *stmt, int columnIndex) {
            auto str = sqlite3_column_text(stmt, columnIndex);
            return this->extract((const char *)str);
        }
    };
} // namespace sqlite_orm