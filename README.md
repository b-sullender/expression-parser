# Expression-Parser

Expression Parser is a versatile C-based parser designed for efficiency and speed, making it suitable for a wide range of applications. It supports various operations, both arithmetic and bitwise, and can be extended to include additional features.

## Supported Operations

The Expression Parser currently supports the following operations:

### Arithmetic Operations

- Addition `+`
- Subtraction `-`
- Multiplication `*`
- Division `/`
- Modulo `%`

### Bitwise Operations

- Left Shift `<<`
- Right Shift `>>`
- Bitwise AND `&`
- Bitwise OR `|`
- Bitwise XOR `^`

You can use parentheses to control the order of operations and ensure precise calculations.

LICENSE TERMS
=============
```
This software is provided 'as-is', without any express or implied
warranty.  In no event will the authors be held liable for any damages
arising from the use of this software.

Permission is granted to anyone to use this software for any purpose,
including commercial applications, and to alter it and redistribute it
freely, subject to the following restrictions:

(1) If any part of the source code or algorithm is used, it is requested that
    appropriate attribution is given to the authors by providing a link to
    the original repository or a reference to the authors name.
(2) Permission for use of this software is granted only if the user accepts
    full responsibility for any undesirable consequences; the authors accept
    NO LIABILITY for damages of any kind.

By using this software, you acknowledge that you have read and understood
these terms and agree to abide by them.
```

To compile and run the expression parser, follow these steps:

1. Clone the repository to your local machine:
   ```shell
   git clone https://github.com/b-sullender/expression-parser.git
   ```

2. Navigate to the project directory:
   ```shell
   cd expression-parser
   ```

3. Compile the application using GCC:
   ```shell
   gcc main.c -o exp
   ```

4. Run the program:
   ```shell
   ./exp
   ```

## Extending the Parser

One of the key strengths of the Expression Parser is its extensibility. You can easily add support for more operations by following these guidelines:

1. **Define the Operation**: Add the name of the new operation to the `TokenType` enum. This enum defines the types of tokens that the parser can recognize.

2. **Token Handling**: In the `getNextToken` function, add an `else if` handler for your new token type and set the appropriate precedence. This step ensures that the parser recognizes the new token correctly.

3. **Operation Implementation**: In the `parseExpression` function, add an `else if` handler for your new token type. Here, you'll implement the actual operation associated with the new token type.

Feel free to experiment and expand the capabilities of the Expression Parser to meet your specific needs or contribute to its development.

