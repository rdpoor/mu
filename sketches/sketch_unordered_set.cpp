/*
 * Test using unordered_map to cache instances of a Symbol.
 */

#include <string>
#include <unordered_map>
#include <iostream>


class Symbol {

public:
  // Use an unordered_map to map a name to a symbol.
  typedef std::unordered_map<std::string, Symbol *> SymbolTable;

  // Return the number of Symbol objects in the symbol table.
  static int symbolTableSize() {
    return symbol_table_.size();
  }

  // Look for a symbol with the given name in the symbol table.  If it
  // does not exist, create one and insert it in the table.  Otherwise
  // return the symbol already in the table.
  static Symbol *fetch(std::string name) {
    SymbolTable::iterator iterator;
    
    if ((iterator = symbol_table_.find(name)) == symbol_table_.end()) {
      Symbol *symbol = new Symbol();
      symbol->name_ = name;
      symbol_table_[name] = symbol;
      return symbol;
    } else {
      return symbol_table_[name];
    }
    
  }

protected:
  static SymbolTable symbol_table_;
  std::string name_;
};

Symbol::SymbolTable Symbol::symbol_table_;

int main() {
  std::cout << "symbol table count = " << Symbol::symbolTableSize() << '\n';
  Symbol::fetch("woof");
  std::cout << "symbol table count = " << Symbol::symbolTableSize() << '\n';
  Symbol::fetch("woof");
  std::cout << "symbol table count = " << Symbol::symbolTableSize() << '\n';
  Symbol::fetch("meow");
  std::cout << "symbol table count = " << Symbol::symbolTableSize() << '\n';
}
