#include <functional>
#include <list>
#include <vector>
#include <exception>

template<class KeyType, class ValueType, class Hash = std::hash<KeyType>>
class HashMap {
public:
  typedef typename std::pair<const KeyType, ValueType> pair_kv;
  typedef typename std::list<pair_kv>::iterator iterator;
  typedef typename std::list<pair_kv>::const_iterator const_iterator;
private:
  static const size_t NORM = 16;
  static const size_t MAX_DIFF = 4;
  
  size_t data_size;
  std::list<pair_kv> data;
  std::vector<std::vector<iterator>> hash_table;
  Hash hasher;

  void add_in_data(const pair_kv& elem) {
    restructure_map();
    erase_in_data(elem.first);
    data.push_back(elem);
    hash_table[hasher(elem.first) % hash_table.size()].push_back((--data.end()));
    data_size++;
  }

  void erase_in_data(const KeyType& key) {
    restructure_map();
    size_t ind_ht = hasher(key) % hash_table.size();
    for (size_t ind_cell = 0; ind_cell < hash_table[ind_ht].size(); ++ind_cell) {
      if (hash_table[ind_ht][ind_cell]->first == key) {
        data.erase(hash_table[ind_ht][ind_cell]);
        hash_table[ind_ht].erase(hash_table[ind_ht].begin() + ind_cell);
        data_size--;
        break;
      }
    }
  }

  void restructure_map() {
    if (data_size == 0) {
      hash_table.resize(1);
      hash_table[0].clear();
    } else if ((data_size * NORM > hash_table.size() * MAX_DIFF) || (data_size * NORM * MAX_DIFF < hash_table.size())) {
      hash_table.clear();
      hash_table.resize(data_size * NORM);
      for (iterator it = data.begin(); it != data.end(); ++it) {
        hash_table[hasher(it->first) % hash_table.size()].push_back(it);
      }
    }
  }

public:
  HashMap& operator =(const HashMap& _hash) {
    hasher = _hash.hasher;
    data = std::list<pair_kv>(_hash.data);
    data_size = _hash.data_size;
    hash_table.clear();
    hash_table.resize(data_size * NORM);
    for (auto it = data.begin(); it != data.end(); ++it) {
      hash_table[hasher(it->first) % hash_table.size()].push_back(it);
    }
    restructure_map();
    return (*this);
  }

  HashMap(Hash _hasher = Hash()) : data_size(0), hasher(_hasher) {
    restructure_map();
  }

  template<typename seq_iterator>
  HashMap(seq_iterator it_begin, const seq_iterator& it_end, Hash _hasher = Hash()) : data_size(0), hasher(_hasher) {
    restructure_map();
    for (; it_begin != it_end; ++it_begin) {
      add_in_data((*it_begin));
    }
  }

  HashMap(const std::initializer_list<pair_kv>& init_list, Hash _hasher = Hash()) : data_size(0), hasher(_hasher) {
    restructure_map();
    for (auto it_cur = init_list.begin(); it_cur != init_list.end(); ++it_cur) {
      add_in_data((*it_cur));
    }
  }

  size_t size() const {
    return data_size;
  }

  bool empty() const {
    return (data_size == 0);
  }

  Hash hash_function() const {
    return hasher;
  }

  void insert(const pair_kv& elem) {
    if (find(elem.first) == data.end()) {
      add_in_data(elem);
    }
  }

  void erase(const KeyType& key) {
    erase_in_data(key);
  }

  iterator begin() {
    return data.begin();
  }
  
  const_iterator begin() const {
    return data.cbegin();
  }

  iterator end() {
    return data.end();
  }
  
  const_iterator end() const {
    return data.cend();
  }

  iterator find(const KeyType& key) {
    size_t ind = hasher(key) % hash_table.size();
    for (size_t i = 0; i < hash_table[ind].size(); ++i) {
      if (hash_table[ind][i]->first == key) {
        return hash_table[ind][i];
      }
    }
    return data.end();
  }

  const_iterator find(const KeyType& key) const {
    size_t ind = hasher(key) % hash_table.size();
    for (size_t i = 0; i < hash_table[ind].size(); ++i) {
      if (hash_table[ind][i]->first == key) {
        return hash_table[ind][i];
      }
    }
    return data.cend();
  }

  ValueType& operator[] (const KeyType& key) {
    auto iter = find(key);
    if (iter == data.end()) {
      add_in_data(std::make_pair(key, ValueType()));
      return ((--data.end())->second);
    } else {
      return (iter->second);
    }
  }

  const ValueType& at(const KeyType& key) const {
    auto iter = find(key);
    if (iter == data.end()) {
      throw std::out_of_range("out of range");
    } else {
      return (iter->second);
    }
  }

  void clear() {
    data_size = 0;
    data.clear();
    hash_table.clear();
    restructure_map();
  }
};
