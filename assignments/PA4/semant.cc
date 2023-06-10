

#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include "semant.h"
#include "utilities.h"



extern int semant_debug;
extern char *curr_filename;

//////////////////////////////////////////////////////////////////////
//
// Symbols
//
// For convenience, a large number of symbols are predefined here.
// These symbols include the primitive type and method names, as well
// as fixed names used by the runtime system.
//
//////////////////////////////////////////////////////////////////////
static Symbol 
    arg,
    arg2,
    Bool,
    concat,
    cool_abort,
    copy,
    Int,
    in_int,
    in_string,
    IO,
    length,
    Main,
    main_meth,
    No_class,
    No_type,
    Object,
    out_int,
    out_string,
    prim_slot,
    self,
    SELF_TYPE,
    Str,
    str_field,
    substr,
    type_name,
    val;
//
// Initializing the predefined symbols.
//
static void initialize_constants(void)
{
    arg         = idtable.add_string("arg");
    arg2        = idtable.add_string("arg2");
    Bool        = idtable.add_string("Bool");
    concat      = idtable.add_string("concat");
    cool_abort  = idtable.add_string("abort");
    copy        = idtable.add_string("copy");
    Int         = idtable.add_string("Int");
    in_int      = idtable.add_string("in_int");
    in_string   = idtable.add_string("in_string");
    IO          = idtable.add_string("IO");
    length      = idtable.add_string("length");
    Main        = idtable.add_string("Main");
    main_meth   = idtable.add_string("main");
    //   _no_class is a symbol that can't be the name of any 
    //   user-defined class.
    No_class    = idtable.add_string("_no_class");
    No_type     = idtable.add_string("_no_type");
    Object      = idtable.add_string("Object");
    out_int     = idtable.add_string("out_int");
    out_string  = idtable.add_string("out_string");
    prim_slot   = idtable.add_string("_prim_slot");
    self        = idtable.add_string("self");
    SELF_TYPE   = idtable.add_string("SELF_TYPE");
    Str         = idtable.add_string("String");
    str_field   = idtable.add_string("_str_field");
    substr      = idtable.add_string("substr");
    type_name   = idtable.add_string("type_name");
    val         = idtable.add_string("_val");
}



ClassTable::ClassTable(Classes classes) : semant_errors(0) , error_stream(cerr) {

    /* Fill this in */
    install_basic_classes();

    for(int i = classes->first(); classes->more(i); i = classes->next(i))
        add_to_class_table(classes->nth(i));

}

