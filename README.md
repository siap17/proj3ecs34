# proj3ecs34
asma &amp; sia !!


In our project, we needed a bit of help to format the CSVBusSystem test cases, we needed to figure out how to handle the test cases from overwriting our MakeFile so I found this link on how to alter the test cases a little bit. 

This is the link to the website we used: https://learn.microsoft.com/en-us/cpp/standard-library/overloading-the-output-operator-for-your-own-classes?view=msvc-170. We also used this website for helping to overload the << Operator in our CSVBusSystem.cpp file since we were running into errors there. 

Along the way, we also needed some help from Copilot and some other resources online. 

Here is my conversation with CoPilot: 

User input: can you show me an example how to implement ostream wehn trying to overload the << operator for your own classes?


Copilot Response: 

Absolutely! Overloading the << operator for your own classes allows you to customize how your class objects are output using the ostream (like std::cout). Here's an example to illustrate this with a simple class:

Example Class: Point
cpp
#include <iostream>

class Point {
private:
    int x, y;

public:
    Point(int x, int y) : x(x), y(y) {}

    // Friend declaration to allow access to private members
    friend std::ostream& operator<<(std::ostream& os, const Point& point);
};

// Overloading the << operator
std::ostream& operator<<(std::ostream& os, const Point& point) {
    os << "Point(" << point.x << ", " << point.y << ")";
    return os;
}

int main() {
    Point p(3, 4);
    std::cout << p << std::endl;  // Output: Point(3, 4)
    return 0;
}
Explanation
Class Definition:

Define a class Point with private member variables x and y.

Add a constructor to initialize these member variables.

Friend Declaration:

Declare the operator<< function as a friend of the class to allow it access to the private members. This is necessary because the overloaded operator needs to access the private members x and y.

Overloading << Operator:

Define the operator<< function outside the class.

This function takes an std::ostream reference and a Point object as parameters.

Output the desired format to the stream (in this case, Point(x, y)).

Using the Overloaded Operator:

Create a Point object in the main function.

Use std::cout to print the object. The overloaded << operator is automatically called to format the output.

Example Output
Point(3, 4)
This example demonstrates the basic technique for overloading the << operator. You can customize the format based on your class requirements. If you have specific classes you'd like to apply this to, feel free to share, and I can help you implement the overload for them! 😊💻📚


For CSVBusSystem, I did also have to use some outside sources to figure out how to link my classes and SImplementation to my header files. Here is a link to a YouTube video I used and my chat with 

YouTube Video Link: https://www.google.com/url?sa=t&source=web&rct=j&opi=89978449&url=https://www.youtube.com/watch%3Fv%3Dru_u9N7jydU&ved=2ahUKEwivzaOX3e-LAxXqIUQIHX7hAywQwqsBegQIDxAG&usg=AOvVaw3kDX7YNxe-GauvSY0nBHNS

Conversation with ChatGPT: 

User input: how ddo i declare these error: ‘SStop’ was not declared in this scope
error: ‘SRoute’ was not declared in this scope
error: ‘TStopID’ was not declared in this scope
error: ‘SImplementation’ with no type

Response: These errors typically mean that the compiler does not know what SStop, SRoute, and TStopID are when it reaches SImplementation. Here are some steps to fix it:

1. Ensure Proper Includes
Make sure that SStop, SRoute, and TStopID are defined before you use them in SImplementation.

If they are defined in another header file, include that file in CSVBusSystem.h:
cpp
Copy
Edit
#include "SStop.h"
#include "SRoute.h"
#include "TStopID.h" // If TStopID is defined in a separate file
If TStopID is just a typedef (e.g., using TStopID = int;), ensure it's declared before SImplementation.
2. Use Forward Declarations
If SStop and SRoute are only used as pointers or references, you can forward declare them at the top of CSVBusSystem.h:

cpp
Copy
Edit
class SStop;
class SRoute;
using TStopID = int; // Or however TStopID is defined
This tells the compiler that SStop and SRoute exist before they are fully defined.

3. Check for Circular Dependencies
If SStop.h includes SRoute.h and vice versa, you may have a circular dependency. Try using forward declarations instead of including full headers wherever possible.

If the Errors Persist
Check that SStop.h and SRoute.h actually define SStop and SRoute (not missing definitions).
Ensure CSVBusSystem.h is included where it is needed (CSVBusSystem.cpp).
Verify that TStopID is defined before it is used.
Let me know if you need further clarification! 🚍

The conversation with ChatGPT was actually not the correct answer but it did help me reach the correct conclusion, which was to link my classes in my .header files for CSVBusSystem.h and BusSystem.h. 




For our Makefile, we ran into some issues and we needed to utilize some outside sources to help us a little bit. We used the following links to try and gain a better understanding of what was going wrong with out MakeFile: https://unix.stackexchange.com/questions/18807/where-can-i-find-a-list-of-make-error-codes and https://stackoverflow.com/questions/61442299/how-to-derive-the-meaning-of-error-produced-by-make. 
