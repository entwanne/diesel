#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <iomanip>
#include <fstream>
#include <iostream>
#include <ctype.h>
#include <string.h>
#include "symtab.hh" 

using namespace std;

/*** Global variables ***/

symbol_table *sym_tab = new symbol_table(); // The symbol table itself. 
// The symbeltable is a table of pointers to symbol (which can be of various types)
sym_index     void_type;
sym_index     integer_type;
sym_index     real_type;



 /*** The symbol_table class - watch out, it's big. ***/

 /* Constructor: allocates the data members. The symbol table itself is just
    a table of pointers to symbols. This is due to the various subclasses of
    symbols used. */
symbol_table::symbol_table() {
    int i; // counter, later used when initialising hash_table, block_table,
           // sym_table. See below.

    // Used to link parameters to preinstalled functions. See below.
    sym_index tmp, tmp2; 
    symbol *sym, *sym2;
    parameter_symbol *par;
    procedure_symbol *proc;
    function_symbol *func;
    
    // This is just a dummy position for the preinstalled functions.
    position_information *dummy_pos = new position_information(); 

    // --- Initialize string pool. ---   
    /* The string pool (String table) will 
       contain the output from scanner, the
       prevoius lab (for instance "7GLOBAL4VOID4REAL..")
       Take a look at figure 2 in the laboratory
       assignement.*/
					 
    pool_pos = 0; // Always points to the last position in the string pool
    pool_length = BASE_POOL_SIZE; // set base size of string pool. Can be 
    // extended later on, if there are many symbols to scan.
    
    // create a string with length of pool_length
    string_pool = new char[pool_length]; 
    string_pool[0] = '\0'; // insert the null char in the end

    // --- Initialize hash table. ---
    hash_table = new sym_index[MAX_HASH];    // Allocate space.
    for(i = 0; i < MAX_HASH; i++) {          // Zero the table.
	hash_table[i] = NULL_SYM;
    }

    // --- Initialize display. ---
    // The block_table will keep track of the current lexical level
    // global level is 0
    current_level = 0;                           // Zero block level.
    block_table = new sym_index[MAX_BLOCK];      // Allocate space.
    for(i = 0; i < MAX_BLOCK; i++) {             // Zero the table.
	block_table[i] = 0;
    }

    // --- Initialize symbol table. ---
    sym_table = new symbol*[MAX_SYM];            // Weird syntax, gives us a
                                                 //   table of pointers to
                                                 //   symbols.
    for(i = 0; i < MAX_SYM; i++)                 // Zero the table.
	sym_table[i] = NULL;

    label_nr = -1;                               // Zero the assembler label
                                                 // counter.
    temp_nr = 0;                                 // Zero temp var counter.
    // sym_pos will point to the last entry in symbol table
    sym_pos = -1;                                // Zero the current symbol
                                                 // position.

    // --- Install predefined symbols. ---
    // If the scanner works the TEST_SCANNER must be set to 0.
    // Otherwise the creation of symbol table will end here.
    // Make sure you have set TEST_SCANNER to 0 while
    // working with lab2-8.
    if(TEST_SCANNER) 
	return;
    
    // This "empty" symbol represents the global level.
    enter_procedure(dummy_pos, pool_install(capitalize("global.")));
    sym_table[0]->type = void_type; // Needed since there have been no types
                                    // installed yet.

    // Install the default nametypes. This is the only place enter_nametype()
    // is used, since currently Diesel's grammar doesn't handle used-defined
    // types.

    void_type = enter_nametype(dummy_pos, pool_install(capitalize("void")));
    sym_table[void_type]->type = void_type; // Needed since it's the first one.
    
    integer_type = enter_nametype(dummy_pos, pool_install(capitalize("integer")));
    
    real_type = enter_nametype(dummy_pos, pool_install(capitalize("real")));

    // Add the read() function. It returns an integer and takes no arguments.
    tmp = enter_function(dummy_pos, pool_install(capitalize("read")));
    sym_table[tmp]->type = integer_type;

    // Add the write(int-arg) procedure. It takes an integer argument.
    // We need to set the parameter links by hand here since the global
    // environment doesn't work exactly like a normal scope. To do this
    // we're forced to do some safe downcasting (the get_foo_symbol() calls).
    // We do that to get hold of the correct subclass of symbol.
    tmp = enter_procedure(dummy_pos, pool_install(capitalize("write")));
    tmp2 = enter_parameter(dummy_pos,
			   pool_install(capitalize("int-arg")),
			   integer_type);
    sym = sym_table[tmp];
    sym2 = sym_table[tmp2];
    proc = sym->get_procedure_symbol();
    par = sym2->get_parameter_symbol();
    proc->last_parameter = par;

    // Add the trunc(real-arg) function. It returns an integer and takes
    // a real argument.
    tmp = enter_function(dummy_pos, pool_install(capitalize("trunc")));
    sym_table[tmp]->type = integer_type;
    
    tmp2 = enter_parameter(dummy_pos,
			   pool_install(capitalize("real-arg")),
			   real_type);
    sym = sym_table[tmp];
    sym2 = sym_table[tmp2];
    func = sym->get_function_symbol();
    par = sym2->get_parameter_symbol();

    // Get rid of int-arg, which is linked together with real-arg by
    // enter_parameter. This is very handy everywhere in this compiler except
    // just here. So this workaround is unfortunately needed.
    par->preceding = NULL;
    par->offset = 0;
    func->last_parameter = par; // ...which is now real-arg only.

    proc = sym_table[0]->get_procedure_symbol();
    proc->last_parameter = NULL;  
}



