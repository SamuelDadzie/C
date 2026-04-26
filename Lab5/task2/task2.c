#include <stdio.h>
#include <stdlib.h>

/* Function declarations */
int add      (int a, int b);
int subtract (int a, int b);
int multiply (int a, int b);
int divide   (int a, int b);
int ex       (int a, int b);
int invalid  (int a, int b);

/* Function pointer type */
typedef int (*Operation)(int, int);

int main(void)
{
    int a = 6;
    int b = 3;
    int x;
    char op;

    /* Array of function pointers indexed by operation choice 0-4
     * Index 5+ maps to invalid to handle out-of-range without conditionals */
    Operation ops[256] = { [0 ... 255] = invalid };
    ops['0'] = add;
    ops['1'] = subtract;
    ops['2'] = multiply;
    ops['3'] = divide;
    ops['4'] = ex;

    /* Labels indexed by char value */
    char *op_labels[256] = { [0 ... 255] = "Invalid operation" };
    op_labels['0'] = "Adding 'a' and 'b'";
    op_labels['1'] = "Subtracting 'b' from 'a'";
    op_labels['2'] = "Multiplying 'a' and 'b'";
    op_labels['3'] = "Dividing 'a' by 'b'";
    op_labels['4'] = "Exiting";

    printf("Operand 'a' : %d | Operand 'b' : %d\n", a, b);

    while (1)
    {
        printf("Specify the operation to perform "
               "(0 : add | 1 : subtract | 2 : Multiply | 3 : divide | 4 : exit): ");
        scanf(" %c", &op);

        /* Print operation label */
        printf("%s\n", op_labels[(unsigned char)op]);

        /* Call the function via pointer - no conditionals needed */
        x = ops[(unsigned char)op](a, b);

        printf("x = %d\n", x);
    }

    return 0;
}

/* Function definitions - no print statements (main handles labels) */
int add      (int a, int b) { return a + b; }
int subtract (int a, int b) { return a - b; }
int multiply (int a, int b) { return a * b; }
int divide   (int a, int b) { return a / b; }
int ex       (int a, int b) { exit(0); return 0; }
int invalid  (int a, int b) { return 0; }