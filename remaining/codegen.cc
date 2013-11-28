#include <iostream>
#include <iomanip>
#include <fstream>
#include <stdio.h>
#include <string.h>
#include "symtab.hh"
#include "quads.hh"
#include "codegen.hh"

using namespace std;


extern int assembler_trace; // Defined in main.cc.

// Used in parser.y. Ideally the filename should be parametrized, but it's not
// _that_ important...
code_generator *code_gen = new code_generator("d.out");


// Constructor.
code_generator::code_generator(const char *object_file_name) {

    out.open(object_file_name);
    
    // Initialize register array.
    strcpy(reg[static_cast<int>(o0)], "%o0");
    strcpy(reg[static_cast<int>(o1)], "%o1");
    strcpy(reg[static_cast<int>(o2)], "%o2");
    strcpy(reg[static_cast<int>(o3)], "%o3");
    strcpy(reg[static_cast<int>(o4)], "%o4");
    strcpy(reg[static_cast<int>(o5)], "%o5");
    strcpy(reg[static_cast<int>(i0)], "%i0");
    strcpy(reg[static_cast<int>(f0)], "%f0");
    strcpy(reg[static_cast<int>(f1)], "%f1");
    strcpy(reg[static_cast<int>(f2)], "%f2");
    
    // Contains the preinstalled diesel functions: read, write, trunc.
    out << "#include \"diesel_glue.s\"" << endl;
}



/* Destructor. */
code_generator::~code_generator() {
    // Make sure we close the outfile before exiting the compiler.
    out << flush;
    out.close();
}



/* This method is called from parser.y when code generation is to start.
   The argument is a quad_list representing the body of the procedure, and
   the symbol for the environment for which code is being generated. */
void code_generator::generate_assembler(quad_list *q, symbol *env) {
    prologue(env);
    expand(q);
    epilogue(env);
}



/* This method aligns a frame size on an 8-byte boundary. Used by prologue().
 */
int code_generator::align(int frame_size) {
    return ((frame_size+7) / 8) * 8;
}



/* This method generates assembler code for initialisating a procedure or
   function. */
void code_generator::prologue(symbol *new_env) {
    //block_level level;          // The new scope level.
    //int nr_args;                // Nr of formal parameters.
    int ar_size;                // Activation record size.
    int label_nr;               // Assembler label nr.
    parameter_symbol *last_arg; // Used to move parameters onto the stack.

    // Again, we need a safe downcast for a procedure/function.
    // Note that since we have already generated quads for the entire block
    // before we expand it to assembler, the size of the activation record
    // is known here (ar_size).
    if(new_env->tag == SYM_PROC) {
	procedure_symbol *proc = new_env->get_procedure_symbol();
	ar_size = align(proc->ar_size + MIN_FRAME_SIZE);
	label_nr = proc->label_nr;
	last_arg = proc->last_parameter;
    } else if(new_env->tag == SYM_FUNC) {
	function_symbol *func = new_env->get_function_symbol();
	ar_size = align(func->ar_size + MIN_FRAME_SIZE);
	label_nr = func->label_nr;
	last_arg = func->last_parameter;
    } else {
	fatal("code_generator::prologue() called for non-proc/func");
	return;
    }

    out << "L" << label_nr << ":" << "\t\t\t" << "! " <<
	sym_tab->pool_lookup(new_env->id) << endl;

    if(assembler_trace)
	out << "\t" << "! PROLOGUE (" << short_symbols << new_env
	    << long_symbols << ")" << endl;

    /* Your code here. */
    
    out << flush;    
}



/* This method generates assembler code for leaving a procedure or function. */
void code_generator::epilogue(symbol *old_env) {
    //block_level level;          // The scope level we're leaving.
    
    if(assembler_trace)
	out << "\t" << "! EPILOGUE (" << short_symbols << old_env
	    << long_symbols << ")" << endl;
    /* Your code here. */
    
    out << flush;    
}



/* This function finds the display register level and offset for a variable or
   a parameter. Note the pass-by-reference arguments. */
