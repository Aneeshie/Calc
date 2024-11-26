#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

struct Stack {
    char **items;  // Array of strings instead of chars
    int top;
    int maxSize;
};

struct DoubleStack {
    int top;
    int size;
    double *S;
};

void createDoubleStack(struct DoubleStack *st, int size) {
    st->top = -1;
    st->size = size;
    st->S = (double *)malloc(size * sizeof(double));
}

void pushDouble(struct DoubleStack *st, double value) {
    st->S[++(st->top)] = value;
}

double popDouble(struct DoubleStack *st) {
    if (st->top >= 0) {
        return st->S[(st->top)--];
    }
    return 0.0;
}

void createStack(struct Stack *st, int size) {
    st->items = (char**)malloc(size * sizeof(char*));
    st->top = -1;
    st->maxSize = size;
}

void clearStack(struct Stack *st) {
    while (st->top >= 0) {
        free(st->items[st->top--]);
    }
}

void push(struct Stack *st, const char *str) {
    if (st->top < st->maxSize - 1) {
        st->items[++(st->top)] = strdup(str);
    }
}

char* pop(struct Stack *st) {
    if (st->top >= 0) {
        return st->items[st->top--];
    }
    return NULL;
}

int isEmpty(struct Stack *st) {
    return st->top == -1;
}

int precedence(const char *op) {
    if (strlen(op) == 1) {
        switch (op[0]) {
            case '+':
            case '-': return 1;
            case '*':
	    case '%':
            case '/': return 2;
            case '^': return 3;
		     
        }
    }
    return 0;
}

int isFunction(const char *exp, int i) {
    const char *funcs[] = {"sin", "cos", "tan", "log"};
    for (int j = 0; j < 4; j++) {
        if (strncmp(exp + i, funcs[j], 3) == 0) {
            return 1;
        }
    }
    return 0;
}

char* convertToPostFix(const char *exp) {
    int len = strlen(exp);
    struct Stack stack;
    createStack(&stack, len);

    // Allocate enough memory to hold postfix expression
    char *postfix = (char*)malloc(len * 4 * sizeof(char));  // Extra space for output
    char buffer[32];  // For temporary number storage
    int j = 0;  // Output index
    memset(postfix, 0, len * 4 * sizeof(char));  // Clear previous contents

    for (int i = 0; i < len; i++) {
        if (isspace(exp[i])) continue;

        // Handle numbers
        if (isdigit(exp[i]) || exp[i] == '.' || 
            (exp[i] == '-' && (i == 0 || exp[i-1] == '(' || strchr("+-*/^", exp[i-1])))) {
            int k = 0;
            while (i < len && (isdigit(exp[i]) || exp[i] == '.' || 
                   (k == 0 && exp[i] == '-'))) {
                buffer[k++] = exp[i++];
            }
            i--;  // Back up one character
            buffer[k] = '\0';
            strcat(postfix, buffer);
            strcat(postfix, " ");
            j = strlen(postfix);
        }
        // Handle functions
        else if (isFunction(exp, i)) {
            char func[4] = {0};
            strncpy(func, exp + i, 3);
            push(&stack, func);
            i += 2;  // Skip rest of function name
        }
        // Handle parentheses
        else if (exp[i] == '(') {
            buffer[0] = '(';
            buffer[1] = '\0';
            push(&stack, buffer);
        }
        else if (exp[i] == ')') {
            while (!isEmpty(&stack)) {
                char *top = pop(&stack);
                if (strcmp(top, "(") == 0) {
                    free(top);
                    break;
                }
                strcat(postfix, top);
                strcat(postfix, " ");
                free(top);
            }
            // If there's a function waiting, output it now
            if (!isEmpty(&stack)) {
                char *top = stack.items[stack.top];
                if (isFunction(top, 0)) {
                    top = pop(&stack);
                    strcat(postfix, top);
                    strcat(postfix, " ");
                    free(top);
                }
            }
        }
        // Handle operators
        else if (strchr("+-*/^%", exp[i])) {
            buffer[0] = exp[i];
            buffer[1] = '\0';
            while (!isEmpty(&stack) && 
                   strcmp(stack.items[stack.top], "(") != 0 && 
                   precedence(stack.items[stack.top]) >= precedence(buffer)) {
                char *op = pop(&stack);
                strcat(postfix, op);
                strcat(postfix, " ");
                free(op);
            }
            push(&stack, buffer);
        }
    }

    // Pop remaining operators
    while (!isEmpty(&stack)) {
        char *op = pop(&stack);
        if (strcmp(op, "(") != 0) {  // Ignore any remaining parentheses
            strcat(postfix, op);
            strcat(postfix, " ");
        }
        free(op);
    }

    // Remove trailing space if exists
    int lastIdx = strlen(postfix) - 1;
    if (lastIdx >= 0 && postfix[lastIdx] == ' ') {
        postfix[lastIdx] = '\0';
    }

    clearStack(&stack);  // Clear stack before returning
    return postfix;
}

double evaluatePostfix(char *postfix) {
    struct DoubleStack st;
    createDoubleStack(&st, strlen(postfix));

    char *token = strtok(postfix, " ");
    while (token != NULL) {
        // Handle numbers (including negative numbers)
        if (isdigit(token[0]) || token[0] == '.' || 
            (token[0] == '-' && (isdigit(token[1]) || token[1] == '.'))) {
            pushDouble(&st, atof(token));
        } 
        // Handle functions
        else if (strncmp(token, "sin", 3) == 0) {
            double arg = popDouble(&st);
            pushDouble(&st, sin(arg));
        }
        else if (strncmp(token, "cos", 3) == 0) {
            double arg = popDouble(&st);
            pushDouble(&st, cos(arg));
        }
        else if (strncmp(token, "tan", 3) == 0) {
            double arg = popDouble(&st);
            pushDouble(&st, tan(arg));
        }
        else if (strncmp(token, "log", 3) == 0) {
            double arg = popDouble(&st);
            pushDouble(&st, log(arg));
        }
        // Handle operators
        else if (strchr("+-*/^%", token[0])) {
            double b = popDouble(&st);
            double a = popDouble(&st);
            switch (token[0]) {
                case '+': pushDouble(&st, a + b); break;
                case '-': pushDouble(&st, a - b); break;
                case '*': pushDouble(&st, a * b); break;
                case '/': pushDouble(&st, a / b); break;
                case '^': pushDouble(&st, pow(a, b)); break;
		case '%': pushDouble(&st, fmod(a,b)); break;
            }
        }
        token = strtok(NULL, " ");
    }

    double result = popDouble(&st);
    free(st.S);  // Free the stack memory
    return result;
}

int main() {
    char expression[256];

    while (1) {
        printf("Enter an expression (or 'exit' to quit): ");
        
        if (fgets(expression, sizeof(expression), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            break;
        }
        
        // Remove newline character
        expression[strcspn(expression, "\n")] = 0;
        
        // Check for exit condition
        if (strcmp(expression, "exit") == 0) {
            printf("Exiting calculator...\n");
            break;
        }
        
        // Skip empty input
        if (strlen(expression) == 0) {
            continue;
        }
        
        char *postfix = convertToPostFix(expression);
        if (postfix) {
            printf("Postfix: %s\n", postfix);
            double result = evaluatePostfix(postfix);
            printf("Result: %.2lf\n", result);
            free(postfix);
        } else {
            fprintf(stderr, "Error converting expression to postfix\n");
        }

        printf("\n");  // Add a blank line for readability
    }

    return 0;
}
