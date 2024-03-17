#include <iostream>
#include <map>
#include <any>
#include <array>
#include <vector>
#include <memory>
#include <unordered_map>

struct Token {
    std::string lexeme;
    enum class Type {
        Variable = 0,
        Constant,
        If, 
        Else,
        For,
        While,
        Or,
        And,
        Boolean,
        Identifier,
        Number,
        String,
        Begin,
        End,
        LeftCurly,
        RightCurly,
        Semicolon,
        LeftParen,
        RightParen,
        EqualTo,
        NotEqualTo,
        LessThan,
        GreaterThan,
        LessThanOrEqualTo,
        GreaterThanOrEqualTo,
        Equal,
        Asterisk,
        ForwardSlash,
        Plus,
        Dash,
        Exclamation,
        Modulus,
        Comma,
        Caret,
        Function,
        Lambda,
        Return,
        Error // Error needs to always be at the bottom of the list!
    } type;
    int line{-1};
    int col{-1};
    bool operator==(const Type rhs) const;
    bool operator==(const Token &rhs) const;
};

template <>
struct std::hash<Token> {
    std::size_t operator()(const Token &token) const noexcept {
        return std::hash<std::string>{}(token.lexeme);
    }
};

using Tokens = std::vector<Token>;

template <typename KeyType, typename ValueType, std::size_t N = 1024>
class PersistentMap {
public:
    using Entry = std::shared_ptr<std::pair<KeyType, ValueType>>;
    using Table = std::array<std::vector<Entry>, N>;
    
    PersistentMap() = default;
    
    PersistentMap(const Table &iTable) : table{iTable} {}

    PersistentMap define(const KeyType &key, const ValueType &value) {
        Table newTable{table};
        const std::size_t hashIndex{std::hash<KeyType>{}(key) % N};
        newTable[hashIndex].push_back(std::make_shared<std::pair<KeyType, ValueType>>(std::make_pair(variable, value)));
        return PersistentMap{newTable};
    }

    PersistentMap assign(const KeyType &key, const ValueType &value) {
        if(auto search = values.find(variable); search != values.end()) {
            values.insert_or_assign(variable, value);
            return;
        }
    }

    std::any get(const Token &variable) {
        if(auto search = values.find(variable); search != values.end())
        return search->second;
        if(outer) return outer->get(variable);
    }

    private:
    Table table;
};

int main() {
    using EnvEntry = std::shared_ptr<std::pair<Token, std::any>>;
    EnvEntry pair1{std::make_shared<std::pair<Token, std::any>>(std::make_pair(Token{"test1"}, 1))};
    EnvEntry pair2{std::make_shared<std::pair<Token, std::any>>(std::make_pair(Token{"test2"}, 2))};
    EnvEntry pair3{std::make_shared<std::pair<Token, std::any>>(std::make_pair(Token{"test3"}, 3))};
    std::vector<EnvEntry> pairs1{pair1, pair2};
    std::vector<EnvEntry> pairs2{pair1, pair2, pair3};
    for(EnvEntry entry : pairs1)
        std::cout << entry << '\n';
    for(EnvEntry entry : pairs2)
        std::cout << entry << '\n';
    return 0;
}