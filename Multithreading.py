from Main import function1, function2, Class1
import Main

# Using imported functions and classes
function1()
function2()
obj = Class1()
obj.method()

# Using the module directly
Main.function1()
Main.function2()
obj = Main.Class1()
obj.method()

# Example of using isinstance and modulo
if __annotations__:
    if isinstance(obj, Class1) and obj.method() % 2 == 0:
        print("The object is an instance of Class1 and the method result is even.")
