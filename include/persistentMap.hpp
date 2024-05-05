#pragma once

#include <memory>
#include <optional>
#include <string>
#include <vector>

/**
 * @brief A persistent data structure implementation of a hash map. Used within
 * memory environments to deal with scope issues while reducing memory usage.
 *
 * @tparam KeyType
 * @tparam ValueType
 * @tparam N
 */
template <typename KeyType, typename ValueType, std::size_t N = 1024>
class PersistentMap {
  public:
  using Entry = std::shared_ptr<std::pair<KeyType, ValueType>>;

  using Table = std::array<std::vector<Entry>, N>;

  /**
   * @brief Constructs a new PersistentMap.
   *
   */
  PersistentMap() = default;

  /**
   * @brief Constructs a new PersistentMap based off a table of values.
   *
   * @param iTable
   */
  PersistentMap(const Table &iTable) : table{iTable} {}

  /**
   * @brief Inserts a new key and value pair into the table and returns the new
   * map.
   *
   * @param key
   * @param value
   * @return PersistentMap
   */
  PersistentMap insert(const KeyType &key, const ValueType &value) {
    Table newTable{table};
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    newTable[hashIndex].push_back(
        std::make_shared<std::pair<KeyType, ValueType>>(
            std::make_pair(key, value)));
    return PersistentMap{newTable};
  }

  /**
   * @brief Assigns a key to a value and returns the new map if it was possible.
   *
   * @param key
   * @param value
   * @return std::optional<PersistentMap>
   */
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

  /**
   * @brief Gets the value associated with the provided key if it exists.
   *
   * @param key
   * @return std::optional<ValueType>
   */
  std::optional<ValueType> get(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i]->second;
    }
    return {};
  }

  /**
   * @brief Gets the entry associated with the provided key.
   *
   * @param key
   * @return Entry
   */
  Entry getEntry(const KeyType &key) const {
    const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
    for(std::size_t i{0}; i < table[hashIndex].size(); i++) {
      if(table[hashIndex][i]->first == key) return table[hashIndex][i];
    }
    return nullptr;
  }

  /**
   * @brief Copies the values of another PersistentMap (rather than sharing the
   * entries themselves).
   *
   * @param other
   * @return PersistentMap
   */
  PersistentMap copyOver(const PersistentMap &other) {
    PersistentMap newMap{table};
    for(std::vector<Entry> entries : other.table) {
      for(Entry entry : entries)
        newMap = newMap.insert(entry->first, entry->second);
    }
    return newMap;
  }

  /**
   * @brief Combines several maps into one new PersistentMap.
   *
   * @param maps
   * @return PersistentMap
   */
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