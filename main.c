//
// Copyright (c) 2023 Brian Sullender
// All rights reserved.
//
// This source code is licensed under the terms provided in the README file.
//
// https://github.com/b-sullender/expression-parser
//

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

// Define token types
typedef enum {
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    REMAINDER,
    BIT_AND,
    BIT_OR,
    BIT_XOR,
    BIT_LSHIFT,
    BIT_RSHIFT,
    LPAREN,
    RPAREN,
    END
} TokenType;

// Define token structure
typedef struct {
    TokenType type;
    int precedence;
    int value;
} Token;

// Define parser context
typedef struct {
    Token token;
    char* input;
} ParserContext;

// Function to get the next token
void getNextToken(ParserContext* context)
{
    char c = *context->input++;
    while (c == ' ' || c == '\r' || c == '\n' || c == '\t') {
        c = *context->input++;
    }

    if (c == '\0') {
        context->token.type = END;
        context->token.precedence = 0;
        context->input--;
        return;
    }

    if (c >= '0' && c <= '9') {
        context->token.type = NUMBER;
        context->token.precedence = 0;
        context->token.value = c - '0';
        c = *context->input++;
        while (c >= '0' && c <= '9') {
            context->token.value = context->token.value * 10 + (c - '0');
            c = *context->input++;
        }
        context->input--;
    } else if (c == '+') {
        context->token.type = PLUS;
        context->token.precedence = 700;
    } else if (c == '-') {
        context->token.type = MINUS;
        context->token.precedence = 700;
    } else if (c == '*') {
        context->token.type = MULTIPLY;
        context->token.precedence = 800;
    } else if (c == '/') {
        context->token.type = DIVIDE;
        context->token.precedence = 800;
    } else if (c == '%') {
        context->token.type = REMAINDER;
        context->token.precedence = 800;
    } else if (c == '&') {
        context->token.type = BIT_AND;
        context->token.precedence = 500;
    } else if (c == '|') {
        context->token.type = BIT_OR;
        context->token.precedence = 300;
    } else if (c == '^') {
        context->token.type = BIT_XOR;
        context->token.precedence = 400;
    } else if (c == '<' && *(context->input) == '<') {
        context->token.type = BIT_LSHIFT;
        context->token.precedence = 600;
        context->input++;
    } else if (c == '>' && *(context->input) == '>') {
        context->token.type = BIT_RSHIFT;
        context->token.precedence = 600;
        context->input++;
    } else if (c == '(') {
        context->token.type = LPAREN;
        context->token.precedence = 900;
    } else if (c == ')') {
        context->token.type = RPAREN;
        context->token.precedence = 900;
    }
}

// Forward declaration
int parseExpression(ParserContext* context, int minPrecedence);

// Function to parse a factor
int parseFactor(ParserContext* context)
{
    int value;

    if (context->token.type == NUMBER) {
        value = context->token.value;
        getNextToken(context);
    } else if (context->token.type == LPAREN) {
        getNextToken(context);
        value = parseExpression(context, context->token.precedence);
        if (context->token.type != RPAREN) {
            fprintf(stderr, "Error: Missing closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
        getNextToken(context);
    } else {
        fprintf(stderr, "Error: Unexpected token\n");
        exit(EXIT_FAILURE);
    }

    return value;
}

// Function to parse an expression
int parseExpression(ParserContext* context, int minPrecedence)
{
    // Get the factor
    int value = parseFactor(context);

    // Check that we can continue processing this term
    while ((context->token.type != END) && (context->token.type != RPAREN) && (context->token.precedence > minPrecedence))
    {
        // Check if we have a valid operator token
        if (context->token.type == NUMBER) {
            fprintf(stderr, "%s\n", "Error: expected operator, not variable");
            exit(EXIT_FAILURE);
        }

        // Save current token
        Token token = context->token;

        // Get the factor/term to the right
        getNextToken(context);
        int term = parseExpression(context, token.precedence);

        // Perform the operation
        if (token.type == PLUS) {
            value += term;
        } else if (token.type == MINUS) {
            value -= term;
        } else if (token.type == MULTIPLY) {
            value *= term;
        } else if (token.type == DIVIDE) {
            value /= term;
        } else if (token.type == REMAINDER) {
            value %= term;
        } else if (token.type == BIT_LSHIFT) {
            value <<= term;
        } else if (token.type == BIT_RSHIFT) {
            value >>= term;
        } else if (token.type == BIT_AND) {
            value &= term;
        } else if (token.type == BIT_XOR) {
            value ^= term;
        } else if (token.type == BIT_OR) {
            value |= term;
        }
    }

    // Return the result of the term
    return value;
}

int main()
{
    char expre[256];

    printf("%s\n", "Enter 'q' to exit");
    while (true)
    {
        printf("Enter expression: ");
        fflush(stdout);
        if (fgets(expre, 255, stdin) != expre) {
            perror("Error reading expression\n");
            return EXIT_FAILURE;
        }
        if (strcmp(expre, "q\n") == 0) {
            return EXIT_SUCCESS;
        }

        char* pInput = expre;
        ParserContext context;
        context.input = pInput;

        getNextToken(&context);
        int result = parseExpression(&context, context.token.precedence);
        if (context.token.type != END) {
            fprintf(stderr, "%s\n", "Error: Unexpected token\n");
            continue;
        }
        printf("Result: %d\n", result);
    }

    return EXIT_SUCCESS;
}

