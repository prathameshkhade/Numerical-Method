# Numerical Methods Calculator

A powerful tool for solving mathematical equations using various numerical methods. This program allows you to find roots of equations without manually performing complex calculations.

## Features

- **Interactive Equation Input**: Enter your mathematical equation directly or load it from a file
- **Multiple Numerical Methods**:
  - Regula Falsi (False Position) Method
  - Secant Method
  - Newton-Raphson Method
  - Muller's Method
- **Customizable Parameters**: Set your desired tolerance and maximum iterations
- **Cross-Platform Support**: Works on Windows, macOS, and Linux

## Installation

### Download Pre-built Binaries

The easiest way to get started is to download the pre-built binaries from the [Releases](https://github.com/prathameshkhade/Numerical-Method/releases) page.

### Build from Source

1. Clone the repository:
   ```
   git clone https://github.com/prathameshkhade/Numerical-Method.git
   cd Numerical-Method
   ```

2. Build using a C++ compiler:
   ```
   g++ -o NumericalMethods Numerical_Methods.cpp -std=c++11
   ```

3. Run the program:
   ```
   ./NumericalMethods     # On Linux/macOS
   NumericalMethods.exe   # On Windows
   ```

## Usage

1. Start the program
2. Choose how to input your equation:
   - Enter it directly, for example: `x^2 - 4`, `x*log10(x) - 1.2`, or `sin(x) + cos(x)`
   - Load from a text file containing the equation
3. Set the tolerance and maximum iterations (or use defaults)
4. Select the numerical method you want to use
5. Follow the prompts to enter the required initial values
6. View the step-by-step iteration process and final result

### Supported Mathematical Functions

- Basic operations: `+`, `-`, `*`, `/`, `^` (exponentiation)
- Trigonometric: `sin(x)`, `cos(x)`, `tan(x)`
- Logarithmic: `log(x)` or `ln(x)` (natural logarithm), `log10(x)` (base-10 logarithm)
- Others: `exp(x)` (e^x), `sqrt(x)`, `abs(x)`
- Constants: `e` (2.718...), `pi` (3.141...)

### Example Equations

- `x^2 - 4`
- `x*log10(x) - 1.2`
- `sin(x) + cos(x) - 0.5`
- `exp(-x) - x`

## Creating Function Files

To use the file input option, create a text file containing only the equation expression. For example:

**myfunction.txt:**
```
x*log10(x) - 1.2
```

## Methods Overview

### Regula Falsi Method
The Regula Falsi method (also known as the False Position method) is a root-finding algorithm that combines aspects of the bisection method and the secant method. It uses linear interpolation to find improved approximations to the roots of a function.

### Secant Method
The Secant method is a root-finding algorithm that uses a succession of roots of secant lines to better approximate a root of a function. It's similar to the Newton-Raphson method but doesn't require the computation of derivatives.

### Newton-Raphson Method
The Newton-Raphson method is a powerful root-finding algorithm that uses the first derivative of a function to find successively better approximations to the roots of a real-valued function.

### Muller's Method
Muller's method is a root-finding algorithm that extends the secant method to use quadratic interpolation among three points. It can find complex roots and often converges faster than linear methods for certain types of problems.

## Contributing

Contributions are welcome! Please feel free to submit a Pull Request.

## License

This project is licensed under the MIT License - see the LICENSE file for details.