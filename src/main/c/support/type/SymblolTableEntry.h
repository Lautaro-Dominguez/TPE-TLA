#ifndef SYMBOL_TABLE_ENTRY_HEADER
#define SYMBOL_TABLE_ENTRY_HEADER

typedef struct SymbolTableEntry SymbolTableEntry;

/**
 * An entry in the symbol table. You can change this structure as you see fit.
 */
struct SymbolTableEntry {
    char * identifier;
    void * value;
    char * type;
    SymbolTableEntry * next;
};

#endif