/*** Utility functions ***/

 /* This help function is used by the scanner to turn a float (like 2.15)
    into an integer using IEEE 32-bit format. Doing this avoids lots of
    problems with constants (that can be both integer and real) during quad
    and code generation. */
int symbol_table::ieee(float f) {
    float *fp = &f;

    assert(sizeof(float) == 4);
    assert(sizeof(int) == 4);

    return (*(int *) fp);
} 


/* This function generates assembler label numbers. */
long symbol_table::get_next_label() {
    // Labels start on -1 (which is the global level, meaning that all labels
    // generated for user-defined functions etc start with 0).
    return label_nr++;
}


/* Generate a unique temporary variable name. We do it without any extra fuss:
   $1, $2, $3, $4 ... up to 1 million. Diesel isn't written to handle that
   large programs anyway. The type should never be void_type; if it is, it's
   an error. This method is used for quad generation. */
sym_index symbol_table::gen_temp_var(sym_index type) {
    // Your code here.

    return NULL_SYM;    
}


/* This function returns the byte size of a nametype. */

int symbol_table::get_size(const sym_index type) {
    // We assume an integer is 32-bit.
    if(type == integer_type)
	return 4;

    // The size 4 here comes from the IEEE short float format.
    if(type == real_type) 
	return 4;

    if(type == void_type) {
	debug() << "get_size() request for void_type - probably an error.\n";
	return 0;
    }

    // Should never happen.
    print(1);
    fatal("Internal compiler error: get_size(): type size confusion");
    return -1;
}


/* This function prints the contents of the symbol table.
    Args: 1     - print a one-liner for each symbol with relevant data.
	  2     - only print the string table showing the current pool_pos.
	  3     - dump the nonzero elements in the hash table.
	  other - dump detailed info about every symbol in the table. Watch
		  out, this gets _very_ long if you have more than a few
		  symbols installed. */
