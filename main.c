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

// The hint for the next token, for negative factors
typedef enum {
    NONE,
    OPERATOR,
    FACTOR
} TokenHint;

// Define token structure
typedef struct {
    TokenType type;
    int value;
} Token;

// Define parser context
typedef struct {
    Token token;
    char* input;
} ParserContext;

// Function to get the next token
void getNextToken(ParserContext* context, int* precedence, TokenHint hint)
{
    char c = *context->input++;
    while (c == ' ' || c == '\r' || c == '\n' || c == '\t') {
        c = *context->input++;
    }

    if (c == '\0') {
        context->token.type = END;
        context->input--;
    } else if (c >= '0' && c <= '9') {
        context->token.type = NUMBER;
        context->token.value = c - '0';
        c = *context->input++;
        while (c >= '0' && c <= '9') {
            context->token.value = context->token.value * 10 + (c - '0');
            c = *context->input++;
        }
        context->input--;
    } else if (c == '+') {
        context->token.type = PLUS;
        *precedence = 700;
    } else if (c == '-') {
        if ((hint == FACTOR) && (*context->input >= '0' && *context->input <= '9'))
        {
            c = *context->input++;
            context->token.type = NUMBER;
            context->token.value = c - '0';
            c = *context->input++;
            while (c >= '0' && c <= '9') {
                context->token.value = context->token.value * 10 + (c - '0');
                c = *context->input++;
            }
            context->token.value = -context->token.value;
            context->input--;
        }
        else
        {
            context->token.type = MINUS;
            *precedence = 700;
        }
    } else if (c == '*') {
        context->token.type = MULTIPLY;
        *precedence = 800;
    } else if (c == '/') {
        context->token.type = DIVIDE;
        *precedence = 800;
    } else if (c == '%') {
        context->token.type = REMAINDER;
        *precedence = 800;
    } else if (c == '&') {
        context->token.type = BIT_AND;
        *precedence = 500;
    } else if (c == '|') {
        context->token.type = BIT_OR;
        *precedence = 300;
    } else if (c == '^') {
        context->token.type = BIT_XOR;
        *precedence = 400;
    } else if (c == '<' && *(context->input) == '<') {
        context->token.type = BIT_LSHIFT;
        *precedence = 600;
        context->input++;
    } else if (c == '>' && *(context->input) == '>') {
        context->token.type = BIT_RSHIFT;
        *precedence = 600;
        context->input++;
    } else if (c == '(') {
        context->token.type = LPAREN;
    } else if (c == ')') {
        context->token.type = RPAREN;
    }
}

// Forward declaration
int parseExpression(ParserContext* context, int* precedence, int minPrecedence);

// Function to parse a factor
int parseFactor(ParserContext* context, int* precedence)
{
    int value;

    if (context->token.type == NUMBER) {
        value = context->token.value;
        getNextToken(context, precedence, OPERATOR);
    } else if (context->token.type == LPAREN) {
        // Create a new precedence for operations in parentheses
        int subPrecedence = 0;
        // Parse the expression inside the parentheses
        getNextToken(context, &subPrecedence, FACTOR);
        value = parseExpression(context, &subPrecedence, 0);
        // Check that we have a closing parenthesis
        if (context->token.type != RPAREN) {
            fprintf(stderr, "Error: Missing closing parenthesis\n");
            exit(EXIT_FAILURE);
        }
        getNextToken(context, precedence, OPERATOR);
    } else {
        fprintf(stderr, "Error: Unexpected token\n");
        exit(EXIT_FAILURE);
    }

    return value;
}

// Function to parse an expression
int parseExpression(ParserContext* context, int* precedence, int minPrecedence)
{
    // Get the factor
    int value = parseFactor(context, precedence);

    // Check that we can continue processing this term
    while ((context->token.type != END) && (context->token.type != RPAREN) && (*precedence >= minPrecedence))
    {
        // Check if we have a valid operator token
        if (context->token.type == NUMBER) {
            fprintf(stderr, "%s\n", "Error: expected operator, not variable");
            exit(EXIT_FAILURE);
        }

        // Save current operator token
        Token token = context->token;

        // Get the factor/term to the right
        getNextToken(context, precedence, FACTOR);
        int term = parseExpression(context, precedence, *precedence);

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
        memset(&context, 0, sizeof(ParserContext));
        context.input = pInput;

        int precedence = 0;
        getNextToken(&context, &precedence, FACTOR);
        int result = parseExpression(&context, &precedence, 0);
        if (context.token.type != END) {
            fprintf(stderr, "%s\n", "Error: Unexpected token\n");
            continue;
        }
        printf("Result: %d\n", result);
    }

    return EXIT_SUCCESS;
}
