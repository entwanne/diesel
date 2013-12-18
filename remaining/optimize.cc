#include "optimize.hh"

/*** This file contains all code pertaining to AST optimisation. It currently
     implements a simple optimisation called "constant folding". Most of the
     methods in this file are empty, or just relay optimize calls downward
     in the AST. If a more powerful AST optimization scheme were to be 
     implemented, only methods in this file should need to be changed. ***/


ast_optimizer *optimizer = new ast_optimizer();


/* The optimizer's interface method. Starts a recursive optimize call down
   the AST nodes, searching for binary operators with constant children. */
void ast_optimizer::do_optimize(ast_stmt_list *body) {
    if(body != NULL)
	body->optimize();
}


/* Returns 1 if an AST expression is a subclass of ast_binaryoperation,
   ie, eligible for constant folding. You might want to change this
   method if you extend the folding beyond integers and reals.*/
int ast_optimizer::is_binop(ast_expression *node) {
    switch(node->tag) {
	case AST_ADD:
	case AST_SUB:
	case AST_OR:
	case AST_AND:
	case AST_MULT:
	case AST_DIVIDE:
	case AST_IDIV:
	case AST_MOD:
	    return 1;
	default:
	    return 0;
    }
}



/* We overload this method for the various ast_node subclasses that can
   appear in the AST. By use of virtual (dynamic) methods, we ensure that
   the correct method is invoked even if the pointers in the AST refer to
   one of the abstract classes such as ast_expression or ast_statement. */
void ast_node::optimize() {
    fatal("Trying to optimize abstract class ast_node.");
}

void ast_statement::optimize() {
    fatal("Trying to optimize abstract class ast_statement.");
}

void ast_expression::optimize() {
    fatal("Trying to optimize abstract class ast_expression.");
}

void ast_lvalue::optimize() {
    fatal("Trying to optimize abstract class ast_lvalue.");
}

void ast_binaryoperation::optimize() {
    fatal("Trying to optimize abstract class ast_binaryoperation.");
}

void ast_binaryrelation::optimize() {
    fatal("Trying to optimize abstract class ast_binaryrelation.");
}



/*** The optimize methods for the concrete AST classes. ***/

/* Optimize a statement list. This one you get for free.*/
void ast_stmt_list::optimize() {
    if(preceding != NULL)
	preceding->optimize();
    if(last_stmt != NULL)
	last_stmt->optimize();
}

static ast_expression* fold_constants_id(ast_expression* node);

/* Optimize a list of expressions. */
void ast_expr_list::optimize() {
    /* Your code here. */
    if(preceding != NULL)
	preceding->optimize();
    if(last_expr != NULL)
	last_expr = fold_constants_id(optimizer->fold_constants(last_expr));
}


/* Optimize an elsif list. */
void ast_elsif_list::optimize() {
    /* Your code here. */
    if(preceding != NULL)
	preceding->optimize();
    if(last_elsif != NULL)
	last_elsif->optimize();
}


/* An identifier's value can change at run-time, so we can't perform
   constant folding optimization on it unless it is a constant.
   Thus we just do nothing here. It can be treated in the fold_constants()
   method, however. */
void ast_id::optimize() {
}

void ast_indexed::optimize() {
/* Your code here. */
    id = optimizer->fold_constants(id)->get_ast_id();
    index = optimizer->fold_constants(index);
}


static ast_expression* fold_constants_id(ast_expression* node)
{
    if(node->tag == AST_ID) {
	ast_id* id = node->get_ast_id();
	if(sym_tab->get_symbol_tag(id->sym_p) == SYM_CONST) {
	    constant_symbol* csym = sym_tab->get_symbol(id->sym_p)->get_constant_symbol();
	    if(csym->type == integer_type)
		node = new ast_integer(node->pos, csym->const_value.ival);
	    else
		node = new ast_real(node->pos, csym->const_value.rval);
	}
    }
    return node;
}

/* This convenience method is used to apply constant folding to all 
   binary operations. It returns either the resulting optimized node or the
   original node if no optimization could be performed. */