void symbol_table::print(int detail) {
    int i;
    symbol *tmp;
    procedure_symbol *proc;  // Needed for safe downcasting below.
    function_symbol *func;   // Needed for safe downcasting below.
    array_symbol *arr;       // Needed for safe downcasting below.
    parameter_symbol *par;   // Needed for safe downcasting below.
    constant_symbol *con;    // Needed for safe downcasting below.

    if(detail == 2) {
	if(pool_pos > 0) {
	    unsigned int pos, k;
	    unsigned int len;
	    pos = 0;
	    while(pos < strlen(string_pool)) {
		len = (int)string_pool[pos];
		cout << len;
		for(k = pos+1; k < pos+len+1; k++) {
		    cout << string_pool[k];
		}
		pos += len+1;
	    }
	    cout << endl;

	    int j;
	    // cout << string_pool << endl;
	    for(j = 0; j < pool_pos; j++)
		cout << "-";
	    cout << "^" << " (pool_pos = " << pool_pos << ")" << endl;
	} else
	    cout << "(String pool empty)" << endl;
	return;
    }

    if(detail == 3) {
	cout << "Hash table:\n";
	int j;
	for(j = 0; j < MAX_HASH; j++) {
	    if(hash_table[j])
		cout << j << ": " << hash_table[j] << endl;
	}
	return;
    }

    cout << endl << "Symbol table (size = " << sym_pos << "):\n";

    switch(detail) {
	case 1:
	    // Element 0 is the global environment, "program.".
	    cout << "Pos  Name      Lev Hash Back Offs Type "
		 << "     Tag\n";
	    cout << "---------------------------------------"
		 << "--------\n";
	    for(i = 0; i < sym_pos+1; i++) {
		tmp = sym_table[i];
		if(tmp == NULL) {
		    cout << i << ": " << "NULL" << endl;
		    continue;
		}

		cout << setw(3) << i << ": ";
		cout.flags(ios::left);
		cout << setw(12) << pool_lookup(tmp->id);
		cout.flags(ios::right);
		cout << tmp->level
		     << setw(5) << tmp->hash_link << setw(5)
		     << tmp->back_link << setw(5) << tmp->offset << " ";

		cout.flags(ios::left);
		cout << setw(10);
		cout << pool_lookup(sym_table[tmp->type]->id);
		cout << setw(14);
		switch(tmp->tag) {
		    case SYM_UNDEF:
			cout << "SYM_UNDEF";
			break;
		    case SYM_NAMETYPE:
			cout << "SYM_NAMETYPE";
			break;
		    case SYM_VAR:
			cout << "SYM_VAR";
			break;
		    case SYM_PARAM:
			par = tmp->get_parameter_symbol();
			cout << "SYM_PARAM";
			if(par->preceding != NULL) {
			    cout << setw(7) << "prec = "
				 << setw(12) <<
				pool_lookup(par->preceding->id);
			}
			break;
		    case SYM_PROC:
			proc = tmp->get_procedure_symbol();
			cout << "SYM_PROC" << setw(6) << "lbl = "
			     << setw(3) << proc->label_nr << setw(9)
			     << "ar_size = " << setw(3) << proc->ar_size;
			break;
		    case SYM_FUNC:
			func = tmp->get_function_symbol();
			cout << "SYM_FUNC" << setw(6) << "lbl = "
			     << setw(3) << func->label_nr << setw(9)
			     << "ar_size = " << setw(3) << func->ar_size;
			break;
		    case SYM_ARRAY:
			arr = tmp->get_array_symbol();
			cout << "SYM_ARRAY" << setw(7) << "card = "
			     << setw(4) << arr->array_cardinality;
			break;
		    case SYM_CONST:
			con = tmp->get_constant_symbol();
			if(con->type == integer_type)
			    cout << "SYM_CONST" << setw(7) << "value = "
				 << con->const_value.ival;
			else if(con->type == real_type)
			    cout << "SYM_CONST" << setw(7) << "value = "
				 << con->const_value.rval;
			else
			    cout << "SYM_CONST" << setw(7) << "value = "
				 << "(error: bad type)";
			break;
		}
		cout.flags(ios::right);
		cout << setw(0) << endl;
	    }
	    break;
	default:
	    for(i = 0; i < sym_pos+1; i++) {
		tmp = sym_table[i];
		cout << "Pos = " << i << " -----------------------------\n"
		     << tmp;
	    }
	    break;
    }
}



/*** String pool methods ***/