void ClassTable::install_basic_classes() {

    // The tree package uses these globals to annotate the classes built below.
   // curr_lineno  = 0;
    Symbol filename = stringtable.add_string("<basic class>");
    
    // The following demonstrates how to create dummy parse trees to
    // refer to basic Cool classes.  There's no need for method
    // bodies -- these are already built into the runtime system.
    
    // IMPORTANT: The results of the following expressions are
    // stored in local variables.  You will want to do something
    // with those variables at the end of this method to make this
    // code meaningful.

    // 
    // The Object class has no parent class. Its methods are
    //        abort() : Object    aborts the program
    //        type_name() : Str   returns a string representation of class name
    //        copy() : SELF_TYPE  returns a copy of the object
    //
    // There is no need for method bodies in the basic classes---these
    // are already built in to the runtime system.

    Class_ Object_class =
	class_(Object, 
	       No_class,
	       append_Features(
			       append_Features(
					       single_Features(method(cool_abort, nil_Formals(), Object, no_expr())),
					       single_Features(method(type_name, nil_Formals(), Str, no_expr()))),
			       single_Features(method(copy, nil_Formals(), SELF_TYPE, no_expr()))),
	       filename);

    // 
    // The IO class inherits from Object. Its methods are
    //        out_string(Str) : SELF_TYPE       writes a string to the output
    //        out_int(Int) : SELF_TYPE            "    an int    "  "     "
    //        in_string() : Str                 reads a string from the input
    //        in_int() : Int                      "   an int     "  "     "
    //
    Class_ IO_class = 
	class_(IO, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       single_Features(method(out_string, single_Formals(formal(arg, Str)),
										      SELF_TYPE, no_expr())),
							       single_Features(method(out_int, single_Formals(formal(arg, Int)),
										      SELF_TYPE, no_expr()))),
					       single_Features(method(in_string, nil_Formals(), Str, no_expr()))),
			       single_Features(method(in_int, nil_Formals(), Int, no_expr()))),
	       filename);  

    //
    // The Int class has no methods and only a single attribute, the
    // "val" for the integer. 
    //
    Class_ Int_class =
	class_(Int, 
	       Object,
	       single_Features(attr(val, prim_slot, no_expr())),
	       filename);

    //
    // Bool also has only the "val" slot.
    //
    Class_ Bool_class =
	class_(Bool, Object, single_Features(attr(val, prim_slot, no_expr())),filename);

    //
    // The class Str has a number of slots and operations:
    //       val                                  the length of the string
    //       str_field                            the string itself
    //       length() : Int                       returns length of the string
    //       concat(arg: Str) : Str               performs string concatenation
    //       substr(arg: Int, arg2: Int): Str     substring selection
    //       
    Class_ Str_class =
	class_(Str, 
	       Object,
	       append_Features(
			       append_Features(
					       append_Features(
							       append_Features(
									       single_Features(attr(val, Int, no_expr())),
									       single_Features(attr(str_field, prim_slot, no_expr()))),
							       single_Features(method(length, nil_Formals(), Int, no_expr()))),
					       single_Features(method(concat, 
								      single_Formals(formal(arg, Str)),
								      Str, 
								      no_expr()))),
			       single_Features(method(substr, 
						      append_Formals(single_Formals(formal(arg, Int)), 
								     single_Formals(formal(arg2, Int))),
						      Str, 
						      no_expr()))),
	       filename);

    class_map[Object_class->get_name()] = Object_class;
    inheritance_graph[Object_class->get_name()] = Object_class->get_parent();
    class_map[IO_class->get_name()] = IO_class;
    inheritance_graph[IO_class->get_name()] = IO_class->get_parent();
    class_map[Str_class->get_name()] = Str_class;
    inheritance_graph[Str_class->get_name()] = Str_class->get_parent();
    class_map[Int_class->get_name()] = Int_class;
    inheritance_graph[Int_class->get_name()] = Int_class->get_parent();
    class_map[Bool_class->get_name()] = Bool_class;
    inheritance_graph[Bool_class->get_name()] = Bool_class->get_parent();

}

////////////////////////////////////////////////////////////////////
//
// semant_error is an overloaded function for reporting errors
// during semantic analysis.  There are three versions:
//
//    ostream& ClassTable::semant_error()                
//
//    ostream& ClassTable::semant_error(Class_ c)
//       print line number and filename for `c'
//
//    ostream& ClassTable::semant_error(Symbol filename, tree_node *t)  
//       print a line number and filename
//
///////////////////////////////////////////////////////////////////

ostream& ClassTable::semant_error(Class_ c)
{                                                             
    return semant_error(c->get_filename(),c);
}    

ostream& ClassTable::semant_error(Symbol filename, tree_node *t)
{
    error_stream << filename << ":" << t->get_line_number() << ": ";
    return semant_error();
}

ostream& ClassTable::semant_error()                  
{                                                 
    semant_errors++;                            
    return error_stream;
} 


void ClassTable::add_to_class_table(Class_ c){
    Symbol name = c->get_name();
    Symbol parent = c->get_parent();
    if(parent == Bool || parent == SELF_TYPE || parent == Str){
        ostream& err_stream = semant_error(c);
        err_stream << "Class " << name << " cannot inherit class " << parent << ".\n";

    }
    else if(name == SELF_TYPE || name == Bool || name == Str || name == Int || name == Object){
        ostream& err_stream = semant_error(c);
        err_stream << "Redefinition of basic class " << name << ".\n";

    }
    else if(class_map.count(name) == 0 && inheritance_graph.count(name) == 0){
        class_map[name] = c;
        inheritance_graph[name] = parent;

    }
    else{
        ostream& err_stream = semant_error(c);
        err_stream << "Class " << name << " was previously defined.\n";

    }
}

