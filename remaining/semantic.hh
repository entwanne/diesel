#include "ast.hh"

class semantic;


extern semantic *type_checker; // Defined in semantic.cc.


class semantic {
private:
    // Check formal vs actual parameters. This function recurses down the
    // parameter lists. Called by check_parameters. Returns 1 if the last
    // two parameters match, 0 if there's an error.
    int                  chk_param(ast_id *,
				   parameter_symbol *,
				   ast_expr_list *);
public:
    // Compare formal vs. actual parameters in function/procedure calls.
    void                 check_parameters(ast_id *, ast_expr_list *);

    // Initiate type checking of a block of code.
    void                 do_typecheck(symbol *, ast_stmt_list *);
    
    // Perform type checking on a procedure/function/program body. Note that
    // the body is represented as an ast_stmt_list. See the productions for
    // comp_stmt and subprog_decl in parser.y.
    
    sym_index            check_binop1(ast_binaryoperation *);
    sym_index            check_binop2(ast_binaryoperation *, char *);
    sym_index            check_binrel(ast_binaryrelation *);
};