/* Convenience method for capitalizing strings. Called by the scanner.
   Note that using this method without deleting the string afterwards causes
   a memory leak. */
   
char *symbol_table::capitalize(const char *s) {
    char *capitalized_s = new char[strlen(s)+1]; // The result string.
    unsigned int i;                              // Since toupper gives unsigned.
    
    for(i = 0; i < strlen(s); i++) {
	capitalized_s[i] = (unsigned char)toupper(s[i]);
    }

    capitalized_s[i] = '\0';
    
    return capitalized_s;
}

 /* Install a string into the pool table and return its index.
    The table is on the form <string1 length>string1<string2 length>string2...
    Note that the null char denotes the end of the entire pool,
    NOT the separator of two strings. It is appended in pool_lookup, though.
    Snapshot:
    7INTEGER4REAL4READ5WRITE4PROG1A\0
				   ^
				   pool_pos
 */
 
pool_index symbol_table::pool_install(char *s) {
    long old_pos;       // The return value, ie, the start of the string.
    
    // Make sure pool is not full. If it is, double pool size.
    if(pool_pos + (int)strlen(s) >= pool_length) {
	char *tmp_pool = new char[2*pool_length]; // Tmp storage.

	pool_length *= 2;                         // Double pool size.
	strcpy(tmp_pool, string_pool);            // Copy to tmp storage.
	delete[] string_pool;
	string_pool = tmp_pool;
    }

    old_pos = pool_pos;       

    // This is not really a pretty solution but it works for now. Some sort
    // struct with length/char * would be a more general solution, since this
    // way we're limited to strings that fit within 255 bytes.
    if(strlen(s) >= 255) {
	fatal("symbol_table::pool_install: Too long string");
	return 0;
    }
    
    // First install the length of the string.
    string_pool[pool_pos++] = (unsigned char)strlen(s);
    string_pool[pool_pos] = '\0';
    
    // Add the string itself to the end of the pool.
    strcat(string_pool, s);
    
    // Move pool_pos to the end of the new entry.
    pool_pos += strlen(s); 

    return old_pos;
}


/* Allocate memory for and return a string given a pool_index. */

char *symbol_table::pool_lookup(const pool_index p) {
    int i;                    // Counter used to find next space.
    char *s;                  // The string to be returned.
    char *start;              // The start of the string.

    assert(p < pool_pos);     // Catch references to beyond last string.
    
    start = &string_pool[p+1];  // Start points to first char of the sought
                                // string. We just use it to index the pool,
                                // it is never assigned to.
    
    // p points to the char holding the length of the sought string.
    i = (int)string_pool[p];
    
    s = new char[i+1];      // We only want to return a string of i chars, plus
                            // one extra for the null terminator.
    strncpy(s, start, i);   
    s[i] = '\0';
    
    return s;
}


/* Compare two strings, return 1 if equal, return 0 otherwise. */

int symbol_table::pool_compare(const pool_index pool_p1,
			       const pool_index pool_p2) {
    int result;                                         // Return value. 

    assert(pool_p1 < pool_pos && pool_p2 < pool_pos);   // Catch too large pos.

    // Compare the two strings. strcmp returns 0 if they're equal.
    result = !strcmp(pool_lookup(pool_p1), pool_lookup(pool_p2)); 
    return result;
}


/* Remove the last entry into the string pool. */

pool_index symbol_table::pool_forget(const pool_index pool_p) {
    char *last_entry;        

    last_entry = pool_lookup(pool_p);

    // Make sure that this really is the last entry. 
    assert((pool_p + (int)strlen(last_entry)) == pool_pos - 1);

    pool_pos = pool_p;              // Back up pool_pos one entry.
    string_pool[pool_pos] = '\0';   // Terminate the string pool there.
    return pool_pos;                // Mostly useful for debugging.
}


/* Convert a scanned string into a better format: Strip the leading and
   trailing quotes, and convert any internal double quotes to single ones.
   The first arg will be filled in with the fixed string, the second arg is
   the string that needs fixing. */
   