bool ClassTable::is_valid() {
    bool is_main_defined = false;
    bool is_classes_valid = true;
    for(std::map<Symbol, Symbol>::iterator iter = inheritance_graph.begin();
    iter != inheritance_graph.end(); iter++){
        Symbol child = iter->first;
        Symbol parent = iter->second;
        if(child == Main)
            is_main_defined = true;
        while(parent != No_class){
            if(parent == child){
                ostream& err_stream  = semant_error(class_map[child]);
                err_stream << "Class "<< child <<", or an ancestor of "<< child <<", is involved in an inheritance cycle.\n";
                is_classes_valid = false;
                break;
            }
            else if(inheritance_graph.count(parent) == 0){
                ostream& err_stream = semant_error(class_map[child]);
                err_stream << "Class " << child << " inherits from an undefined class " << parent << ".\n";
                is_classes_valid = false;
                break;
            }
            else
                parent = inheritance_graph[parent];
        }
    }
    if(is_classes_valid == false) return false;
    if(is_main_defined == false){
        ostream& err_stream = semant_error();
        err_stream << "Class Main is not defined.\n";
        return false;
    }
    return true;
}

Symbol ClassTable::lub(Symbol class1, Symbol class2) {
    Symbol c1 = class1;
    Symbol parent = Object;
    while(c1 != Object){
        Symbol c2 = class2;
        while(c2 != Object){
            if(c1 == c2){
                parent = c1;
                return parent;
            }
            c2 = inheritance_graph[c2];
        }
        c1 = inheritance_graph[c1];
    }
    return parent;
}

bool ClassTable::is_child(Symbol child, Symbol parent){
    Symbol c = child;
    if(parent == Object)
        return true;
    while(c != Object){
        if(c == parent)
            return true;
        c = inheritance_graph[c];
    }
    return false;
}

bool ClassTable::class_exists(Symbol c) {
    return (inheritance_graph.count(c) > 0);
}

Class_ ClassTable::get_class(Symbol class_name) {
    return class_map[class_name];
}

Formals ClassTable::get_formals(Symbol class_name, Symbol method_name){
    Symbol cname = class_name;
    while(cname != No_class){
        Class_ c = class_map[cname];
        Formals f = c->get_formals(method_name);
        if(f != NULL)
            return f;
        cname = inheritance_graph[cname];
    }
    return NULL;
}

Symbol ClassTable::get_return_type(Symbol class_name, Symbol method_name) {
    Symbol cname = class_name;
    while(cname != No_class){
        Class_ c = class_map[cname];
        Symbol r = c->get_return_type(method_name);
        if (r != NULL)
            return r;
        cname = inheritance_graph[cname];
    }
    return NULL;
}

Symbol ClassTable::get_ancestor_method_class(Symbol class_name, Symbol method_name) {
    Symbol cname = inheritance_graph[class_name];
    while(cname != No_class){
        Class_ c = class_map[cname];
        if(c->get_return_type(method_name) != NULL)
            return c->get_name();
        cname = inheritance_graph[cname];
    }
    return NULL;
}

bool ClassTable::check_method_signature(Symbol c1, Symbol c2, Symbol method_name) {
    Class_ class1 = class_map[c1];
    Class_ class2 = class_map[c2];
    Formals f1 = class1->get_formals(method_name);
    Formals f2 = class2->get_formals(method_name);
    Symbol ret1 = class1->get_return_type(method_name);
    Symbol ret2 = class2->get_return_type(method_name);
    int i = f1->first();
    int j = f2->first();

    while( f1->more(i) && f2->more(j)) {
        if(f1->nth(i)->get_type() != f2->nth(j)->get_type() )
            return false;
        i = f1->next(i);
        j = f2->next(j);
    }
    if(f1->more(i) || f2->more(j))
        return false;
    if(ret1 != ret2)
        return false;
    return true;
}

/*   This is the entry point to the semantic checker.

     Your checker should do the following two things:

     1) Check that the program is semantically correct
     2) Decorate the abstract syntax tree with type information
        by setting the `type' field in each Expression node.
        (see `tree.h')

     You are free to first do 1), make sure you catch all semantic
     errors. Part 2) can be done in a second stage, when you want
     to build mycoolc.
 */
