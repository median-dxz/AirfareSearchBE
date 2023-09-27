#include "database.h"

using Database::Storage;

std::unique_ptr<Storage> _p = nullptr;

Storage &Database::getStorage(std::optional<std::string> path) {
    if (_p == nullptr) {
        _p = std::make_unique<Storage>(createStroage(path.value_or("as.db")));
    }
    return (*_p);
}