char *symbol_table::fix_string(const char *old_str) {
    char *new_str = new char[strlen(old_str)-2+1];
    unsigned int i;              // Unsigned since strlen returns unsigned.
    int j;                       // Index for new_str.

    // Make sure the string is at least ''.
    assert(strlen(old_str) >= 2);

    j = 0;
    new_str = new char[strlen(old_str)-2+1];

    // Start on 1 to skip the first quote. End on strlen-1 to skip the last
    // quote.
    for(i = 1; i < strlen(old_str)-1; i++) {
	// Compact double quotes to single quotes.
	if(old_str[i] == '\'' &&
	   i < strlen(old_str)-2 &&
	   old_str[i+1] == '\'') {
	    continue;
	}
	new_str[j++] = old_str[i];
    }

    new_str[j] = '\0';                       // Terminate string.
    
    return new_str;
}




/*** Hash table methods. ***/

 /* Uses the hash_x33 algorithm. Returns an index into the symbol table
    given a string. The string is probably attained using pool_lookup(). */
 hash_index symbol_table::hash(const pool_index p) {
    int len;                             // Length of s.
    unsigned int h;                      // Magical hash value variable.
    char *s;                             // Hash string. Causes memory leak.

    s = pool_lookup(p);                  // Get (and allocate) the string.
    len = (int)strlen(s);             
    h = 0;


    while(len > 0) {                     // Calculate the hash value.
	h = (h << 5) + h + *s++;   
	len--;
    }

    return h % MAX_HASH;                 // Memory leak since s is still
                                         //   allocated. However, since we've
                                         //   changed it, delete[] s is not a
                                         //   good idea at all.
}



/*** Display methods. ***/

 /* Return sym_index pointer to the current environment, ie, block level. */
 sym_index symbol_table::current_environment() {
    return block_table[current_level];
}


/* Increase the current_level by one. */
 void symbol_table::open_scope() {
 /*  Your code here. */
}


/* Decrease the current_level by one. Return sym_index to new environment. */
sym_index symbol_table::close_scope() {
  /*  Your code here. */
   return NULL_SYM;
}


/*** Main symbol table methods. ***/

 /* Return a sym_index to the sought symbol (or 0 if none was found), given
    a string_pool index. Starts searching in the current block level and
    follows hash links outwards. */
sym_index symbol_table::lookup_symbol(const pool_index pool_p) {  //%% OK
    // Your code here. 

	return NULL_SYM;   
}


/* Returns a symbol * given a sym_index, or NULL if no symbol found. */

symbol *symbol_table::get_symbol(const sym_index sym_p) {
    if(sym_p == NULL_SYM)
	return NULL;

    return sym_table[sym_p];
}


/* Given a sym_index, we return the id field of the symbol. The scanner needs
   this information in order to treat already-installed identifiers properly
   if shared strings are implemented. */
   
pool_index symbol_table::get_symbol_id(const sym_index sym_p) {
    // This should never happen, but paranoia never hurts. Note also that
    // 0 really does point to the name of the global level, which in our case
    // is 'program.'. But no program working correctly should ever refer to
    // it, by name or symbol...
    if(sym_p == NULL_SYM)
	return 0;
    
    return sym_table[sym_p]->id;
}


/* Given a sym_index, we return the type field of the symbol (which is in
   itself a sym_index to a type symbol). We need to be able to access this
   information in parser.y, and this is a convenient way to do it. */
   
sym_index symbol_table::get_symbol_type(const sym_index sym_p) {
    if(sym_p == NULL_SYM) 
	return void_type;

    return sym_table[sym_p]->type;
}


/* Given a sym_index, we return the tag field of the symbol. This is a
   convenience method used in parser.y so we don't need to explicitly handle
   symbols in it when differing various types of identifiers from one
   another. */
 sym_type symbol_table::get_symbol_tag(const sym_index sym_p) {
    if(sym_p == NULL_SYM)
	return SYM_UNDEF;

    return sym_table[sym_p]->tag;
}