void program_class::semant()
{
    initialize_constants();

    /* ClassTable constructor may do some semantic analysis */
    ClassTable *classtable = new ClassTable(classes);
    if(!classtable->errors() && classtable->is_valid()){
        type_env_t env;
        env.om = new SymbolTable<Symbol, Symbol>();
        env.curr = NULL;
        env.ct = classtable;

        for(int i = classes->first(); classes->more(i); i = classes->next(i)){
            env.om->enterscope();
            env.curr = classes->nth(i);
            classes->nth(i)->init_class(env);
            classes->nth(i)->type_check(env);
            env.om->exitscope();
        }
    }

    /* some semantic analysis code may go here */

    if (classtable->errors()) {
	cerr << "Compilation halted due to static semantic errors." << endl;
	exit(1);
    }
}

Symbol class__class::get_name(){
    return name;
}

Symbol class__class::get_parent(){
    return parent;
}

void class__class::init_class(type_env_t env){
    if (name != Object){
        env.ct->get_class(parent)->init_class(env);
    }
    std::map<Symbol,bool> name_map;
    for(int i = features->first(); features->more(i); i = features->next(i)){
            features->nth(i)->add_to_environment(env);
        // if(name_map.count(features->nth(i)->get_name()) == 0){ 
        //     name_map[features->nth(i)->get_name()] = true;
        //     features->nth(i)->add_to_environment(env);
        // }
        // else{
        //     ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        //     if(features->nth(i)->is_method()){
        //         err_stream << "Method " << features->nth(i)->get_name() << " is multiply defined.\n";
        //     }
        //     else
        //         err_stream << "Attribut " << features->nth(i)->get_name() << " is multiply defined.\n";
        // }

    }
}

Formals class__class::get_formals(Symbol method){
    for(int i = features->first(); features->more(i); i = features->next(i)){
        Feature feature = features->nth(i);
        if(feature->is_method() && feature->get_name() == method)
            return feature->get_formals();
    }
    return NULL;
}

Symbol class__class::get_return_type(Symbol method){
    for(int i = features->first(); features->more(i);i = features->next(i)){
        Feature feature = features->nth(i);
        if(feature->is_method() && feature->get_name() == method)
            return feature->get_return_type();
    }
    return NULL;
}


bool method_class::is_method() { return true;}
bool attr_class::is_method() {return false;}

Formals method_class::get_formals() {return formals;}
Symbol method_class::get_return_type(){ return return_type;}

Formals attr_class::get_formals() {return NULL;}
Symbol attr_class::get_return_type() {return NULL; }

Symbol method_class::get_name() {return name;}
Symbol attr_class::get_name() {return name;}

void method_class::add_to_environment(type_env_t env) {}
void attr_class::add_to_environment(type_env_t env){
    if(env.om->probe(name) == NULL)
        env.om->addid(name, &type_decl);
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Attribute " << name
                    << " is multiply defined in class.\n";
    }
}

Symbol formal_class::get_type() {return type_decl;}
Symbol branch_class::get_type() {return type_decl;}

Class_ class__class::type_check(type_env_t env){
    for(int i = features->first(); features->more(i); i = features->next(i)){
        features->nth(i)->type_check(env);
    }
    return this;
}


Feature method_class::type_check(type_env_t env){
    env.om->enterscope();
    Symbol curr_class = env.curr->get_name();
    env.om->addid(self,&curr_class);
    for(int i = formals->first(); formals->more(i); i = formals->next(i)){
        formals->nth(i)->type_check(env);
    }
    Symbol tret = expr->type_check(env)->type;

    Symbol ancestor = NULL;
    if((ancestor = env.ct->get_ancestor_method_class(curr_class, name)) != NULL){
        if(!env.ct->check_method_signature(ancestor, curr_class, name)) {
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
            err_stream << "Overriding method signature of "<< name << "for class "
                        << curr_class << " doesn't match method signature for ancestor "
                        << ancestor << ".\n";
        }
    }
    int name_count = 0;
    for(int i = 0; i < env.curr->features_vec.size();i++){
        if(name == env.curr->features_vec[i]) name_count++;
    }
    if(name_count > 0){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Method " << name << " is multiply defined.\n";
        
    }
    if(return_type == SELF_TYPE) {
        if(tret != SELF_TYPE){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
            err_stream << "Inferred return type " << tret << " of method " << name 
                        << " does not conform to declared return type " << return_type << ".\n";
                        
        }
    }
    else if(!env.ct->class_exists(return_type)) {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Undefined return type " << return_type << " in method " << name << ".\n";    
    }
    else{
        if(tret == SELF_TYPE)
            tret = env.curr->get_name();
        if(!env.ct->is_child(tret, return_type)) {
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
            err_stream << "Inferred return type " << tret << " of method " << name 
                        << " does not conform to declared return type " << return_type << ".\n";
        }
    }
    env.curr->features_vec.push_back(name);
    env.om->exitscope();
    return this;
}

