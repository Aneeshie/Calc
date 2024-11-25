#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#define MAX_EXPR_LENGTH 100

struct CharStack {
    int top;
    int size;
    char *S;
};

struct DoubleStack {
    int top;
    int size;
    double *S;
};

void createCharStack(struct CharStack *st, int size) {
    st->top = -1;
    st->size = size;
    st->S = (char *)malloc(size * sizeof(char));
}

void createDoubleStack(struct DoubleStack *st, int size) {
    st->top = -1;
    st->size = size;
    st->S = (double *)malloc(size * sizeof(double));
}

void pushChar(struct CharStack *st, char ch) {
    st->S[++(st->top)] = ch;
}

void pushDouble(struct DoubleStack *st, double value) {
    st->S[++(st->top)] = value;
}

char popChar(struct CharStack *st) {
    return st->S[(st->top)--];
}

double popDouble(struct DoubleStack *st) {
    return st->S[(st->top)--];
}

int isEmptyCharStack(struct CharStack *st) {
    return st->top == -1;
}

int isFunction(char ch) {
    return ch == 's' || ch == 'c' || ch == 't' || ch == 'l';
}

int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/') return 2;
    if (op == '^') return 3;
    return 0;
}

int checkValidity(char ch) {
   
    if (isdigit(ch) || ch == '.' || ch == '-' ||
        ch == 's' || ch == 'c' || ch == 't' || ch == 'l' || 
        strchr("+-*/^()", ch)) { 
        return 1;  
    }
    return 0;  
}

char *convertToPostFix(char *exp) {
    int len = strlen(exp);
    struct CharStack st;
    createCharStack(&st, len);
    pushChar(&st, '$');  

    char *postfix = (char *)malloc((len * 2 + 1) * sizeof(char));
    int i = 0, j = 0;

    while (i < len) {

        if (isspace(exp[i])) {
            i++;
            continue;
        }

        if(!checkValidity(exp[i])) {
            printf("Invalid character '%c'\n", exp[i]);
            free(postfix);
            return NULL;
        }
        // Numbers (including negative)
        if (isdigit(exp[i]) || exp[i] == '.' || 
            (exp[i] == '-' && (i == 0 || exp[i - 1] == '(' || strchr("+-*/^", exp[i - 1])))) {
            int start = i;
            i++;
            while (i < len && (isdigit(exp[i]) || exp[i] == '.')) {
                i++;
            }
            strncpy(&postfix[j], &exp[start], i - start);
            j += i - start;
            postfix[j++] = ' ';
        } 
        // Functions
        else if (isFunction(exp[i])) {
            pushChar(&st, exp[i++]);
        } 
        // Parentheses
        else if (exp[i] == '(') {
            pushChar(&st, exp[i++]);
        } 
        else if (exp[i] == ')') {
            while (!isEmptyCharStack(&st) && st.S[st.top] != '(') {
                postfix[j++] = popChar(&st);
                postfix[j++] = ' ';
            }
            if (!isEmptyCharStack(&st)) popChar(&st);
            i++;
        } 
        // Operators
        else if (strchr("+-*/^", exp[i])) {
            while (!isEmptyCharStack(&st) && st.S[st.top] != '(' && precedence(st.S[st.top]) >= precedence(exp[i])) {
                postfix[j++] = popChar(&st);
                postfix[j++] = ' ';
            }
            pushChar(&st, exp[i++]);
        }
    }

    while (!isEmptyCharStack(&st) && st.S[st.top] != '$') {
        postfix[j++] = popChar(&st);
        postfix[j++] = ' ';
    }
    postfix[j] = '\0';

    return postfix;
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
        else if (isFunction(token[0])) {
            double arg = popDouble(&st);
            switch (token[0]) {
                case 's': pushDouble(&st, sin(arg)); break;
                case 'c': pushDouble(&st, cos(arg)); break;
                case 't': pushDouble(&st, tan(arg)); break;
                case 'l': pushDouble(&st, log(arg)); break;
            }
        } 
        else if (strchr("+-*/^", token[0])) {
            double b = popDouble(&st);
            double a = popDouble(&st);
            switch (token[0]) {
                case '+': pushDouble(&st, a + b); break;
                case '-': pushDouble(&st, a - b); break;
                case '*': pushDouble(&st, a * b); break;
                case '/': pushDouble(&st, a / b); break;
                case '^': pushDouble(&st, pow(a, b)); break;
            }
        }
        token = strtok(NULL, " ");
    }

    return popDouble(&st);
}

int main() {
    char infix[1000];
    
    while(1){
        printf("=> ");
        scanf("%[^\n]%*c", infix);

        if(strcmp(infix,"exit")==0)break;

        char *postFix = convertToPostFix(infix); 
        if(postFix == NULL){
            continue;
        }else{
            double result = evaluatePostfix(postFix);
            printf("Result: %.2f\n", result);
            free(postFix);
        }
    }
}