ast_expression *ast_optimizer::fold_constants(ast_expression *node) {
    /* Your code here. */
    if(node == NULL)
	return NULL;
    node->optimize();
    if(!is_binop(node))
	return node;
    ast_node_type tag = node->tag;
    ast_binaryoperation* op = node->get_ast_binaryoperation();
    op->left = fold_constants_id(op->left);
    op->right = fold_constants_id(op->right);
    if ((op->left->tag != AST_INTEGER && op->left->tag != AST_REAL) || op->right->tag != op->left->tag)
	return node;
    if(op->left->tag == AST_INTEGER) {
	int ileft = op->left->get_ast_integer()->value;
	int iright = op->right->get_ast_integer()->value;
	switch(tag) {
	    case AST_ADD:
		node = new ast_integer(op->left->pos, ileft + iright);
		break;
	    case AST_SUB:
		node = new ast_integer(op->left->pos, ileft - iright);
		break;
	    case AST_OR:
		node = new ast_integer(op->left->pos, ileft || iright);
		break;
	    case AST_AND:
		node = new ast_integer(op->left->pos, ileft && iright);
		break;
	    case AST_MULT:
		node = new ast_integer(op->left->pos, ileft * iright);
		break;
	    case AST_DIVIDE:
		node = new ast_real(op->left->pos, (float) ileft / (float) iright);
		break;
	    case AST_IDIV:
		node = new ast_integer(op->left->pos, ileft / iright);
		break;
	    case AST_MOD:
		node = new ast_integer(op->left->pos, ileft % iright);
		break;
	    default:
		break;
	}
    }
    else {
	float rleft = op->left->get_ast_real()->value;
	float rright = op->right->get_ast_real()->value;
	switch(tag) {
	    case AST_ADD:
		node = new ast_real(op->left->pos, rleft + rright);
		break;
	    case AST_SUB:
		node = new ast_real(op->left->pos, rleft - rright);
		break;
	    case AST_MULT:
		node = new ast_real(op->left->pos, rleft * rright);
		break;
	    case AST_DIVIDE:
		node = new ast_real(op->left->pos, rleft / rright);
		break;
	    default:
		break;
	}
    }
    return node;
}


void ast_add::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_sub::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_mult::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_divide::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_or::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_and::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_idiv::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_mod::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}



/* We can apply constant folding to binary relations as well. */
void ast_equal::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_notequal::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_lessthan::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}

void ast_greaterthan::optimize() {
    /* Your code here. */
    left = optimizer->fold_constants(left);
    right = optimizer->fold_constants(right);
}



/*** The various classes derived from ast_statement. ***/

void ast_procedurecall::optimize() {
/* Your code here. */
    id = optimizer->fold_constants(id)->get_ast_id();
    parameter_list->optimize();
}


void ast_assign::optimize() {
    /* Your code here. */
    lhs->optimize();
    rhs = optimizer->fold_constants(rhs);
}


void ast_while::optimize() {
    /* Your code here. */
    condition = optimizer->fold_constants(condition);
    if(body != NULL)
	body->optimize();
}
    

void ast_if::optimize() {
    /* Your code here. */
    condition = optimizer->fold_constants(condition);
    if(body != NULL)
	body->optimize();
    if(elsif_list != NULL)
	elsif_list->optimize();
    if(else_body != NULL)
	else_body->optimize();
}


void ast_return::optimize() {
    /* Your code here. */
    if (value != NULL)
	value = optimizer->fold_constants(value);
}


void ast_functioncall::optimize() {
/* Your code here. */
    id = optimizer->fold_constants(id)->get_ast_id();
    parameter_list->optimize();
}

void ast_uminus::optimize() {
    /* Your code here. */
    expr = optimizer->fold_constants(expr);
}

void ast_not::optimize() {
    /* Your code here. */
    expr = optimizer->fold_constants(expr);
}


void ast_elsif::optimize() {
    /* Your code here. */
    condition = optimizer->fold_constants(condition);
    if(body != NULL)
	body->optimize();
}



void ast_integer::optimize() {
    /* Your code here. */
}

void ast_real::optimize() {
    /* Your code here. */
}


void ast_cast::optimize() {
    /* Your code here. */
    expr = optimizer->fold_constants(expr);
}



void ast_procedurehead::optimize() {
    fatal("Trying to call ast_procedurehead::optimize()");
}


void ast_functionhead::optimize() {
    fatal("Trying to call ast_functionhead::optimize()");
}



	    