Feature attr_class::type_check(type_env_t env){
    env.om->enterscope();
    Symbol curr_class = env.curr->get_name();
    env.om->addid(self, &curr_class);
    Symbol t1 = init->type_check(env)->type;
    env.om->exitscope();
    if(t1 == SELF_TYPE)
        t1 = env.curr->get_name();
    if(t1 != No_type){
        if(!(env.ct->is_child(t1, type_decl))){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
            err_stream << "Inferred type " << t1
                        << " of initialization of attribute "<< name << " does not conform to declared type "<< type_decl << ".\n";

        }
    }
    if(name == self){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "'self' cannot be the name of an attribute.\n";

    }
    return this;
}

Formal formal_class::type_check(type_env_t env) {
    if (env.om->probe(name) != NULL) {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Duplicate formal " << name << ".\n";
    }
    else if (type_decl == SELF_TYPE) {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Formal parameter " << name << " cannot have type SELF_TYPE.\n";
    }
    else if (!env.ct->class_exists(type_decl)){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Class " << type_decl << " of formal parameter " << name << " is undefined.\n";
    }
    else
        env.om->addid(name, &type_decl);
    return this;
}

Symbol branch_class::type_check(type_env_t env){
    if (env.om->probe(name) != NULL){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Identifier " << name << " already defined in current scope.\n";
        return Object;
    }
    env.om->addid(name, &type_decl);
    return expr->type_check(env)->type;
}

Expression assign_class::type_check(type_env_t env){
    Symbol t1 = *env.om->lookup(name);
    Symbol t2 = expr->type_check(env)->type;
    if(t2 == SELF_TYPE){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Cannot assign to 'self'.\n";
        type = Object;
    }
    else if (env.ct->is_child(t2,t1))
        type = t2;
    else if(!*env.om->lookup(name)){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Assignment to undeclared variable"<< name<<".\n";
    }
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << t2 << " is not a subclass of " << t1 << ".\n";
        type = Object;
    }
    return this;
}

Expression static_dispatch_class::type_check(type_env_t env){
    std::vector<Symbol> eval_types;
    Symbol t0 = expr->type_check(env)->type;
    if(t0 == SELF_TYPE)
        t0 = env.curr->get_name();
    if(env.ct->is_child(t0, type_name)){
        for(int i = actual->first(); actual->more(i); i = actual->next(i)){
            Symbol tn = actual->nth(i)->type_check(env)->type;
            // if(tn == SELF_TYPE)
            //     tn = env.curr->get_name();
            // cout << tn;
            eval_types.push_back(tn);
        }
        Formals formals = env.ct->get_formals(t0,name);
        Symbol ret_type = env.ct->get_return_type(t0, name);
        if(formals == NULL || ret_type == NULL){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
            err_stream << "Dispatch to undefined method " << name << ".\n";
            type = Object;
            return this;
        }

        std::vector<Symbol>::iterator iter = eval_types.begin();
        int fi = formals->first();
        while(iter != eval_types.end() && formals->more(fi)){
            Symbol eval_type = *iter;
            Symbol declared_type = formals->nth(fi)->get_type();
            if(declared_type == SELF_TYPE){
                ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
                err_stream<< "Formal parameter cannot have type SELF_TYPE.\n";
            }
            else if(!env.ct->is_child(eval_type, declared_type)){
                ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
                // err_stream << "Formal parameter declared type " << declared_type
                            // << " is not a subclass of " << eval_type << ".\n"; 
                // Symbol para_name = formals->nth(fi)->get_name();
                err_stream << "In call of method " << name << ", type " << eval_type << " of parameter " << formals->nth(fi)->get_name()
                            << " does not conform to declared type " << declared_type << ".\n";
            }
            ++iter;
            fi = formals->next(fi);
        }
        if(iter != eval_types.end() || formals->more(fi)){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
            err_stream << "Method "<< name << " called with wrong number of arguments.\n";
        }

        if (ret_type == SELF_TYPE)
            type = t0;
        else
            type = ret_type;
    }
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Expression type " << t0 << " does not conform to declared static dispatch type "
                    << type_name << ".\n";
        type = Object;
    }
    return this;
}

