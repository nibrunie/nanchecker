#include <stdint.h>
#include <inttypes.h>
#include <stdio.h>
#include <math.h>

/** Union type to easily cast fp32 and 32-bit unsigned integer values */
typedef union {
    /** 32-bit unsigned integer view */
    uint32_t u;
    /** 32-bit floating point view */
    float f;
} uf_t;



/** static array of qNaNs to be used as operands */
const uf_t checker_NaNs[] = {
    {.u = 0xffc0aaaa},
    {.u = 0xffc0bbbb},
    {.u = 0xffc0cccc},
};

/** description of binary operation */
typedef struct {
    /** operation descriptor */
    const char* op_desc;
    /** format descriptor */
    const char* format_desc;
    /** functionnal operator **/
    float (*func)(float, float);
} checker_binary_op_desc_t;

#define GEN_FUNC_2OP_MACRO(format, name, op) __attribute__((noinline)) format name(format lhs, format rhs) { return lhs op rhs;}

GEN_FUNC_2OP_MACRO(float, fp32_add, +)
GEN_FUNC_2OP_MACRO(float, fp32_mul, *)
GEN_FUNC_2OP_MACRO(float, fp32_sub, -)
GEN_FUNC_2OP_MACRO(float, fp32_div, /)

const checker_binary_op_desc_t BIN2_OPLIST[] = {
    {.op_desc = "add", .format_desc = "fp32", .func = fp32_add},
    {.op_desc = "mul", .format_desc = "fp32", .func = fp32_mul},
    {.op_desc = "sub", .format_desc = "fp32", .func = fp32_sub},
    {.op_desc = "div", .format_desc = "fp32", .func = fp32_div}
};


int main(void) {
    const int verbose = 0;
    const int NAN_NUM = sizeof(checker_NaNs) / sizeof(checker_NaNs[0]);

    const int num_op = sizeof(BIN2_OPLIST) / sizeof(BIN2_OPLIST[0]);
    // iterating over possible operations
    for (int op_index = 0; op_index < num_op; op_index++) {
        const char* op_desc = BIN2_OPLIST[op_index].op_desc;
        const char* format_desc = BIN2_OPLIST[op_index].format_desc;
        unsigned lhs_count = 0, rhs_count = 0;
        // iterating over possible pairs of left-hand side and right-hand side
        // operand variables
        for (int lhs_index=0; lhs_index < NAN_NUM; lhs_index++)
        {
            uf_t lhs_op = checker_NaNs[lhs_index];
            for (int rhs_index=0; rhs_index < NAN_NUM; rhs_index++)
            {
                uf_t rhs_op = checker_NaNs[rhs_index];
                uf_t result;
                result.f = BIN2_OPLIST[op_index].func(lhs_op.f, rhs_op.f);
                if (verbose)  {
                    printf("%s_%s(NaN[%"PRIx32"], NaN[%"PRIx32"]) = %f/(%"PRIx32")\n",
                            format_desc, op_desc, lhs_op.u, rhs_op.u, result.f, result.u);
                }
                if (result.u == lhs_op.u) lhs_count++;
                if (result.u == rhs_op.u) rhs_count++;
            }
        }
        const unsigned test_count = NAN_NUM * NAN_NUM;
        if (lhs_count == test_count) {
            printf("%s %s forwards left-hand-side NaN payload\n", format_desc, op_desc);
        } else if (rhs_count == test_count) {
            printf("%s %s forwards right-hand-side NaN payload\n", format_desc, op_desc);
        } else {
            printf("unable to determine NaN behavior for %s %s: lhs_count=%u, rhs_count=%u\n", 
                   format_desc, op_desc, lhs_count, rhs_count);
        }
    }

    // testing fma
    unsigned op0_count = 0, op1_count = 0, op2_count = 0;
    for (int op0_index=0; op0_index < NAN_NUM; op0_index++) {
        uf_t op0 = checker_NaNs[op0_index];
        for (int op1_index=0; op1_index < NAN_NUM; op1_index++) {
            uf_t op1 = checker_NaNs[op1_index];
            for (int op2_index=0; op2_index < NAN_NUM; op2_index++) {
                uf_t op2 = checker_NaNs[op2_index];
                uf_t result;
                result.f = fmaf(op0.f, op1.f, op2.f);
                if (verbose)  {
                    printf("fmaf(NaN[%"PRIx32"], NaN[%"PRIx32"], NaN[%"PRIx32"]) = %f/(%"PRIx32")\n",
                            op0.u, op1.u, op2.u, result.f, result.u);
                }
                if (result.u == op0.u) op0_count++;
                if (result.u == op1.u) op1_count++;
                if (result.u == op2.u) op2_count++;
            }
        }
    }
    const unsigned test_count = NAN_NUM * NAN_NUM * NAN_NUM;
    if (op0_count == test_count) {
        printf("fmaf forwards left-hand-side NaN payload\n");
    } else if (op1_count == test_count) {
        printf("fmaf forwards middle-hand-side NaN payload\n");
    } else if (op2_count == test_count) {
        printf("fmaf forwards right-hand-side NaN payload\n");
    } else {
        printf("unable to determine NaN behavior for fmaf: op0_count=%u, op1_count=%u, op2_count=%u\n", 
               op0_count, op1_count, op2_count);
    }


    return 0;
}
