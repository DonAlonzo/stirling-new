#include "deleter.hpp"

ReferenceCounter::operator size_t() {
    std::unique_lock<std::mutex> lock{mutex};
    return references;
}

size_t ReferenceCounter::operator++() {
    std::unique_lock<std::mutex> lock{mutex};
    ++references;
}

size_t ReferenceCounter::operator--() {
    std::unique_lock<std::mutex> lock{mutex};
    --references;
}