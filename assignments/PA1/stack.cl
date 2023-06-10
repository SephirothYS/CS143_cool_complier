(*
 *  CS164 Fall 94
 *
 *  Programming Assignment 1
 *    Implementation of a simple stack machine.
 *
 *  Skeleton file
 *)
 
 class Node{
    val : String;
    next : Node;

    init(s:String,n:Node) : Node{
        {
        val <- s;
        next <- n;
        self;
        }
    };

    getNext() : Node{
        next
    };

    getVal() : String{
        val
    };


 };

 class Stack{
    top : Node;

    push(s : String) : Stack{
        {
        top <- (new Node).init(s, top);
        self;
        }
    };

    pop() : Stack{
        {
        if(isvoid top) then
            self
        else{
            top <- top.getNext();
            self;
        }
        fi;
        }
    };

    top() : Node{
        top
    };

 };

 class StackCommand{
    
    getCommand() : String{
        "No such definition."
    };



 };

 class PushCommand inherits StackCommand{
    num : String;

    init(n : String) : PushCommand{
        {
        num <- n;
        self;
        }
    };

    getCommand() : String{
        num
    };

    exec(s : Stack) : Object{
        s.push(num)
    };

 }; 

 class PlusCommand inherits StackCommand{
    num1 : String;
    num2 : String;
    node : Node;
    aToi : A2I <- new A2I;

    getCommand() : String{
        "+"
    };

    exec(s : Stack) : Object{
    {   
        node <- s.top();
        if(isvoid node) then
            self
        else{
            num1 <- s.top().getVal();
            s.pop();
            num2 <- s.top().getVal();
            s.pop();
            s.push( aToi.i2a( aToi.a2i(num1) + aToi.a2i(num2) ) );
        }
        fi;
    }
    };

 };

 class SwapCommand inherits StackCommand{
    num1 : String;
    num2 : String;
    node : Node;

    getCommand() : String{
        "s"
    };

    exec(s : Stack) : Object{
    {
        node <- s.top();
        if(isvoid node) then
            self
        else{
            num1 <- s.top().getVal();
            s.pop();
            num2 <- s.top().getVal();
            s.pop();
            s.push(num1);
            s.push(num2);
        }
        fi;
    }
    };

 };

 class DCommand inherits StackCommand{
    cur : Node;
    io : IO <- new IO;

    getCommand() : String{
        "d"
    };

    exec(s : Stack) : Object{
        {
            cur <- s.top();

            while(not isvoid cur) loop{
                io.out_string(cur.getVal().concat("\n"));
                cur <- cur.getNext();
            }
            pool;
            self;
            
        }
    };

 };

 class ECommand inherits StackCommand{
    command : String;
    node : Node;

    getCommand() : String{
        "e"
    };

    exec(s : Stack) : Object{
        {
        node <- s.top();
        if(isvoid node) then
            self
        else{
            command <- node.getVal();
            if(command = "+") then{
                s.pop();
                (new PlusCommand).exec(s);
            }
            else{
                if(command = "s") then{
                    s.pop();
                    (new SwapCommand).exec(s);
                }
                else{
                    node <- s.top();
                }
                fi;
            }
            fi;
        }
        fi;
        }
    };

 };

 class Main inherits IO{
    stack : Stack <- new Stack;
    flag : Bool;
    command : String;

    main() : Object{
        {
            flag <- true;
            while(flag) loop{
                out_string(">");
                command <- in_string();
                if (command = "e") then
                    (new ECommand).exec(stack)
                else
                    if(command = "d") then
                        (new DCommand).exec(stack)
                    else
                        if(command = "x") then
                            flag <- false
                        else
                            let p : PushCommand <- new PushCommand.init(command) 
                            in{
                                p.exec(stack);
                            }
                        fi
                    fi
                fi;
            }
            pool;
        }
    };
 };
