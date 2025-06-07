#include <iostream>
#include <cmath>
#include <string>
#include <vector>
#include <algorithm>
#include <limits>
#include <fstream>
#include <chrono>
#include <thread>
#include <iomanip>

#ifdef _WIN32
#include <conio.h>
#else
#include <termios.h>
#include <unistd.h>
#endif

// Platform-specific functions
#ifdef _WIN32
void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int getch() {
    return ::getch();
}
#else
void sleep_ms(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

int getch() {
    struct termios oldt, newt;
    int ch;
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    ch = getchar();
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    return ch;
}
#endif

// Constants
const double e = 2.718281828459045;
const double pi = 3.141592653589793;

// Function expression parser
class ExpressionParser {
private:
    std::string expression;
    size_t position;

    double parseExpression();
    double parseTerm();
    double parseFactor();
    double parseFunction();
    double parseNumber();

public:
    ExpressionParser(const std::string& expr) : expression(expr), position(0) {
        // Remove spaces from the expression
        expression.erase(remove_if(expression.begin(), expression.end(), isspace), expression.end());
    }

    double evaluate(double x);
    double evaluateDerivative(double x, double h = 0.0001);
};

double ExpressionParser::parseExpression() {
    double result = parseTerm();

    while (position < expression.size()) {
        if (expression[position] == '+') {
            position++;
            result += parseTerm();
        } else if (expression[position] == '-') {
            position++;
            result -= parseTerm();
        } else {
            break;
        }
    }

    return result;
}

double ExpressionParser::parseTerm() {
    double result = parseFactor();

    while (position < expression.size()) {
        if (expression[position] == '*') {
            position++;
            result *= parseFactor();
        } else if (expression[position] == '/') {
            position++;
            double divisor = parseFactor();
            if (divisor == 0) {
                throw std::runtime_error("Division by zero");
            }
            result /= divisor;
        } else {
            break;
        }
    }

    return result;
}

double ExpressionParser::parseFactor() {
    double result;

    if (expression[position] == '(') {
        position++;
        result = parseExpression();
        if (position >= expression.size() || expression[position] != ')') {
            throw std::runtime_error("Missing closing parenthesis");
        }
        position++;
    } else if (isalpha(expression[position])) {
        result = parseFunction();
    } else {
        result = parseNumber();
    }

    // Handle exponentiation
    if (position < expression.size() && expression[position] == '^') {
        position++;
        double exponent = parseFactor();
        result = pow(result, exponent);
    }

    return result;
}

double ExpressionParser::parseFunction() {
    std::string function;
    while (position < expression.size() && isalpha(expression[position])) {
        function += expression[position++];
    }

    if (function == "x") {
        return 0; // Placeholder for variable x, will be replaced during evaluation
    }

    if (position >= expression.size() || expression[position] != '(') {
        throw std::runtime_error("Missing opening parenthesis after function name");
    }
    position++;

    double arg = parseExpression();

    if (position >= expression.size() || expression[position] != ')') {
        throw std::runtime_error("Missing closing parenthesis after function argument");
    }
    position++;

    if (function == "sin") {
        return sin(arg);
    } else if (function == "cos") {
        return cos(arg);
    } else if (function == "tan") {
        return tan(arg);
    } else if (function == "exp") {
        return exp(arg);
    } else if (function == "log" || function == "ln") {
        if (arg <= 0) {
            throw std::runtime_error("Invalid argument for logarithm");
        }
        return log(arg);
    } else if (function == "log10") {
        if (arg <= 0) {
            throw std::runtime_error("Invalid argument for logarithm");
        }
        return log10(arg);
    } else if (function == "sqrt") {
        if (arg < 0) {
            throw std::runtime_error("Invalid argument for square root");
        }
        return sqrt(arg);
    } else if (function == "abs") {
        return fabs(arg);
    } else {
        throw std::runtime_error("Unknown function: " + function);
    }
}

double ExpressionParser::parseNumber() {
    size_t start = position;
    
    // Handle negative numbers
    if (expression[position] == '-') {
        position++;
    }
    
    // Parse digits before decimal point
    while (position < expression.size() && isdigit(expression[position])) {
        position++;
    }
    
    // Parse decimal point and digits after it
    if (position < expression.size() && expression[position] == '.') {
        position++;
        while (position < expression.size() && isdigit(expression[position])) {
            position++;
        }
    }
    
    if (start == position) {
        throw std::runtime_error("Invalid number");
    }
    
    return std::stod(expression.substr(start, position - start));
}

double ExpressionParser::evaluate(double x) {
    // Replace x in the expression with the provided value
    std::string expr_copy = expression;
    size_t pos = 0;
    
    position = 0; // Reset position for parsing
    
    // Replace standalone 'x' variables
    while ((pos = expr_copy.find("x", pos)) != std::string::npos) {
        // Check if 'x' is a standalone variable and not part of a function name or another identifier
        bool standalone = true;
        if (pos > 0 && (isalpha(expr_copy[pos-1]) || isdigit(expr_copy[pos-1]))) {
            standalone = false;
        }
        if (pos < expr_copy.length() - 1 && (isalpha(expr_copy[pos+1]) || isdigit(expr_copy[pos+1]))) {
            standalone = false;
        }
        
        if (standalone) {
            std::string x_value = std::to_string(x);
            expr_copy.replace(pos, 1, x_value);
            pos += x_value.length();
        } else {
            pos++;
        }
    }
    
    expression = expr_copy;
    double result = parseExpression();
    expression = expr_copy; // Restore original expression
    return result;
}

double ExpressionParser::evaluateDerivative(double x, double h) {
    // Numerical derivative using central difference
    return (evaluate(x + h) - evaluate(x - h)) / (2 * h);
}

// Function manager to handle user-defined functions
class FunctionManager {
private:
    std::string expression;
    ExpressionParser* parser;

public:
    FunctionManager() : parser(nullptr) {}
    
    ~FunctionManager() {
        if (parser) {
            delete parser;
        }
    }
    
    bool loadFromFile(const std::string& filename) {
        std::ifstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        std::getline(file, expression);
        file.close();
        
        if (parser) {
            delete parser;
        }
        
        try {
            parser = new ExpressionParser(expression);
            // Test the parser with a simple value to catch basic syntax errors
            parser->evaluate(1.0);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing function: " << e.what() << std::endl;
            delete parser;
            parser = nullptr;
            return false;
        }
    }
    
    bool setExpression(const std::string& expr) {
        expression = expr;
        
        if (parser) {
            delete parser;
        }
        
        try {
            parser = new ExpressionParser(expression);
            // Test the parser with a simple value to catch basic syntax errors
            parser->evaluate(1.0);
            return true;
        } catch (const std::exception& e) {
            std::cerr << "Error parsing function: " << e.what() << std::endl;
            delete parser;
            parser = nullptr;
            return false;
        }
    }
    
    double evaluate(double x) {
        if (!parser) {
            throw std::runtime_error("No function defined");
        }
        return parser->evaluate(x);
    }
    
    double evaluateDerivative(double x, double h = 0.0001) {
        if (!parser) {
            throw std::runtime_error("No function defined");
        }
        return parser->evaluateDerivative(x, h);
    }
    
    std::string getExpression() const {
        return expression;
    }
};

// Numerical methods classes
class RegulaFalsi {
    double a, b, c;
    double y_c = std::numeric_limits<double>::max();
    FunctionManager& func;
    double tolerance;
    int max_iterations;

public: 
    RegulaFalsi(FunctionManager& f, double tol = 0.0001, int max_iter = 100) 
        : func(f), tolerance(tol), max_iterations(max_iter) {
        
        // Find initial bracket
        findBracket();
        
        sleep_ms(500);
        std::cout << std::endl << "Roots lie between (" << std::fixed << std::setprecision(4) << a 
                  << ", " << std::fixed << std::setprecision(4) << b << ")" << std::endl << std::endl;

        int count = 0;
        while (std::abs(y_c) >= tolerance && count < max_iterations) {
            count++;
            c = ((a * func.evaluate(b)) - (b * func.evaluate(a))) / (func.evaluate(b) - func.evaluate(a));
            y_c = func.evaluate(c);
            
            sleep_ms(200);
            std::cout << std::endl << count << ") Iteration: " << std::endl;
            std::cout << "c = " << std::fixed << std::setprecision(4) << c 
                      << "\t f(c) = " << std::fixed << std::setprecision(4) << y_c << std::endl;

            if (y_c >= tolerance) {
                b = c;
            } else if (y_c <= -tolerance) {
                a = c;
            }
        } 

        std::cout << std::endl << "Number of Iterations = " << count << std::endl;
        if (count >= max_iterations) {
            std::cout << "Maximum iterations reached, solution may not be accurate." << std::endl;
        }
        std::cout << "By Regula Falsi Method, c = " << std::fixed << std::setprecision(4) << c << " (approximately)";
    }
    
private:
    void findBracket() {
        // Ask user for search range
        double start, end, step;
        std::cout << "Enter search range for roots:" << std::endl;
        std::cout << "Start value: ";
        std::cin >> start;
        std::cout << "End value: ";
        std::cin >> end;
        std::cout << "Step size: ";
        std::cin >> step;
        
        bool found = false;
        for (double i = start; i <= end - step; i += step) {
            try {
                double f_i = func.evaluate(i);
                double f_i_next = func.evaluate(i + step);
                
                if (f_i * f_i_next <= 0) {  // Root exists between i and i+step
                    a = i;
                    b = i + step;
                    found = true;
                    break;
                }
            } catch (const std::exception& e) {
                std::cout << "Error evaluating function at x = " << std::fixed << std::setprecision(4) << i 
                          << ": " << e.what() << std::endl;
            }
        }
        
        if (!found) {
            std::cout << "No root found in the specified range. Please try a different range." << std::endl;
            std::cout << "Enter manual bracket values:" << std::endl;
            std::cout << "Lower bound a: ";
            std::cin >> a;
            std::cout << "Upper bound b: ";
            std::cin >> b;
            
            // Verify that the function changes sign
            if (func.evaluate(a) * func.evaluate(b) > 0) {
                std::cout << "Warning: Function does not change sign between a and b." << std::endl;
                std::cout << "The method may not converge." << std::endl;
            }
        }
    }
};

class SecantMethod {
    double a, b, c;
    double y_x = std::numeric_limits<double>::max();
    FunctionManager& func;
    double tolerance;
    int max_iterations;

public: 
    SecantMethod(FunctionManager& f, double tol = 0.0001, int max_iter = 100) 
        : func(f), tolerance(tol), max_iterations(max_iter) {
        
        // Ask user for initial approximations
        std::cout << "Enter two initial approximations:" << std::endl;
        std::cout << "First approximation (a): ";
        std::cin >> a;
        std::cout << "Second approximation (b): ";
        std::cin >> b;
        
        sleep_ms(500);
        std::cout << std::endl << "Starting with initial values (" << std::fixed << std::setprecision(4) << a 
                  << ", " << std::fixed << std::setprecision(4) << b << ")" << std::endl << std::endl;

        int count = 0;
        while (std::abs(y_x) >= tolerance && count < max_iterations) {
            count++;
            try {
                double fa = func.evaluate(a);
                double fb = func.evaluate(b);
                
                if (std::abs(fb - fa) < 1e-10) {
                    std::cout << "Division by near-zero value. Method failed to converge." << std::endl;
                    break;
                }
                
                c = ((a * fb) - (b * fa)) / (fb - fa);
                y_x = func.evaluate(c);
                
                sleep_ms(200);
                std::cout << std::endl << count << ") Iteration: " << std::endl;
                std::cout << "x" << count << " = " << std::fixed << std::setprecision(4) << c 
                          << "\t f(x" << count << ") = " << std::fixed << std::setprecision(4) << y_x << std::endl;

                // Updating the values of x:
                a = b;
                b = c;
                
            } catch (const std::exception& e) {
                std::cout << "Error during computation: " << e.what() << std::endl;
                break;
            }
        } 

        std::cout << std::endl << "Number of Iterations = " << count << std::endl;
        if (count >= max_iterations) {
            std::cout << "Maximum iterations reached, solution may not be accurate." << std::endl;
        }
        std::cout << "By Secant Method, x" << count << " = " << std::fixed << std::setprecision(4) << c << " (approximately)";
    }
};

class NewtonRaphson {
    double x1, x2;
    double y_x = std::numeric_limits<double>::max();
    FunctionManager& func;
    double tolerance;
    int max_iterations;

public: 
    NewtonRaphson(FunctionManager& f, double tol = 0.0001, int max_iter = 100) 
        : func(f), tolerance(tol), max_iterations(max_iter) {
        
        sleep_ms(500);
        std::cout << std::endl << "Enter the first approximate value: ";
        std::cin >> x1;

        int count = 0;
        while (std::abs(y_x) >= tolerance && count < max_iterations) {
            count++;
            try {
                double fx1 = func.evaluate(x1);
                double dfx1 = func.evaluateDerivative(x1);
                
                if (std::abs(dfx1) < 1e-10) {
                    std::cout << "Derivative near zero. Method failed to converge." << std::endl;
                    break;
                }
                
                // Formula of Newton Raphson method
                x2 = x1 - (fx1 / dfx1);

                sleep_ms(200);
                std::cout << std::endl << count << ") Iteration: " << "\t";
                std::cout << "x" << count-1 << " = " << std::fixed << std::setprecision(4) << x1 
                          << "\t f(x" << count-1 << ") = " << std::fixed << std::setprecision(4) << fx1 
                          << "\t f'(x" << count-1 << ") = " << std::fixed << std::setprecision(4) << dfx1 << std::endl;
                std::cout << "x" << count << " = " << std::fixed << std::setprecision(4) << x2 << std::endl;

                // Updating values
                x1 = x2;
                y_x = func.evaluate(x1);
                
            } catch (const std::exception& e) {
                std::cout << "Error during computation: " << e.what() << std::endl;
                break;
            }
        }

        // Printing the Final Answer
        std::cout << std::endl << "Number of Iterations = " << count;
        if (count >= max_iterations) {
            std::cout << " (Maximum iterations reached, solution may not be accurate)" << std::endl;
        }
        std::cout << std::endl << "Answer is " << std::fixed << std::setprecision(4) << x2;
    }
};

class MullersMethod {
    double x3, x2, x1, x0, y3, y2, y1, y0, A, B;
    double f_x3 = std::numeric_limits<double>::max();
    FunctionManager& func;
    double tolerance;
    int max_iterations;

public: 
    MullersMethod(FunctionManager& f, double tol = 0.0001, int max_iter = 100) 
        : func(f), tolerance(tol), max_iterations(max_iter) {
        
        // Taking the Initial Approximate values
        sleep_ms(500);
        std::cout << std::endl << "Enter the initial approximations: " << std::endl;
        std::cout << "x(i-2) = ";
        std::cin >> x2;
        std::cout << "x(i-1) = ";
        std::cin >> x1;
        std::cout << "x(i) = ";
        std::cin >> x0;
        std::cout << std::endl << std::endl;

        // Muller's Method
        int count = 0;
        while (std::abs(f_x3) >= tolerance && count < max_iterations) {
            count++;
            try {
                y2 = func.evaluate(x2);
                y1 = func.evaluate(x1);
                y0 = func.evaluate(x0);

                sleep_ms(200);
                std::cout << count << ") Iteration: " << std::endl;
                std::cout << "x(i-2) = " << std::fixed << std::setprecision(4) << x2 
                          << "\t x(i-1) = " << std::fixed << std::setprecision(4) << x1 
                          << "\t x(i) = " << std::fixed << std::setprecision(4) << x0 << std::endl;
                std::cout << "y(i-2) = " << std::fixed << std::setprecision(4) << y2 
                          << "\t y(i-1) = " << std::fixed << std::setprecision(4) << y1 
                          << "\t y(i) = " << std::fixed << std::setprecision(4) << y0 << std::endl << std::endl;

                double t1 = (y1 - y0);
                double t2 = (x1 - x2)*(x1 - x0);
                double t3 = (y2 - y0);
                double t4 = (x2 - x1)*(x2 - x0);

                A = ((t1/t2) + (t3/t4));
                B = (t1 / (x1 - x0)) - (A * (x1 - x0));
                
                double discriminant = B*B - 4*A*y0;
                if (discriminant < 0) {
                    std::cout << "Complex roots encountered. Using absolute value of discriminant." << std::endl;
                    discriminant = std::abs(discriminant);
                }

                // Choose the denominator that gives the smaller absolute value
                double denominator1 = B + std::sqrt(discriminant);
                double denominator2 = B - std::sqrt(discriminant);
                
                if (std::abs(denominator1) >= std::abs(denominator2) && std::abs(denominator2) > 1e-10) {
                    x3 = x0 - ((2*y0) / denominator2);
                } else if (std::abs(denominator1) > 1e-10) {
                    x3 = x0 - ((2*y0) / denominator1);
                } else {
                    std::cout << "Division by near-zero value. Method failed to converge." << std::endl;
                    break;
                }
                
                f_x3 = func.evaluate(x3);

                sleep_ms(200);
                std::cout << "A = " << std::fixed << std::setprecision(4) << A 
                          << "\t B = " << std::fixed << std::setprecision(4) << B 
                          << " \t x(i+1) = " << std::fixed << std::setprecision(4) << x3 << std::endl << std::endl;
                
                // Updating the values
                x2 = x1;
                x1 = x0;
                x0 = x3;
                
            } catch (const std::exception& e) {
                std::cout << "Error during computation: " << e.what() << std::endl;
                break;
            }
        }

        sleep_ms(500);
        std::cout << std::endl << "By Muller's Method," << std::endl;
        if (count >= max_iterations) {
            std::cout << "Maximum iterations reached, solution may not be accurate." << std::endl;
        }
        std::cout << "x(i+1) = " << std::fixed << std::setprecision(4) << x3 << " (approximately)";
    }
};

// Display a stylish ASCII art banner
void displayBanner() {
    std::cout << "\n";
    std::cout << " ╔════════════════════════════════════════════════════════════════╗\n";
    std::cout << " ║                                                                ║\n";
    std::cout << " ║  ███╗   ██╗██╗   ██╗███╗   ███╗███████╗██████╗ ██╗ ██████╗    ║\n";
    std::cout << " ║  ████╗  ██║██║   ██║████╗ ████║██╔════╝██╔══██╗██║██╔════╝    ║\n";
    std::cout << " ║  ██╔██╗ ██║██║   ██║██╔████╔██║█████╗  ██████╔╝██║██║         ║\n";
    std::cout << " ║  ██║╚██╗██║██║   ██║██║╚██╔╝██║██╔══╝  ██╔══██╗██║██║         ║\n";
    std::cout << " ║  ██║ ╚████║╚██████╔╝██║ ╚═╝ ██║███████╗██║  ██║██║╚██████╗    ║\n";
    std::cout << " ║  ╚═╝  ╚═══╝ ╚═════╝ ╚═╝     ╚═╝╚══════╝╚═╝  ╚═╝╚═╝ ╚═════╝    ║\n";
    std::cout << " ║                                                                ║\n";
    std::cout << " ║              METHODS CALCULATOR                               ║\n";
    std::cout << " ║                                                                ║\n";
    std::cout << " ║             Author: Prathamesh Khade                          ║\n";
    std::cout << " ║                                                                ║\n";
    std::cout << " ╚════════════════════════════════════════════════════════════════╝\n\n";
}

// Main program
int main() {
    FunctionManager functionManager;
    double tolerance = 0.0001;
    int max_iterations = 100;
    
    // Display banner with author name
    displayBanner();
    
    // Function input method selection
    int input_method;
    std::cout << "Select how to input the function:" << std::endl;
    std::cout << "1. Enter function expression directly" << std::endl;
    std::cout << "2. Load function from a file" << std::endl;
    std::cout << "Choose: ";
    std::cin >> input_method;
    
    // Clear input buffer
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    
    bool function_loaded = false;
    
    switch (input_method) {
        case 1: {
            std::string expression;
            std::cout << std::endl << "Enter the function f(x) expression:" << std::endl;
            std::cout << "Examples: x^2-4, sin(x)+cos(x), log(x)-1.2" << std::endl;
            std::cout << "Supported functions: sin, cos, tan, exp, log, log10, sqrt, abs" << std::endl;
            std::cout << "f(x) = ";
            std::getline(std::cin, expression);
            
            function_loaded = functionManager.setExpression(expression);
            if (!function_loaded) {
                std::cout << "Failed to parse the function. Please check the syntax and try again." << std::endl;
                return 1;
            }
            break;
        }
        case 2: {
            std::string filename;
            std::cout << std::endl << "Enter the path to the function file:" << std::endl;
            std::cout << "(The file should contain a single line with the function expression)" << std::endl;
            std::cout << "File path: ";
            std::getline(std::cin, filename);
            
            function_loaded = functionManager.loadFromFile(filename);
            if (!function_loaded) {
                std::cout << "Failed to load function from file. Please check the file path and syntax." << std::endl;
                return 1;
            }
            break;
        }
        default:
            std::cout << "Invalid option selected." << std::endl;
            return 1;
    }
    
    std::cout << std::endl << "Function loaded: f(x) = " << functionManager.getExpression() << std::endl;
    
    // Algorithm parameters
    std::cout << std::endl << "Set algorithm parameters (or use defaults):" << std::endl;
    std::cout << "Enter tolerance [default = 0.0001]: ";
    std::string tol_input;
    std::getline(std::cin, tol_input);
    if (!tol_input.empty()) {
        tolerance = std::stod(tol_input);
    }
    
    std::cout << "Enter maximum iterations [default = 100]: ";
    std::string iter_input;
    std::getline(std::cin, iter_input);
    if (!iter_input.empty()) {
        max_iterations = std::stoi(iter_input);
    }
    
    // Method selection
    int option;
    std::cout << std::endl << "Which method do you want to perform?" << std::endl;
    std::cout << "1. Regula Falsi Method" << std::endl;
    std::cout << "2. Secant Method" << std::endl;
    std::cout << "3. Newton Raphson Method" << std::endl;
    std::cout << "4. Muller's Method" << std::endl;
    std::cout << "Choose: ";
    std::cin >> option;

    try {
        switch (option) {
            case 1: {   
                RegulaFalsi r(functionManager, tolerance, max_iterations);
                break;
            }
            
            case 2: {
                SecantMethod s(functionManager, tolerance, max_iterations);
                break;
            }

            case 3: {   
                NewtonRaphson nr(functionManager, tolerance, max_iterations);
                break;
            }
            
            case 4: {
                MullersMethod m(functionManager, tolerance, max_iterations);
                break;
            }
            
            default: {
                std::cout << std::endl << "Choose correct option!!!";
                break;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Error: " << e.what() << std::endl;
    }
    
    std::cout << std::endl << std::endl << "Press any key to exit...";
    getch();
    return 0;
}