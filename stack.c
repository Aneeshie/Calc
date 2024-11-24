#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// Stack for characters (used for operators and parentheses)
struct CharStack {
    int top;
    int size;
    char *S;
};

// Stack for integers (used for evaluating postfix)
struct IntStack {
    int top;
    int size;
    int *S;
};

// Create CharStack
void createCharStack(struct CharStack *st, int size) {
    st->top = -1;
    st->size = size;
    st->S = (char *)malloc(size * sizeof(char));
}

// Create IntStack
void createIntStack(struct IntStack *st, int size) {
    st->top = -1;
    st->size = size;
    st->S = (int *)malloc(size * sizeof(int));
}

// Push to CharStack
void pushChar(struct CharStack *st, char ch) {
    st->S[++(st->top)] = ch;
}

// Push to IntStack
void pushInt(struct IntStack *st, int value) {
    st->S[++(st->top)] = value;
}

// Pop from CharStack
char popChar(struct CharStack *st) {
    return st->S[(st->top)--];
}

// Pop from IntStack
int popInt(struct IntStack *st) {
    return st->S[(st->top)--];
}

// Check if stack is empty
int isEmptyCharStack(struct CharStack *st) {
    return st->top == -1;
}

// Precedence of operators
int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    return 0;
}

// Check if character is an operator
int isOperator(char ch) {
    return ch == '+' || ch == '-' || ch == '*' || ch == '/';
}

// Convert infix to postfix
char *convertToPostFix(char *exp) {
    int len = strlen(exp);
    struct CharStack st;
    createCharStack(&st, len);
    pushChar(&st, '$');  // Sentinel value

    char *postfix = (char *)malloc(len * 2 * sizeof(char));
    int i = 0, j = 0;

    while (i < len) {
        // Skip spaces
        if (isspace(exp[i])) {
            i++;
            continue;
        }

        if (isdigit(exp[i]) || (exp[i] == '-' && (i == 0 || exp[i - 1] == '('))) {
            // Handle numbers and negative numbers
            if (exp[i] == '-') {
                postfix[j++] = exp[i++];
            }
            while (isdigit(exp[i])) {
                postfix[j++] = exp[i++];
            }
            postfix[j++] = ' '; // Separate numbers
        } else if (exp[i] == '(') {
            pushChar(&st, exp[i++]);
        } else if (exp[i] == ')') {
            while (!isEmptyCharStack(&st) && st.S[st.top] != '(') {
                postfix[j++] = popChar(&st);
                postfix[j++] = ' ';
            }
            popChar(&st); // Pop '('
            i++;
        } else if (isOperator(exp[i])) {
            while (!isEmptyCharStack(&st) && precedence(st.S[st.top]) >= precedence(exp[i])) {
                postfix[j++] = popChar(&st);
                postfix[j++] = ' ';
            }
            pushChar(&st, exp[i++]);
        }
    }

    // Pop remaining operators
    while (!isEmptyCharStack(&st) && st.S[st.top] != '$') {
        postfix[j++] = popChar(&st);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0';

    return postfix;
}

// Evaluate postfix expression
int evaluatePostfix(char *postfix) {
    struct IntStack st;
    createIntStack(&st, strlen(postfix));

    char *token = strtok(postfix, " ");
    while (token != NULL) {
        if (isdigit(token[0]) || (token[0] == '-' && isdigit(token[1]))) {
            pushInt(&st, atoi(token));
        } else {
            int b = popInt(&st);
            int a = popInt(&st);
            switch (token[0]) {
                case '+': pushInt(&st, a + b); break;
                case '-': pushInt(&st, a - b); break;
                case '*': pushInt(&st, a * b); break;
                case '/': pushInt(&st, a / b); break;
            }
        }
        token = strtok(NULL, " ");
    }

    return popInt(&st);
}

// Main function
int main() {
    char infix[] = "(-35 + 9) * 2";
    char *postfix = convertToPostFix(infix);

    printf("Postfix: %s\n", postfix);
    printf("Result: %d\n", evaluatePostfix(postfix));

    free(postfix);
    return 0;
}