/* We get a sym_index to a symbol, and a sym_index to a type. We set the
    symbol's type pointer to the second argument. Used to set the correct
    return type for a function. Used in parser.y. */
    
void symbol_table::set_symbol_type(const sym_index sym_p,
				   const sym_index type_p) {
    if(sym_p == NULL_SYM)
	return;

    sym_table[sym_p]->type = type_p;
}


/* Install a symbol in the symbol table or return a sym_index to it if it was
   already installed. Note that the various subclasses of 'symbol' need to
   be used here. This function is called by the various enter_* methods.
   Take a look at those enter_methods when writing this one.
   The parameter 'tag' can have one of eight different types, see the file symtabb.hh
   for the type definition of sym_type.
   Remember that the attribute 'tag' and 'id' will be set when creating 
   a new symbol inside the symbol constructor (take a look at symbol.cc).*/
   
sym_index symbol_table::install_symbol(const pool_index pool_p,
				       const sym_type tag) {
		/* Your code here */
	
	    	// Return index to the symbol we just created.
    		return 0;
		
}


/* Enter a constant into the symbol table. The value is an integer. The type
   argument is a sym_index pointer to the correct type.
   This function is used from within parser.y. Currently we call using the
   field data instead of a symbol, since we're not keeping position information
   in the *_symbol classes at the moment. */
sym_index symbol_table::enter_constant(position_information *pos,
				       const pool_index pool_p,
				       const sym_index type,
				       const int ival) {
    sym_index sym_p;

    // Install a constant_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_CONST);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
    constant_symbol *con = tmp->get_constant_symbol();

    // Make sure it's not already been declared.
    if(con->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << con << endl;
	return sym_p; // returns the first symbol
    }

    // Set up the constant-specific fields.
    con->type = type;
    con->tag = SYM_CONST;
    
    con->const_value.ival = ival;
    sym_table[sym_p] = con;
    
    return sym_p;
}


/* Enter a constant into the symbol table. The value is a real. The type
   argument is a sym_index pointer to the correct type.
   This function is used from within parser.y. Currently we call using the
   field data instead of a symbol, since we're not keeping position information
   in the *_symbol classes at the moment. */ 
sym_index symbol_table::enter_constant(position_information *pos,
				       const pool_index pool_p,
				       const sym_index type,
				       const float rval) {
    sym_index sym_p;

    // Install a constant_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_CONST);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
       
    constant_symbol *con = tmp->get_constant_symbol();
    
    // Make sure it's not already been declared.
    // Inside install_symbol, the 'tag' should got the value SYM_UNDEF
    // (which is set by deafault when creating a symbol in the symbol
    // creator).
    // if it happens to has another value, then the symbol already exists 
    // and should not be redeclared!
    if(con->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << con << endl;
	return sym_p; // returns the original symbol
    }

    // Set up the constant-specific fields.
    con->type = type; // set the type to real_type
    con->tag = SYM_CONST; // set the tag to SYM_CONST
    con->const_value.rval = rval; // set the value (real type) 
    
    sym_table[sym_p] = con; // insert the symbol into the symbol table

    return sym_p;
}


/* Enter a variable into the symbol table. This function is used from within
   parser.y. */
