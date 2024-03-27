#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

template <typename KeyType, typename ValueType, std::size_t N = 1024>
class PersistentMap {
  public:
  using Entry = std::shared_ptr<std::pair<KeyType, ValueType>>;

  using Table = std::array<std::vector<Entry>, N>;

  PersistentMap() = default;

  PersistentMap(const Table &iTable) : table{iTable} {}

  PersistentMap insert(const KeyType &key, const ValueType &value) {
    Table newTable{table};
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    newTable[hashIndex].push_back(
        std::make_shared<std::pair<KeyType, ValueType>>(
            std::make_pair(key, value)));
    return PersistentMap{newTable};
  }

  std::optional<PersistentMap> assign(const KeyType &key,
                                      const ValueType &value) {
    Table newTable{table};
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < newTable[hashIndex].size(); i++) {
      if(newTable[hashIndex][i]->first == key) {
        newTable[hashIndex][i]->second = value;
        return PersistentMap{newTable};
      }
    }
    return {};
  }

  std::optional<ValueType> get(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i]->second;
    }
    return {};
  }

  Entry getEntry(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i];
    }
    return nullptr;
  }

  PersistentMap copyOver(const PersistentMap &other) {
    PersistentMap newMap{table};
    for(std::vector<Entry> entries : other.table) {
      for(Entry entry : entries)
        newMap = newMap.insert(entry->first, entry->second);
    }
    return newMap;
  }

  static PersistentMap unionize(const std::vector<PersistentMap> &maps) {
    Table newTable{};
    for(std::size_t i{0}; i < newTable.size(); i++) {
      for(PersistentMap map : maps) {
        for(Entry entry : map.table[i]) newTable[i].push_back(entry);
      }
    }
    return newTable;
  }

  private:
  Table table;
};