Expression dispatch_class::type_check(type_env_t env){
    std::vector<Symbol> eval_types;
    Symbol t0 = expr->type_check(env)->type;
    Symbol curr = t0;
    if(t0 == SELF_TYPE)
        curr = env.curr->get_name();
    for(int i = actual->first(); actual->more(i); i = actual->next(i)){
        Symbol tn = actual->nth(i)->type_check(env)->type;
        if(tn == SELF_TYPE)
            tn = env.curr->get_name();
        eval_types.push_back(tn);
    }
    Formals formals = env.ct->get_formals(curr, name);
    Symbol ret_type = env.ct->get_return_type(curr, name);
    if(formals == NULL || ret_type == NULL){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Dispatch to undefined method " << name << ".\n";
        type = Object;
        return this;
    }

    std::vector<Symbol>::iterator iter = eval_types.begin();
    int fi = formals->first();
    while(iter != eval_types.end() && formals->more(fi)){
        Symbol eval_type = *iter;
        Symbol declared_type = formals->nth(fi)->get_type();
        if(declared_type == SELF_TYPE){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
            err_stream << "Formal parameter cannot have tyoe SLEF_TYPE.\n";

        }
        else if(!env.ct->is_child(eval_type, declared_type)){
            ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
            // err_stream << "Formal parameter declared type " << declared_type
                        // << " is not a subclass of " << eval_type << ".\n";
            err_stream << "In call of method " << name << ", type " << eval_type << " of parameter " << formals->nth(fi)->get_name()
                    << " does not conform to declared type " << declared_type << ".\n";

        }
        iter++;
        fi = formals->next(fi);
    }
    if(iter != eval_types.end() || formals->more(fi)){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
        err_stream << "Method "<< name << " called with wrong number of arguments.\n";

    }

    if(ret_type == SELF_TYPE)
        type = t0;
    else
        type = ret_type;
    return this;
}

Expression cond_class::type_check(type_env_t env){
    Symbol t1 = pred->type_check(env)->type;
    Symbol t2 = then_exp->type_check(env)->type;
    if(t2 == SELF_TYPE)
        t2 = env.curr->get_name();
    Symbol t3 = else_exp->type_check(env)->type;
    if(t3 == SELF_TYPE)
        t3 = env.curr->get_name();
    if(t1 == Bool)
        type = env.ct->lub(t2,t3);
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(),this);
        err_stream << "Predicate of 'if' does not have type Bool.\n";
        type = Object;
    }
    return this;
}

Expression loop_class::type_check(type_env_t env){
    Symbol t1 = pred->type_check(env)->type;
    Symbol t2 = body->type_check(env)->type;
    if(t1 == Bool)
        type = Object;
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Loop condition does not have type Bool.\n";
        type = Object;
    }
    return this;
}

Expression typcase_class::type_check(type_env_t env) {
    Symbol t0 = expr->type_check(env)->type;

    for(int i = cases->first(); cases->more(i) ; i = cases->next(i)){
        for( int j = cases->first(); cases->more(j); j = cases->next(j)){
            if( i != j && cases->nth(i)->get_type() == cases->nth(j)->get_type()){
                ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
                err_stream << "Duplicate branch " << cases->nth(i)->get_type()
                            << " in case statement.\n";
                type = Object;
                return this;
            }
        }
    }
    Symbol tn = cases->nth(cases->first())->type_check(env);
    for(int i = cases->first(); cases->more(i); i = cases->next(i)){
        if(i != cases->first()){
            env.om->enterscope();
            tn = env.ct->lub(tn, cases->nth(i)->type_check(env));
            env.om->exitscope();
        }
    }
    type = tn;
    return this;
}