sym_index symbol_table::enter_variable(position_information *pos,
				       const pool_index pool_p,
				       const sym_index type) {
    sym_index sym_p;

    // Install a variable_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_VAR);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
    // Without this check, the test program will crash until you have
    // finished your install_symbol method.
    if(tmp == NULL) {
	fatal("install_symbol not yet working correctly.");
	return NULL_SYM;
    }
    // Make sure it's not already been declared. If it was, then we give a
    // message about this and simply return sym_p. This will cause trouble
    // later, though. NOTE: What to do when this happens?
    if(tmp->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << tmp << endl;
	return sym_p; // returns the original symbol
    }

    variable_symbol *var = tmp->get_variable_symbol();
    
    // Set up the variable-specific fields.
    var->type = type;
    var->tag = SYM_VAR;

    // This information is used later on when we allocate memory space on
    // activation frames. We need to know how many bytes the variable will
    // take up.

    // We re-use tmp for some more casting here. The current block can either
    // be a function or a procedure, and we need to differ the two. Fortunately
    // we can use the tag field for this, since it's common to all symbols.
    tmp = sym_table[current_environment()];
    if(tmp->tag == SYM_FUNC) {
	function_symbol *cur_func = tmp->get_function_symbol();
	var->offset = cur_func->ar_size;
	cur_func->ar_size += get_size(type);
	sym_table[current_environment()] = cur_func;
    } else {
	procedure_symbol *cur_proc = tmp->get_procedure_symbol();
	var->offset = cur_proc->ar_size;		
	cur_proc->ar_size += get_size(type);
	sym_table[current_environment()] = cur_proc;
    }	
    
    sym_table[sym_p] = var;
    
    return sym_p;
}


/* Convenience method used by quads.cc when installing temporary variables.
   Position information is irrelevant in that case. */
sym_index symbol_table::enter_variable(pool_index pool_p, sym_index type) {
    return enter_variable(NULL, pool_p, type);
}


/* Enter an array into the symbol table. This function is used in parser.y.
   NOTE: We currently assume that parser.y only allows integer index types.
   If that part's changed, we'll need to pass the type of the index as an
   argument to this function as well. */
sym_index symbol_table::enter_array(position_information *pos,
				    const pool_index pool_p,
				    const sym_index type,
				    const int cardinality) {
    sym_index sym_p;

    // Install an array_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_ARRAY);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
    array_symbol *arr = tmp->get_array_symbol();

    // Make sure it's not already been declared.
    if(arr->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << arr << endl;
	return sym_p; // returns the original symbol
    }

    // Set up the array-specific fields.
    arr->type = type;
    arr->tag = SYM_ARRAY;
    arr->array_cardinality = cardinality;
    
    // This is redundant, really, as the grammar stands currently... It can
    // only be an integer.
    
    arr->index_type = integer_type; 

    // This information is used later on when we allocate memory space on
    // activation frames. We need to know how many bytes the variable will
    // take up, and at what offset to find it.
    // We do some more casting here. The current block can either
    // be a function or a procedure, and we need to differ the two. Fortunately
    // we can use the tag field for this, since it's common to all symbols.
    tmp = sym_table[current_environment()];

    // We only do this if the array had a legal index. The reason is that the
    // value we use for illegal indexes happens to be -1, and using that value
    // here would mess up the offsets and ar_sizes to no end. The illegal
    // index being set represents trying to declare an array with a non-integer
    // cardinalityx type, such as a constant with a real value. We obviously
    // can't call this method with a float as the last argument, so we take
    // this approach instead.
    if(cardinality != ILLEGAL_ARRAY_CARD) {
	if(tmp->tag == SYM_FUNC) {
	    function_symbol *cur_func = tmp->get_function_symbol();
	    arr->offset = cur_func->ar_size;
	    cur_func->ar_size += cardinality * get_size(type);
	    sym_table[current_environment()] = cur_func;
	} else {
	    procedure_symbol *cur_proc = tmp->get_procedure_symbol();
	    arr->offset = cur_proc->ar_size;	
	    cur_proc->ar_size += cardinality * get_size(type);
	    sym_table[current_environment()] = cur_proc;
	}	
    }
    sym_table[sym_p] = arr;
    
    return sym_p;
}
    

/* Enter a function_symbol into the symbol table. */
sym_index symbol_table::enter_function(position_information *pos,
				       const pool_index pool_p) {
    sym_index sym_p;

    // Install a function_symbol in the symbol table. 
    // The function type will be set i labb3, the parser, because
    // Diesel's grammar doesn't let us know the type of the symbol when
    // the name is installed.
    // When testing labb2, test nr 3 a function type will be set, 
    // but it is done inside the testprogram symtabtest.cc
    sym_p = install_symbol(pool_p, SYM_FUNC);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
    function_symbol *func = tmp->get_function_symbol();

    // Make sure it's not already been declared.
    if(func->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << func << endl;
	return sym_p; // returns the original symbol
    }

    // Set up the function-specific fields.
    func->tag = SYM_FUNC;
    func->last_parameter = NULL;    // Parameters are added later on.
    
    // This will grow as local variables and temporaries are added.
    func->ar_size = 0; 
    func->label_nr = get_next_label();

    sym_table[sym_p] = func;

    return sym_p;
}


