## How to use ABI/Code generator attributes
Unlike the old ABI generator tool, the new tool uses C++11 or GNU style attributes to mark ```actions``` and ```tables```.

### [[eosio::action]]
This attribute marks either a struct or a method as an action.
Example (four ways to declare an action for ABI generation):
```c++
// this is the C++11 and greater style attribute
[[eosio::action]]
void testa( name n ) {
   // do something
}

// this is the GNU style attribute, this can be used in C code and prior to C++ 11
__attribute__((eosio_action))
void testa( name n ){
   // do something
}

struct [[eosio::action]] testa {
   name n;
   EOSLIB_SERIALIZE( testa, (n) )
};

struct __attribute__((eosio_action)) testa {
   name n;
   EOSLIB_SERIALIZE( testa, (n) )
};
```
If your action name is not a valid [EOSIO name](https://developers.eos.io/eosio-cpp/docs/naming-conventions) you can explicitly specify the name in the attribute ```c++ [[eosio::action("<valid action name>")]]```

### [[eosio::table]]
Example (two ways to declare a table for ABI generation):
```
struct [[eosio::table]] testtable {
   uint64_t owner;
   /* all other fields */
};

struct __attribute__((eosio_table)) testtable {
   uint64_t owner;
   /* all other fields */
};

typedef eosio::multi_index<"tablename"_n, testtable> testtable_t;
```
If you don't want to use the multi-index you can explicitly specify the name in the attribute ```c++ [[eosio::table("<valid action name>")]]```.

### [[eosio::contract("\<any name you like\>")]]
```
class [[eosio::contract("<any name you would like>")]] test_contract : public eosio::contract {
};
```
This will mark this `class` as being an `EOSIO` contract, this allows for namespacing of contracts, i.e. you can include headers like `eosio::token` and not have `eosio::token`'s actions/tables wind up in you ABI or generated dispatcher.

### [[eosio::on_notify("\<valid eosio account name\>::\<valid eosio action name\>")]]
```
[[eosio::on_notify("eosio.token::transfer")]]
void on_token_transfer(name from, name to, assert quantity, std::string memo) {
   do something on transfer from eosio.token...
}

[[eosio::on_notify("*::transfer")]]
void on_any_transfer(name from, name to, assert quantity, std::string memo) {
   do something on transfer from any account...
}
```

### [[eosio::wasm_entry]]
```
[[eosio::wasm_entry]]
void some_function(...) {
   do something...
}
```

This will mark an arbitrary function as an entry point, which will then wrap the function with global constructors (ctors) and global destructors (dtors).  This will allow for the eosio.cdt toolchain to produce WASM binaries for other ecosystems.

### [[eosio::wasm_import]]
```
extern "C" {
   __attribute__((eosio_wasm_import))
   void some_intrinsic(...);
}
```

This will mark a function declaration as being a WebAssembly import.  This allows for other compilation modes to specify which functions are import only (i.e. do not link) without having to maintain a secondary file with duplicate declarations.