void code_generator::find(sym_index sym_p, int *level, int *offset) {
    /* Your code here. */
    
}



/* This function fetches the value of a variable or a constant into a
   register. */
void code_generator::fetch(sym_index sym_p, register_type dest) {
    /* Your code here. */
    
}



/* This function stores the value of a register into a variable. */
void code_generator::store(register_type src, sym_index sym_p) {
    /* Your code here. */
    
}



/* This function fetches the base address of an array. */
void code_generator::array_address(sym_index sym_p, register_type dest) {
    /* Your code here. */
    
}



/* This method expands a quad_list into assembler code, quad for quad. */
void code_generator::expand(quad_list *q_list) {
    quadruple *q;           // Used to iterate through the list.
    int label;              // Assembler label.
    
    //int nr_args;            // Used for parameter generation.
    
    long quad_nr = 0;       // Just to make debug output easier to read.
    //symbol *tmp;            // Used for safe downcasting.
    
    // We use this iterator to loop through the quad list.
    quad_list_iterator *ql_iterator = new quad_list_iterator(q_list);
    
    q = ql_iterator->get_current(); // This is the head of the list.
    
    while(q != NULL) {
	quad_nr++;

	// We always do labels here so that a branch doesn't miss the
	// trace code.
	if(q->op_code == q_labl)
	    out << "L" << q->int1 << ":" << endl;

	// Debug output.
	if(assembler_trace)
	    out << "\t" << "! QUAD " << quad_nr << ": "
		<< short_symbols << q << long_symbols << endl;

	// The main switch on quad type. This is where code is actually
	// generated.
	switch(q->op_code) {
	    case q_iload:
	    case q_rload:
		out << "\t\t" << "set" << "\t" << q->int1 << ",%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_inot:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;
		out << "\t\t" << "be,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_ruminus:
		fetch(q->sym1, f0);
		out << "\t\t" << "fnegs" << "\t" << "%f0,%f1" << endl;
		store(f1, q->sym3);
		break;
		
	    case q_iuminus:
		fetch(q->sym1, o0);
		out << "\t\t" << "neg" << "\t" << "%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rplus:
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fadds" << "\t" << "%f0,%f1,%f2" << endl;
		store(f2, q->sym3);
		break;
		
	    case q_iplus:
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "add" << "\t" << "%o0,%o1,%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rminus:
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fsubs" << "\t" << "%f0,%f1,%f2" << endl;
		store(f2, q->sym3);
		break;
		
	    case q_iminus:
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "sub" << "\t" << "%o0,%o1,%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_ior:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;
		out << "\t\t" << "bne,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		fetch(q->sym2, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;
		out << "\t\t" << "bne,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_iand:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;
		out << "\t\t" << "be,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		fetch(q->sym2, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;
		out << "\t\t" << "be,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rmult:
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fmuls" << "\t" << "%f0,%f1,%f2" << endl;
		store(f2, q->sym3);
		break;
		
	    case q_imult:
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		// Note: We're calling routines from diesel_glue.s here.
		out << "\t\t" << "call" << "\t" << "Mul" << endl;
		out << "\t\t" << "nop" << endl;		
		store(o0, q->sym3);
		break;
		
	    case q_rdivide:
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fdivs" << "\t" << "%f0,%f1,%f2" << endl;
		
		out << "\t\t" << "fmovs" << "\t" << "%f2,%f2" << endl;
		store(f2, q->sym3);
		break;
		
	    case q_idivide:
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		// Note: We're calling routines from diesel_glue.s here.
		out << "\t\t" << "call" << "\t" << "Div" << endl;
		out << "\t\t" << "nop" << endl;		
		store(o0, q->sym3);
		break;
		
	    case q_imod:
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		// Note: We're calling routines from diesel_glue.s here.
		out << "\t\t" << "call" << "\t" << "Rem" << endl;
		out << "\t\t" << "nop" << endl;		
		store(o0, q->sym3);
		break;
		
	    case q_req:
		label = sym_tab->get_next_label();
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fcmps" << "\t" << "%f0,%f1" << endl;
		out << "\t\t" << "nop" << endl;
		out << "\t\t" << "fbne,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_ieq:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "cmp" << "\t" << "%o0,%o1" << endl;
		out << "\t\t" << "bne,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rne:
		label = sym_tab->get_next_label();
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fcmps" << "\t" << "%f0,%f1" << endl;
		out << "\t\t" << "nop" << endl;
		out << "\t\t" << "fbe,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_ine:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "cmp" << "\t" << "%o0,%o1" << endl;
		out << "\t\t" << "be,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rlt:
		label = sym_tab->get_next_label();
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fcmpes" << "\t" << "%f0,%f1" << endl;
		out << "\t\t" << "nop" << endl;
		out << "\t\t" << "fbuge,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_ilt:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "cmp" << "\t" << "%o0,%o1" << endl;
		out << "\t\t" << "bge,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rgt:
		label = sym_tab->get_next_label();
		fetch(q->sym1, f0);
		fetch(q->sym2, f1);
		out << "\t\t" << "fcmpes" << "\t" << "%f0,%f1" << endl;
		out << "\t\t" << "nop" << endl;
		out << "\t\t" << "fbule,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_igt:
		label = sym_tab->get_next_label();
		fetch(q->sym1, o0);
		fetch(q->sym2, o1);
		out << "\t\t" << "cmp" << "\t" << "%o0,%o1" << endl;
		out << "\t\t" << "ble,a" << "\t" << "L" << label << endl;
		out << "\t\t" << "mov" << "\t" << "0,%o0" << endl;
		out << "\t\t" << "mov" << "\t" << "1,%o0" << endl;
		out << "L" << label << ":" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rstore:
	    case q_istore:
		fetch(q->sym1, o0);
		fetch(q->sym3, o1);
		out << "\t\t" << "st" << "\t" << "%o0,[%o1]" << endl;
		break;
		
	    case q_rassign:
	    case q_iassign:
		fetch(q->sym1, o0);
		store(o0, q->sym3);
		break;
		
	    case q_param:
		/* Your code here. */
		
		break;
		
	    case q_call:
		/* Your code here. */
		
		break;
		
	    case q_rreturn:
	    case q_ireturn:
		fetch(q->sym2, i0);
		out << "\t\t" << "ba" << "\t" << "L" << q->int1 << endl;
		out << "\t\t" << "nop" << endl;
		break;
		
	    case q_lindex:
		fetch(q->sym2, o1);
		array_address(q->sym1, o0);
		out << "\t\t" << "sll" << "\t" << "%o1,2,%o1" << endl;
		out << "\t\t" << "add" << "\t" << "%o0,%o1,%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_rrindex:
	    case q_irindex:
		fetch(q->sym2, o1);
		array_address(q->sym1, o0);
		out << "\t\t" << "sll" << "\t" << "%o1,2,%o1" << endl;
		out << "\t\t" << "ld" << "\t" << "[%o0+%o1],%o0" << endl;
		store(o0, q->sym3);
		break;
		
	    case q_itor:
		fetch(q->sym1, f0);
		out << "\t\t" << "fitos" << "\t" << "%f0,%f1" << endl;
		store(f1, q->sym3);
		break;
		
	    case q_jmp:
		out << "\t\t" << "ba" << "\t" << "L" << q->int1 << endl;
		out << "\t\t" << "nop" << endl;
		break;
		
	    case q_jmpf:
		fetch(q->sym2, o0);
		out << "\t\t" << "tst" << "\t" << "%o0" << endl;	
		out << "\t\t" << "be" << "\t" << "L" << q->int1 << endl;
		out << "\t\t" << "nop" << endl;
		break;
		
	    case q_labl:
		// We handled this one above already.
		break;
		
	    case q_nop:
		// q_nop quads should never be generated.
		fatal("code_generator::expand(): q_nop quadruple produced.");
		return;
	}

	// Get the next quad from the list.
	q = ql_iterator->get_next();
    }

    // Flush the generated code to file.
    out << flush;
}
