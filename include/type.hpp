#pragma once

#ifndef __TYPE_HPP__
#define __TYPE_HPP__

#include <cstdint>
#include <unordered_map>
#include <unordered_set>

namespace capmds {

typedef uint64_t u64;
typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

typedef double fp64;

template<typename K, typename V>
using map = std::unordered_map<K, V>;

template<typename Val>
using set = std::unordered_set<Val>;

}; // namespace capmds

#endif // __TYPE_HPP__