/* Enter a procedure_symbol into the symbol table. */
sym_index symbol_table::enter_procedure(position_information *pos,
					const pool_index pool_p) {
    // Your code here.

      return NULL_SYM;
}


/* Enter a parameter into the symbol table. */
sym_index symbol_table::enter_parameter(position_information *pos,
					const pool_index pool_p,
					const sym_index type) {
    sym_index         sym_p;
    parameter_symbol *tmp_param;
    int               param_offset = 0;

    // Install a parameter_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_PARAM);

    // This extra mess is required for safe downcasting, so we can access
    // the fields specific to this subclass of symbol.
    symbol *tmp = sym_table[sym_p];
    parameter_symbol *par = tmp->get_parameter_symbol();
    
    // Make sure it's not already been declared.
    if(par->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << par << endl;
    	return sym_p; // returns the original symbol
    }

    // Again we have to do some downcasting... This really looks a lot less
    // ugly in a language that doesn't differ a function from a procedure.
    // All we want is the pointer to the first parameter. But we don't know
    // if we're inside a function or a procedure, so we need to find that out
    // first. Then we get the link, and loop down it to calculate offset and
    // finally add this new parameter on at the end.
    // This code works because in parser.y, we do open_scope() _before_ we
    // call enter_parameter. So the current_environment() is the new function
    // or procedure, not the one from which it's being called. If this part
    // is confusing, don't be afraid to ask someone. :)
    tmp = sym_table[current_environment()];

    if(tmp->tag == SYM_FUNC) {
	function_symbol *func = tmp->get_function_symbol();
	tmp_param = func->last_parameter; // This is the old last parameter.
	func->last_parameter = par;       // Make 'par' the new last parameter.
    } else if(tmp->tag == SYM_PROC) {
	procedure_symbol *proc = tmp->get_procedure_symbol();
	tmp_param = proc->last_parameter; // This is the old last parameter.
	proc->last_parameter = par;       // Make 'par' the new last parameter.
    } else {
	fatal("Compiler confused about scope, aborting.");
	return 0;
    }

    // Link the old parameter list into the preceding field of the last one,
    // ie, the one we're currently installing.
    par->preceding = tmp_param;       
    
    // We loop through the parameters starting with the last and working our
    // way forward.
    while(tmp_param != NULL) {
	
	param_offset += tmp_param->size;
	tmp_param = tmp_param->preceding;
    }
    
    // Set up the parameter-specific fields.
    par->offset = param_offset;
    par->tag = SYM_PARAM;
    par->size = get_size(type);
    par->type = type;

    sym_table[sym_p] = par;
    
    return sym_p;
}


/* Enter a nametype (integer, real, etc) into the symbol table.
   We could just do this in the constructor initalizing the
   table, but in a language where you can define new types, this function is
   needed. So we prepare Diesel for expanding, even if this function
   currently doesn't do any spectacular things. :) */
sym_index symbol_table::enter_nametype(position_information *pos,
				       const pool_index pool_p) {
    sym_index sym_p;

    // Install a nametype_symbol in the symbol table.
    sym_p = install_symbol(pool_p, SYM_NAMETYPE);

    // Make sure it's not already been declared.
    if(sym_table[sym_p]->tag != SYM_UNDEF) {
	type_error(pos) << "Redeclaration: " << sym_table[sym_p] << endl;
    }

    // Set up the nametype-specific fields.
    sym_table[sym_p]->tag = SYM_NAMETYPE;
    sym_table[sym_p]->type = void_type;

    return sym_p;
} 

    
    



