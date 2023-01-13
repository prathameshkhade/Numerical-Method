#include <bits/stdc++.h>
#include <conio.h>

#define e 2.718281
#define pi 3.141592

using namespace std;

inline float f(float x) {
    return (x*log10(x) - 1.2);       // The equation will be written here
}

inline float g(float x) {
    return (1+log10(x));      // This is derivative of f(x)...... In case of Newton Raphson Method
}

class RegulaFalsi {
    float a, b, c;
    float y_c = INT_MAX;

    public: 
        RegulaFalsi() {
            // Checking where the roots are lies between...
            for (int i=0; i<=10; i++) {
                if(f(i) < 0 && f(i+1) >= 0.0000) {
                    a = i;
                    b = i + 1;
                }
            }

            _sleep(1000);
            cout << endl << "Roots are lies between (" << a << ", " << b << ")" << endl << endl;

            int count = 0;
            while (abs(y_c) >= 0.0001) {
                count++;
                c = ((a * f(b)) - (b * f(a))) / (f(b) - f(a));
                y_c = f(c);
                _sleep(545);
                cout << endl << count << ") Iteration: " << endl;
                cout << "c = " << c << "\t f(c) = " << y_c << endl;

                if(y_c >= 0.0001) {
                    b = c;
                } else if(y_c <= -0.0001) {
                    a = c;
                }
            } 

            cout << endl << "Number of Iterations = " << count << endl;
            cout << "By Regula Falsi Method, c = " << c << " (aprroximately)";
        }
};

class SecantMethod {
    float a, b, c, y_x = INT_MAX;

    public: 
        SecantMethod() {
            // Checking where the roots are lies between.......
            for (int i=0; i<=10; i++) {
                if(f(i) < 0 && f(i+1) >= 0.0001) {
                    a = i;
                    b = i + 1;
                } else if (f(i) >= 0.0000 && f(i+1) < 0) {
                    a = i;
                    b = i + 1;
                }
            }

            _sleep(1000);
            cout << endl << "Roots are lies between (" << a << ", " << b << ")" << endl << endl;

            // Secant Method.......
            int count = 0;
            while (abs(y_x) >= 0.0001) {
                count++;
                // float numerator = (a*f(b) - b*f(a));
                // float denominator = f(b) - f(a);
                // c = numerator / denominator;
                c = ((a * f(b)) - (b * f(a))) / (f(b) - f(a));
                y_x = f(c);
                _sleep(545);
                cout << endl << count << ") Iteration: " << endl;
                cout << "x" << count << " = " << c << "\t f(" << count << ") = " << y_x << endl;

                // Updating the values of x:
                a = b;
                b = c;
            } 

            cout << endl << "Number of Iterations = " << count << endl;
            cout << "By Secant Method, x" << count << " = " << c << " (aprroximately)";
        }
};

class NewtonRaphson {
    float x1, x2;
    float y_x = INT_MAX;

    public: 
        NewtonRaphson() {
            _sleep(1000);
            cout << endl << "Enter the first approximate value: ";
            cin >> x1;

            int count = 0;
            while (abs(y_x) >= 0.0001) {
                count++;

                // Formula of Newton Raphoson method.......
                x2 = ( x1 - (f(x1) / g(x1)) );

                _sleep(545);
                cout << endl << count << ") Iteration: " << "\t";
                cout << "x" << count-1 << " = " << x1 << "\t f(x" << count-1 << ") = " << f(x1) << "\t f'(x" << count-1 << ") = " << g(x1) << endl;
                cout << "x" << count << " = " << x2 << endl;

                // Updating values.......
                x1 = x2;
                y_x = f(x1);
            }

            // Printing the Final Answer.......
            cout << endl << "Number of Iterations = " << count;
            cout << endl << "Answer is " << x2;
        }
};

class MullersMethod {
    float x3, x2, x1, x0, y3, y2, y1, y0, A, B;
    float f_x3 = INT_MAX;

    public: 
        MullersMethod() {
        // Taking the Initial Approximate values.......
        _sleep(1054);
        cout << endl << "Enter the initial approximations: " << endl;
        cout << "x(i-2) = " ;
        cin >> x2;
        cout << "x(i-1) = " ;
        cin >> x1;
        cout << "x(i) = " ;
        cin >> x0;
        cout << endl << endl;

        // Muller's Method.........
        int count = 0;
        while (abs(f_x3) >= 0.0001) {
            count++;
            y2 = f(x2);
            y1 = f(x1);
            y0 = f(x0);

            _sleep(545);
            cout << count << ") Iteration: " << endl;
            cout << "x(i-2) = " << x2 << "\t x(i-1) = " << x1 << "\t x(i) = " << x0 << endl;
            cout << "y(i-2) = " << y2 << "\t y(i-1) = " << y1 << "\t y(i) = " << y0 << endl << endl;

            float t1 = (y1 - y0);
            float t2 = (x1 - x2)*(x1 - x0);
            float t3 = (y2 - y0);
            float t4 = (x2 - x1)*(x2 - x0);

            A = ((t1/t2) + (t3/t4));
            B = (t1 / (x1 - x0)) - (A * (x1 - x0));

            x3 = x0 - ( (2*y0) / (B + sqrt(B*B - 4*A*y0)) );  // Formula for the next iteration
            f_x3 = f(x3);

            _sleep(545);
            cout << "A = " << A << "\t B = " << B << " \t x(i+1) = " << x3 << endl << endl;
            
            // Updatating the values........
            x2 = x1;
            x1 = x0;
            x0 = x3;
        }

        _sleep(1054);
        cout << endl << "By Muller's Method," << endl << " x(i+1) = " << x3 << " (approximately)";
    }
};

int main() 
{
    int option;
    cout << endl << "Which method you want to perform....." << endl;
    cout << "1. Regula Falsi Method" << endl;
    cout << "2. Secant Method " << endl;
    cout << "3. Newton Raphson Method " << endl;
    cout << "4. Muller's Method " << endl;
    cout << "choose: ";
    cin >> option;

    switch (option) {
        case 1:  {   
            RegulaFalsi r;
            break;
        }
        
        case 2: {
            SecantMethod s;
            break;
        }

        case 3:  {   
            NewtonRaphson nr;
            break;
        }
        
        case 4: {
            MullersMethod m;
            break;
        }
        
        default:    {
            cout << endl << "Choose correct option!!!";
            break;
        }
    }
    
    getch();
    return 0;
}