Expression block_class::type_check(type_env_t env){
    Symbol t1;
    for(int i = body->first(); body->more(i); i = body->next(i)){
        t1 = body->nth(i)->type_check(env)->type;

    }
    type = t1;
    return this;
}

Expression let_class::type_check(type_env_t env) {
    if(identifier == self){
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "'self' cannot be bound in a 'let' expression.\n";
        type = Object;
    }
    else {
        Symbol t0 = type_decl;
        Symbol t1 = init->type_check(env)->type;

        if(t1 == No_type){
            env.om->enterscope();
            env.om->addid(identifier, &t0);
            Symbol t2 = body->type_check(env)->type;
            env.om->exitscope();
            type = t2;
        }
        else{
            if(env.ct->is_child(t1,t0)){
                env.om->enterscope();
                env.om->addid(identifier, &t0);
                Symbol t2 = body->type_check(env)-> type;
                env.om->exitscope();
                type = t2;
            }
            else{
                ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
                err_stream << "Inferred type "<< t1 << " of initialization of a does not conform to identifier's declared type " << t0 << ".\n";
                type = Object;
            }
        }
    }
    return this;
}

Expression plus_class::type_check(type_env_t env){
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if(t1 == Int && t2 == Int)
        type = Int;
    else{
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " + " << t2 << "\n";
        type = Object;
    }
    return this;
}

Expression sub_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if (t1 == Int && t2 == Int)
        type = Int;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " - " << t2 << "\n";
        type = Object;
    }
    return this;
}

Expression mul_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if (t1 == Int && t2 == Int)
        type = Int;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " * " << t2 << "\n";
        type = Object;
    }
    return this;
}

Expression divide_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if (t1 == Int && t2 == Int)
        type = Int;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " / " << t2 << "\n";
        type = Object;
    }
    return this;
}

Expression neg_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    if (t1 == Int)
        type = Int;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int argument: ~" << t1 << "\n";
        type = Object;
    }
    return this;
}

Expression lt_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if (t1 == Int && t2 == Int)
        type = Bool;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " < " << t2 << "\n";
        type = Object;
    }
    return this;
}
Expression eq_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;

    if ((t1 == Int && t2 != Int) || (t1 != Int && t2 == Int) ||
        (t1 == Str && t2 != Str) || (t1 != Str && t2 == Str) ||
        (t1 == Bool && t2 != Bool) || (t1 != Bool && t2 == Bool)) {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Illegal comparison with a basic type.\n";
        type = Object;
    }
    else {
        type = Bool;
    }
    return this;
}

Expression leq_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    Symbol t2 = e2->type_check(env)->type;
    if (t1 == Int && t2 == Int)
        type = Bool;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "non-Int arguments: " << t1 << " <= " << t2 << "\n";
        type = Object;
    }
    return this;
}

Expression comp_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    if (t1 == Bool)
        type = Bool;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Argument of 'not' has type "<< t1 <<" instead of Bool.\n";
        type = Object;
    }
    return this;
}


Expression int_const_class::type_check(type_env_t env) {
    type = Int;
    return this;
}

Expression bool_const_class::type_check(type_env_t env) {
    type = Bool;
    return this;
}

Expression string_const_class::type_check(type_env_t env) {
    type = Str;
    return this;
}

Expression new__class::type_check(type_env_t env) {
    if (env.ct->class_exists(type_name) || type_name == SELF_TYPE)
        type = type_name;
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "'new' used with undefined class " << type_name << ".\n";
        type = Object;
    }
    return this;
}

Expression isvoid_class::type_check(type_env_t env) {
    Symbol t1 = e1->type_check(env)->type;
    type = Bool;
    return this;
}

Expression no_expr_class::type_check(type_env_t env) {
    type = No_type;
    return this;
}

Expression object_class::type_check(type_env_t env) {
    if (name == self)
        type = SELF_TYPE;
    else if (env.om->lookup(name) != NULL)
        type = *(env.om->lookup(name));
    else {
        ostream& err_stream = env.ct->semant_error(env.curr->get_filename(), this);
        err_stream << "Undeclared identifier " << name << ".\n";
        type = Object;
    }
    return this;
}
