#ifndef __QUADS_HH__
#define __QUADS_HH__

/* Credits to David Byers for the design of this class. /Jonas */

/* We need these here to be able to use the class names in quads.cc properly...
   Sometimes, the relationship between .hh files is less than friendly. */
class ast_expression;
class ast_binaryoperation;
class ast_binaryrelation;


/* These are all the quads we will be using. The comments show what types
   of arguments they take. Note that 'int' can be either int or real, since
   we're representing reals as ieee 32-bit integers when we have come this
   far in the compiling. 'sym' is a sym_index, which is just a typedef for
   a long int (see symtab.hh). '-' means the argument is not used. */
typedef enum {
    q_rload,       // int, -, sym
    q_iload,       // int, -, sym
    q_inot,        // sym, -, sym
    q_ruminus,     // sym, -, sym
    q_iuminus,     // sym, -, sym
    q_rplus,       // sym, sym, sym
    q_iplus,       // sym, sym, sym
    q_rminus,      // sym, sym, sym
    q_iminus,      // sym, sym, sym
    q_ior,         // sym, sym, sym
    q_iand,        // sym, sym, sym
    q_rmult,       // sym, sym, sym
    q_imult,       // sym, sym, sym
    q_rdivide,     // sym, sym, sym
    q_idivide,     // sym, sym, sym
    q_imod,        // sym, sym, sym
    q_req,         // sym, sym, sym
    q_ieq,         // sym, sym, sym
    q_rne,         // sym, sym, sym
    q_ine,         // sym, sym, sym
    q_rlt,         // sym, sym, sym
    q_ilt,         // sym, sym, sym
    q_rgt,         // sym, sym, sym
    q_igt,         // sym, sym, sym
    q_rstore,      // sym, -, sym
    q_istore,      // sym, -, sym
    q_rassign,     // sym, -, sym
    q_iassign,     // sym, -, sym
    q_call,        // sym, int, sym (or - if a procedure)
    q_rreturn,     // int, sym, -
    q_ireturn,     // int, sym, -
    q_lindex,      // sym, sym, sym
    q_rrindex,     // sym, sym, sym
    q_irindex,     // sym, sym, sym
    q_itor,        // sym, -, sym
    q_jmp,         // int, -, -
    q_jmpf,        // int, sym, -
    q_param,       // sym, -, -
    q_labl,        // int, -, -
    q_nop          // -, -, -
} quad_op_type;
	

/* There already exists some "quad" struct in the solaris include files...
  *mutter* */
class quadruple;
class quad_list_element;
class quad_list_iterator;
class quad_list;



/* The quadruple class. A quadruple is a pseudo-assembler op-code with three
   arguments (more correctly, two arguments and one result), which depend on
   the op_code of the quad. See above. */
class quadruple {
private:
    void print(ostream&);
    
public:
    quad_op_type op_code;
    
    sym_index sym1;
    sym_index sym2;
    sym_index sym3;

    int int1;
    int int2;
    int int3;

    // To create a quad with a '-' argument (ie, not used), set the sym_index
    // value to NULL_SYM for that quad. See above.
    quadruple(quad_op_type, sym_index, sym_index, sym_index);
    quadruple(quad_op_type, int, sym_index, sym_index);
    quadruple(quad_op_type, sym_index, int, sym_index);

    friend ostream& operator<<(ostream &, quadruple *);
};


/* This class is simply an abstraction to prevent quads from having to
   contain a link to the next quad, which is a representation issue having
   nothing to do with the program it represents. */
class quad_list_element {
public:
    quadruple         *data;
    quad_list_element *next;

    quad_list_element(quadruple *, quad_list_element *);
};



/* This class lets us iterate over a quad_list in a convenient fashion. */
class quad_list_iterator {
    quad_list_element     *current;

public:
    quad_list_iterator(quad_list *q_list);
    quadruple *get_current();          // Return the current quad if any.
    quadruple *get_next();             // Return the next quad if any.
};



/* A list of quads. This list will eventually contain the entire program in
   quad operations. Or at least entire blocks at a time. Had we represented
   the entire program as an AST, the list would have contained the whole
   program, but since we don't, it doesn't. :-) */
class quad_list {
private:
    quad_list_element *head;           // Pointer to the first list element.
    quad_list_element *tail;           // Pointer to the rest of the elements.

    int               quad_nr;         // Used to get nice printouts.
    
    void              print(ostream&); // Used to get nice printouts.

public:
    int              last_label;       // Label marking the end of a quad list.

    quad_list(int);                    // Constructor. Arg == last_label.

    quad_list& operator+=(quadruple *q); // Add on a new quad last on the list.

    friend class quad_list_iterator;   // Allow the iterator access to private
                                       // data fields in this class.
    friend ostream& operator<<(ostream&, quad_list *);
};
    
    






#endif
