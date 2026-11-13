#ifndef ALIASES_H
#define ALIASES_H

#include <chrono>
#include <filesystem>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace s = std;
namespace sc = std::chrono;
namespace sf = std::filesystem;
namespace st = std::this_thread;

using str = s::string;
using strv = s::string_view;

template<typename T, typename A = s::allocator<T>>
using vec = s::vector<T, A>;
template<typename T>
using opt = s::optional<T>;
template<typename... Ts>
using umap = s::unordered_map<Ts...>;
template<typename... Ts>
using uset = s::unordered_set<Ts...>;
template<typename... Ts>
using ummap = s::unordered_multimap<Ts...>;
template<typename... Ts>
using umset = s::unordered_multiset<Ts...>;
template<typename T, typename D = s::default_delete<T>>
using uptr = s::unique_ptr<T, D>;
template<typename T>
using sptr = s::shared_ptr<T>;
template<typename T>
using wptr = s::weak_ptr<T>;

#endif  // ALIASES_H
