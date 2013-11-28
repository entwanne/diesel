#ifndef __CODEGEN_HH__
#define __CODEGEN_HH__


#include <fstream>
using namespace std;


/* This is just to make the code more readable. */
typedef char register_type;

/* These are the registers we will be using. Would have been nice to use
   an enum here, but enums can't be used to index arrays without an
   explicit cast so we use constants instead. */
const register_type o0 = 0;
const register_type o1 = 1;
const register_type o2 = 2;
const register_type o3 = 3;
const register_type o4 = 4;
const register_type o5 = 5;
const register_type i0 = 6;
const register_type f0 = 7;
const register_type f1 = 8;
const register_type f2 = 9;


// The old display register is stored at [%fp+DISPLAY_REG_OFFSET].
const int DISPLAY_REG_OFFSET = 64;

// %fp+FIRST_ARG_OFFSET points at the first arg.
const int FIRST_ARG_OFFSET = 68;

// 16*4 (dump space) + 4 (old display register) + 6*4 (args) = 92.
const int MIN_FRAME_SIZE = 92;

// Maximum number of formal parameters allowed. 
const int MAX_PARAMETERS = 127;
const int PARAMETER_STACK_SIZE = 128;



/* Prototypes required for code_generator interface (the arguments). */
class quad_list;
class symbol;



/* This class generates assembler code for the Sun Sparc architecture. */
class code_generator {
private:
    register_type reg[10][4];                         // Register array.
    
    ofstream      out;                                // Output file stream.
    
    int  align(int);                                  // Align a stack frame.
    void prologue(symbol *);                          // Initialize new env.
    void epilogue(symbol *);                          // Leave env.
    void expand(quad_list *q);                        // Quadlist -> assembler.
    void find(sym_index, int *, int *);               // Get variable/parameter
                                                      // level & offset.
    void fetch(sym_index, const register_type);       // memory -> register.
    void store(const register_type, sym_index);       // register -> memory.
    void array_address(sym_index, const register_type); // get array base addr.
    
public:
    // Constructor. Arg = filename of assembler outfile.
    code_generator(const char *);

    // Destructor.
    ~code_generator();
    void generate_assembler(quad_list *, symbol *env); // Interface.
};

#endif
