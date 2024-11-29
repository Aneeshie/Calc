#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include <math.h>

struct Stack {
    char **items;  
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

    char *postfix = (char*)malloc(len * 4 * sizeof(char));
    char buffer[32];
    int j = 0;
    memset(postfix, 0, len * 4 * sizeof(char));

    for (int i = 0; i < len; i++) {
        if (isspace(exp[i])) continue;

        // Handle numbers,
        if (isdigit(exp[i]) || exp[i] == '.' || 
            (exp[i] == '-' && (i == 0 || exp[i-1] == '(' || strchr("+-*/^", exp[i-1])) && 
             (i + 1 < len && isdigit(exp[i + 1])))) { 
            int k = 0;
            while (i < len && (isdigit(exp[i]) || exp[i] == '.' || 
                   (k == 0 && exp[i] == '-'))) {
                buffer[k++] = exp[i++];
            }
            i--;  
            buffer[k] = '\0';
            strcat(postfix, buffer);
            strcat(postfix, " ");
            j = strlen(postfix);
        }
        // Handle functions (e.g., sin, cos)
        else if (isFunction(exp, i)) {
            char func[4] = {0};
            strncpy(func, exp + i, 3);
            push(&stack, func);
            i += 2;
        }
        // Handle left parenthesis
        else if (exp[i] == '(') {
            buffer[0] = '(';
            buffer[1] = '\0';
            push(&stack, buffer);
        }
        // Handle right parenthesis
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

    // Empty remaining stack
    while (!isEmpty(&stack)) {
        char *op = pop(&stack);
        if (strcmp(op, "(") != 0) {  
            strcat(postfix, op);
            strcat(postfix, " ");
        }
        free(op);
    }

    // Remove  space
    int lastIdx = strlen(postfix) - 1;
    if (lastIdx >= 0 && postfix[lastIdx] == ' ') {
        postfix[lastIdx] = '\0';
    }

    clearStack(&stack);
    return postfix;
}


int isValidDecimalSequence(const char *exp) {
    int decimalCount = 0;
    int i = 0;

    while (exp[i] != '\0') {
        if (isdigit(exp[i])) {
            i++;
            continue;
        }

        if (exp[i] == '.') {
            decimalCount++;
            if (decimalCount > 1) {
                return 0; // More than one decimal
            }
        } else {
            // Reset decimal 
            decimalCount = 0;
        }

        i++;
    }

    return 1;
}


int isValidExpression(const char *exp) {
    //  decimals first
    if (!isValidDecimalSequence(exp)) {
        return 0; 
    }

    int len = strlen(exp);
    int openParens = 0;
    int decimalCount = 0;
    int expectingOperand = 1; 
    int inNumber = 0; 
    int digitCount = 0;
    int lastWasDigit = 0; // Track if the last character was a digit

    char *functions[] = {"sin", "cos", "tan", "log", "sqrt"};
    int numFunctions = 5;

    for (int i = 0; i < len; i++) {
        if (isspace(exp[i])) {
            // Ignore spaces check for invalid consecutive numbers or operators
            if (lastWasDigit && i + 1 < len && isdigit(exp[i + 1])) {
                return 0; //consecutive numbers without operator
            }
            continue;
        }

        // Parenthesis 
        if (exp[i] == '(') {
            openParens++;
            expectingOperand = 1; 
            inNumber = 0; 
            decimalCount = 0; 
            digitCount = 0;
            lastWasDigit = 0;
        } else if (exp[i] == ')') {
            openParens--;
            if (openParens < 0) return 0; 
            expectingOperand = 0; 
            inNumber = 0; 
            decimalCount = 0; 
            digitCount = 0;
            lastWasDigit = 0;
        }
        // Decimal point 
        else if (exp[i] == '.') {
            if (!inNumber || decimalCount > 0) return 0; 
            decimalCount++;
            lastWasDigit = 0;
        }
        // Operator 
        else if (strchr("+-*/^%", exp[i])) {
            if (expectingOperand && exp[i] != '-' && exp[i] != '+') return 0;
            expectingOperand = 1; 
            inNumber = 0; 
            decimalCount = 0;
            digitCount = 0;
            lastWasDigit = 0;
        }
        // Function 
        else if (isalpha(exp[i])) {
            int isFunction = 0;
            for (int j = 0; j < numFunctions; j++) {
                int lenFunc = strlen(functions[j]);
                if (strncmp(&exp[i], functions[j], lenFunc) == 0) {
                    isFunction = 1;
                    i += lenFunc - 1; 
                    break;
                }
            }
            if (!isFunction) return 0; 
            expectingOperand = 1; 
            digitCount = 0;
            lastWasDigit = 0;
        }
        // Digit 
        else if (isdigit(exp[i])) {
            inNumber = 1;
            digitCount++;
            expectingOperand = 0; 
            lastWasDigit = 1;
        } else {
            // If any character 
            return 0; 
        }

        
        if (!isdigit(exp[i]) && inNumber) {
            decimalCount = 0;
            inNumber = 0;
            digitCount = 0;
        }
    }

    return (openParens == 0 && !expectingOperand);
}





double evaluatePostfix(char *postfix) {
    struct DoubleStack st;
    createDoubleStack(&st, strlen(postfix));

    char *token = strtok(postfix, " ");
    while (token != NULL) {
       
        if (isdigit(token[0]) || token[0] == '.' || 
            (token[0] == '-' && (isdigit(token[1]) || token[1] == '.'))) {
            pushDouble(&st, atof(token));
        } 
        
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
            if(arg <= 0){
              printf("Error: log is undefined for non-postive values \n");
              free(st.S);
              return NAN;
            }
            pushDouble(&st, log(arg));
        }
       
        else if (strchr("+-*/^%", token[0])) {
            double b = popDouble(&st);
            double a = popDouble(&st);
            if(token[0] == '/' && b == 0){
              printf("Error: division by zero\n");
              free(st.S);
              return NAN;
            }
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
    free(st.S);  
    return result;

}

int main() {
    char expression[256];

    while (1) {
        printf("=> ");
        
        if (fgets(expression, sizeof(expression), stdin) == NULL) {
            fprintf(stderr, "Error reading input\n");
            break;
        }
        
      
        expression[strcspn(expression, "\n")] = 0;
        
        
        if (strcmp(expression, "exit") == 0) {
            break;
        }
        
        
        if (strlen(expression) == 0) {
            continue;
        }

        if(!isValidExpression(expression)){
          printf("Invalid expression, Please check your input\n");
          continue;
        }
        
        char *postfix = convertToPostFix(expression);
        if (postfix) {
            //printf("Postfix: %s\n", postfix);
            double result = evaluatePostfix(postfix);
            if(isnan(result)) continue;
            printf("Result: %.2lf\n", result);
            free(postfix);
        } else {
            fprintf(stderr, "Error converting expression to postfix\n");
        }

        printf("\n");  
    }

    return 0;
}
