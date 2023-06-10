class Main{ main():Int{0};};

--method type_check
class A{
    foo(): String{0};
    foo2(): Notexist{0};
};

--attribute type_check
class B{
    self: String;
    b: A <- 0;
};

-- fomals type_check
class C{
    foo(a:Notexist,b:SELF_TYPE) : Int{0};
};

class Correct{
    fun(a:Int,b:Str): Int{0};
};

--unmatch formal-type
class D{
    x : Correct <- new Correct;
    y : Correct <- new Correct;
    foo(): Int{ x.fun("a",y)}; 
};

-- duplicate case
class E {
  foo(x:Int) : Object {
    {
    case x of 
        a:Int => a; 
        c:Int => c;
    esac;
    }
    };
};

-- dispatch undefined method, method called with wrong number of formals
class F{
    x : Correct <- new Correct;
    foo():Object{fun()};
    foo2():Object{x.fun(0)};
};

-- bad if condition
class G{
    foo():Object{{
        if 50 then 1 else 0
        fi;}
    };
};

-- bad while condition
class H{
  x:String;
  foo(x:Int) : Object {while x loop 0 pool};
};

-- bad let expr
class I{
    foo():Object{
        let self:Int <- 0 in 10
    };
    foo2():Object{
        let a:Int <- "0" in 10
    };
};

-- bad calculate
class J{
    foo():Object{
        1 + "a"
    };
    foo2(): Object{
        1 - "a"
    };
    foo3(): Object{
        1 * "a"
    };
    foo4(): Object{
        1 / "a"
    };
    foo5(): Object{
        not"a"
    };
    foo6(): Object{
        1 = "a"
    };
    foo7(): Object{
        1 = true
    };
    foo8(): Object{
        true = "a"
    };
    foo9(): Object{
        1 <= "a"
    };
};

-- new undefined class
class K{
    foo(): Object{
        new L
    };
};

