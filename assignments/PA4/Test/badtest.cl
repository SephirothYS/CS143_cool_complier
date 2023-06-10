class Main { main() : Int {0 }; };

-- an attribute named self
class A {
  self:A <- new A;
  foo(x:String) : String {x <- "a boy named sue"};
};

-- bad init , moo does not declare
class Cow { 
      moo : Object;
};

class Chicken {
      sound : Object <- moo;
};

-- bad calculate
class B {
	inky():Object { 1 + "hello" };
};

-- bad dispatch
class Animal {};

class Cat inherits Animal {
      meow() : String { "meow" };
};
      
class Blerg {
      foo() : Object {
	    let cat : Animal <- new Cat 
	    in cat.meow()
      };
};

-- bad equal
class C {
  x:String;
  foo(x:Int) : Object {x = "test"};
};

