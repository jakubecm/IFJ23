/**
 *  @file buildin.c
 *
 *
 *  @authors Milan Jakubec (xjakub41)
 */

#include "buildin.h"

void gen_buildin_funcs(gen_t *gen){

    gen_func_readstring(gen);
    gen_func_readint(gen);
    gen_func_readdouble(gen);
    gen_func_write(gen);
    gen_func_Int2Double(gen);
    gen_func_Int2Double2(gen);
    gen_func_Double2Int(gen);
    gen_func_length(gen);
    gen_func_substr(gen);
    gen_func_ord(gen);
    gen_func_chr(gen);
    gen_func_nil_check(gen);
}

void gen_func_readstring(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $readString\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@retval\n");
    mergestr(&gen->functions, "READ LF@retval string\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_readint(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $readInt\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@retval\n");
    mergestr(&gen->functions, "READ LF@retval int\n");
    mergestr(&gen->functions, "PUSHS LF@retval\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_readdouble(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $readDouble\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@retval\n");
    mergestr(&gen->functions, "READ LF@retval float\n");
    mergestr(&gen->functions, "PUSHS LF@retval\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_write(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $write\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@index\n");
    mergestr(&gen->functions, "DEFVAR LF@currterm\n");
    mergestr(&gen->functions, "POPS LF@index\n");
    mergestr(&gen->functions, "LABEL $writeLoop\n");
    mergestr(&gen->functions, "JUMPIFEQ $writeLoopEnd int@0 LF@index\n");
    mergestr(&gen->functions, "SUB LF@index LF@index int@1\n");
    mergestr(&gen->functions, "POPS LF@currterm\n");
    mergestr(&gen->functions, "WRITE LF@currterm\n");
    mergestr(&gen->functions, "JUMP $writeLoop\n");
    mergestr(&gen->functions, "LABEL $writeLoopEnd\n");
    mergestr(&gen->functions, "PUSHS nil@nil\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_Int2Double(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $int2float\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "POPS LF@temp\n");
    mergestr(&gen->functions, "INT2FLOAT LF@temp LF@temp\n");
    mergestr(&gen->functions, "PUSHS LF@temp\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_Int2Double2(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $int2float2\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp1\n");
    mergestr(&gen->functions, "DEFVAR LF@temp2\n");
    mergestr(&gen->functions, "POPS LF@temp1\n");
    mergestr(&gen->functions, "POPS LF@temp2\n");
    mergestr(&gen->functions, "INT2FLOAT LF@temp2 LF@temp2\n");
    mergestr(&gen->functions, "PUSHS LF@temp2\n");
    mergestr(&gen->functions, "PUSHS LF@temp1\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_Double2Int(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $float2int\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "POPS LF@temp\n");
    mergestr(&gen->functions, "FLOAT2INT LF@temp LF@temp\n");
    mergestr(&gen->functions, "PUSHS LF@temp\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_length(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $length\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "POPS LF@temp\n");
    mergestr(&gen->functions, "STRLEN LF@temp LF@temp\n");
    mergestr(&gen->functions, "PUSHS LF@temp\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_substr(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $substr\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@index1\n");
    mergestr(&gen->functions, "DEFVAR LF@index2\n");
    mergestr(&gen->functions, "DEFVAR LF@string\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "DEFVAR LF@result\n");
    mergestr(&gen->functions, "DEFVAR LF@length\n");
    mergestr(&gen->functions, "POPS LF@string\n");
    mergestr(&gen->functions, "TYPE GF@tmp LF@string\n");
    mergestr(&gen->functions, "JUMPIFNEQ $ERR_SEM_CALL string@string GF@tmp\n"); // nesedi typ
    mergestr(&gen->functions, "STRLEN LF@length LF@string\n");
    mergestr(&gen->functions, "POPS LF@index1\n");
    mergestr(&gen->functions, "POPS LF@index2\n");
    mergestr(&gen->functions, "LT GF@tmp LF@index1 int@0\n");
    mergestr(&gen->functions, "JUMPIFEQ $returnNull GF@tmp bool@true\n"); // i < 0
    mergestr(&gen->functions, "LT GF@tmp LF@index2 int@0\n");
    mergestr(&gen->functions, "JUMPIFEQ $returnNull GF@tmp bool@true\n"); // j < 0
    mergestr(&gen->functions, "GT GF@tmp LF@index1 LF@index2\n"); // i > j
    mergestr(&gen->functions, "JUMPIFEQ $returnNull GF@tmp bool@true\n");
    mergestr(&gen->functions, "GT GF@tmp LF@index1 LF@length\n"); // i > length
    mergestr(&gen->functions, "JUMPIFEQ $returnNull GF@tmp bool@true\n");
    mergestr(&gen->functions, "JUMPIFEQ $returnNull LF@index1 LF@length\n");
    mergestr(&gen->functions, "GT GF@tmp LF@index2 LF@length\n"); // j > length
    mergestr(&gen->functions, "JUMPIFEQ $returnNull GF@tmp bool@true\n");
    mergestr(&gen->functions, "MOVE LF@result string@\n");
    mergestr(&gen->functions, "LABEL $substrLoop\n");
    mergestr(&gen->functions, "JUMPIFEQ $substrLoopEnd LF@index1 LF@index2\n");
    mergestr(&gen->functions, "GETCHAR LF@temp LF@string LF@index1\n");
    mergestr(&gen->functions, "CONCAT LF@result LF@result LF@temp\n");
    mergestr(&gen->functions, "ADD LF@index1 LF@index1 int@1\n");
    mergestr(&gen->functions, "JUMP $substrLoop\n");
    mergestr(&gen->functions, "LABEL $substrLoopEnd\n");
    mergestr(&gen->functions, "PUSHS LF@result\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
    mergestr(&gen->functions, "LABEL $returnNull\n");
    mergestr(&gen->functions, "PUSHS nil@nil\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_ord(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $ord\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "POPS LF@temp\n");
    mergestr(&gen->functions, "TYPE GF@tmp LF@temp\n");
    mergestr(&gen->functions, "JUMPIFNEQ $ERR_SEM_CALL string@string GF@tmp\n"); // nesedi typ
    mergestr(&gen->functions, "JUMPIFEQ $ord0 string@ LF@tmp\n");
    mergestr(&gen->functions, "STRI2INT LF@temp LF@temp int@0\n");
    mergestr(&gen->functions, "PUSHS LF@temp\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
    mergestr(&gen->functions, "LABEL $ord0\n");
    mergestr(&gen->functions, "PUSHS int@0\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_chr(gen_t *gen)
{
    mergestr(&gen->functions, "LABEL $chr\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@temp\n");
    mergestr(&gen->functions, "POPS LF@temp\n");
    mergestr(&gen->functions, "TYPE GF@tmp LF@temp\n");
    mergestr(&gen->functions, "JUMPIFNEQ $ERR_SEM_CALL string@int GF@tmp\n"); // nesedi typ
    mergestr(&gen->functions, "INT2CHAR LF@temp LF@temp\n");
    mergestr(&gen->functions, "PUSHS LF@temp\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}

void gen_func_nil_check(gen_t *gen) {
    mergestr(&gen->functions, "LABEL $nil_check\n");
    mergestr(&gen->functions, "CREATEFRAME\n");
    mergestr(&gen->functions, "PUSHFRAME\n");
    mergestr(&gen->functions, "DEFVAR LF@op1\n");
    mergestr(&gen->functions, "DEFVAR LF@op2\n");
    mergestr(&gen->functions, "POPS LF@op1\n");
    mergestr(&gen->functions, "POPS LF@op2\n");
    mergestr(&gen->functions, "JUMPIFNEQ $op2NIL LF@op2 nil@nil\n");
    mergestr(&gen->functions, "JUMPIFNEQ $op2NotNIL LF@op2 nil@nil\n");
    
    mergestr(&gen->functions, "LABEL $op2NIL\n");
    mergestr(&gen->functions, "PUSHS LF@op1\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");

    mergestr(&gen->functions, "LABEL $op2NotNIL\n");
    mergestr(&gen->functions, "PUSHS LF@op2\n");
    mergestr(&gen->functions, "POPFRAME\n");
    mergestr(&gen->functions, "RETURN\n");
}