/*
 * Jeffrey Castillejo
 */
#include "ciLisp.h"


void yyerror(char *s) {
    fprintf(stderr, "\nERROR: %s\n", s);
    // note stderr that normally defaults to stdout, but can be redirected: ./src 2> src.log
    // CLion will display stderr in a different color from stdin and stdout
}

// Array of string values for operations.
// Must be in sync with funcs in the OPER_TYPE enum in order for resolveFunc to work.
char *funcNames[] = {
        "neg",
        "abs",
        "exp",
        "sqrt",
        "add",
        "sub",
        "mult",
        "div",
        "remainder",
        "log",
        "pow",
        "max",
        "min",
        "exp2",
        "cbrt",
        "hypot",
        "read",
        "rand",
        "print",
        "equal",
        "less",
        "greater",
        ""
};

char *nameForNodes[] = {
        "Number ","Function"
};


OPER_TYPE resolveFunc(char *funcName)
{
    int i = 0;
    while (funcNames[i][0] != '\0')
    {
        if (strcmp(funcNames[i], funcName) == 0)
            return i;
        i++;
    }
    return CUSTOM_OPER;
}

// Called when an INT or DOUBLE token is encountered (see ciLisp.l and ciLisp.y).
// Creates an AST_NODE for the number.
// Sets the AST_NODE's type to number.
// Populates the value of the contained NUMBER_AST_NODE with the argument value.
// SEE: AST_NODE, NUM_AST_NODE, AST_NODE_TYPE.
AST_NODE *createNumberNode(char *numberType, double value, NUM_TYPE type)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space for the fixed sie and the variable part (union)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, assign values to contained NUM_AST_NODE DONE
    node->data.number.type = type;
    node->type = NUM_NODE_TYPE;
    //populate this with the type that was past in and the value
    node->type = NUM_NODE_TYPE;

    if( numberType== NULL || (strcmp("int",numberType) == 0 && type == INT_TYPE)
       || (strcmp("double",numberType) == 0 && type == DOUBLE_TYPE))
    {

        node->data.number.type = type;

        if(type == INT_TYPE)
        {
            node->data.number.value.ival = value;
        }
        else
        {
            node->data.number.value.dval = value;
        }
    }
    else if(strcmp("int",numberType) == 0)
    {
        node->data.number.type = INT_TYPE;
        printf("\"WARNING: precision loss in the typecasting of a number from double to int\"");

        if(fmod(value,1) >= 0.5)
        {
            double newval = value + .5;
            node->data.number.value.ival = newval;

        }
        else
        {
            node->data.number.value.ival = value;
        }
    }
    else
    {
        node->data.number.type = DOUBLE_TYPE;
        node->data.number.value.dval = value;
    }



    return node;
}


// Called when an f_expr is created (see ciLisp.y).
// Creates an AST_NODE for a function call.
// Sets the created AST_NODE's type to function.
// Populates the contained FUNC_AST_NODE with:
//      - An OPER_TYPE (the enum identifying the specific function being called)
//      - 2 AST_NODEs, the operands
// SEE: AST_NODE, FUNC_AST_NODE, AST_NODE_TYPE.
AST_NODE *createFunctionNode(char *funcName, AST_NODE *op1, AST_NODE *op2)
{
    AST_NODE *node;
    size_t nodeSize;

    // allocate space (or error)
    nodeSize = sizeof(AST_NODE);
    if ((node = calloc(nodeSize, 1)) == NULL)
        yyerror("Memory allocation failed!");

    // TODO set the AST_NODE's type, populate contained FUNC_AST_NODE DONE
    // NOTE: you do not need to populate the "ident" field unless the function is type CUSTOM_OPER.
    // When you do have a CUSTOM_OPER, you do NOT need to allocate and strcpy here.
    // The funcName will be a string identifier for which space should be allocated in the tokenizer.
    // For CUSTOM_OPER functions, you should simply assign the "ident" pointer to the passed in funcName.
    // For functions other than CUSTOM_OPER, you should free the funcName after you're assigned the OPER_TYPE.
    node->type = FUNC_NODE_TYPE;
    node->data.function.oper = resolveFunc(funcName);
    node->data.function.op1 = op1;
    op1->parent = node;
    if (op2 != NULL) {
        node->data.function.op2 = op2;
        op2->parent = node;
    }else{
        printf("Operator 2 is NULL\n");
    }

    return node;

}

// Called after execution is done on the base of the tree.
// (see the program production in ciLisp.y)
// Recursively frees the whole abstract syntax tree.
// You'll need to update and expand freeNode as the project develops.
void freeNode(AST_NODE *node)
{
    if (!node)
        return;

    if (node->type == FUNC_NODE_TYPE)
    {
        // Recursive calls to free child nodes
        freeNode(node->data.function.op1);
        freeNode(node->data.function.op2);

        // Free up identifier string if necessary
        if (node->data.function.oper == CUSTOM_OPER)
        {
            free(node->data.function.ident);
        }
    }

    free(node);
}

// Evaluates an AST_NODE.
// returns a RET_VAL storing the the resulting value and type.
// You'll need to update and expand eval (and the more specific eval functions below)
// as the project develops.
RET_VAL eval(AST_NODE *node)
{
    if (!node)
        return (RET_VAL){INT_TYPE, {NAN}};

    RET_VAL result = {INT_TYPE, {NAN}}; // see NUM_AST_NODE, because RET_VAL is just an alternative name for it.

    // TODO complete the switch. DONE
    // Make calls to other eval functions based on node type.
    // Use the results of those calls to populate result.
    switch (node->type){

        case FUNC_NODE_TYPE:
            result = evalFuncNode(&node->data.function);
            break;
        case NUM_NODE_TYPE:
            result = evalNumNode(&node->data.number);
            break;
        default:
            yyerror("ya dumb bitch!");
    }

    return result;
}

// returns a pointer to the NUM_AST_NODE (aka RET_VAL) referenced by node.
// DOES NOT allocate space for a new RET_VAL.
RET_VAL evalNumNode(NUM_AST_NODE *numNode)
{
    if (!numNode)
        return (RET_VAL){INT_TYPE, {NAN}};

    RET_VAL result = {INT_TYPE, {NAN}};

    // TODO populate result with the values stored in the node. DONE
    // SEE: AST_NODE, AST_NODE_TYPE, NUM_AST_NODE
    result.type = numNode->type;
    result.value = numNode->value;

    return result;

}


RET_VAL evalFuncNode(FUNC_AST_NODE *funcNode) {
    if (!funcNode)
        return (RET_VAL) {INT_TYPE, {NAN}};

    RET_VAL result = {INT_TYPE, {NAN}};

    // TODO populate result with the result of running the function on its operands. DONE
    // SEE: AST_NODE, AST_NODE_TYPE, FUNC_AST_NODE
    RET_VAL op1 = eval(funcNode->op1);
    RET_VAL op2 = eval((funcNode->op2));


    switch (funcNode->oper) {
        case NEG_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.type = INT_TYPE;
                    result.value.ival = -1 * op1.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = -1 * op1.value.dval;
                    break;
            }
            break;
        case ABS_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.type = INT_TYPE;
                    result.value.ival = labs(op1.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = fabs(op1.value.dval);
            }
            break;
        case EXP_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    op1.value.dval = op1.value.ival;
                    result.value.dval = exp(op1.value.dval);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = exp(op1.value.dval);
                    break;
            }
            break;
        case SQRT_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    op1.value.dval = op1.value.ival;
                    result.value.dval = sqrt(op1.value.dval);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = sqrt(op1.value.dval);
                    break;
            }
            break;
        case ADD_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = op1.value.ival + op2.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = op1.value.dval + op2.value.dval;
                    break;
            }
            break;
        case SUB_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = op1.value.ival - op2.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = op1.value.dval - op2.value.dval;
                    break;
            }
            break;
        case MULT_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = op1.value.ival * op2.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = op1.value.dval * op2.value.dval;
                    break;
            }
            break;
        case DIV_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = op1.value.ival / op2.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = op1.value.dval / op2.value.dval;
                    break;
            }
            break;
        case REMAINDER_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = op1.value.ival % op2.value.ival;
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = fmod(op1.value.dval, op2.value.dval);
                    break;
            }
            break;
        case LOG_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    op1.value.dval = op1.value.ival;
                    result.value.ival = (int) log(op1.value.dval);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = log(op1.value.dval);
                    break;
            }
            break;
        case POW_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = pow(op1.value.ival, op2.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = pow(op1.value.dval, op2.value.dval);
                    break;
            }
            break;
        case MAX_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = (op1.value.ival > op2.value.ival ?
                                         op1.value.ival : op2.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = (op1.value.dval > op2.value.dval ?
                                         op1.value.dval : op2.value.dval);
                    break;
            }
            break;
        case MIN_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.value.ival = (op1.value.ival < op2.value.ival ?
                                         op1.value.ival : op2.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = (op1.value.dval < op2.value.dval ?
                                         op1.value.dval : op2.value.dval);
                    break;
            }
            break;
        case EXP2_OPER:
            switch (op1.type) {
                case INT_TYPE:
                    result.value.ival = powl(2, op1.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = pow(2, op1.value.dval);
                    break;
            }

            break;
        case CBRT_OPER: // cube root
            switch (op1.type) {
                case INT_TYPE:
                    result.value.ival = cbrtl(op1.value.ival);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = cbrt(op1.value.dval);
                    break;
            }

            break;
        case HYPOT_OPER:
            switch (evalBinary(&op1, &op2)) {
                case INT_TYPE:
                    result.type = DOUBLE_TYPE;
                    op1.value.dval = op1.value.ival;
                    op2.value.dval = op2.value.ival;
                    result.value.dval = hypotl(op1.value.dval, op2.value.dval);
                    break;
                case DOUBLE_TYPE:
                    result.type = DOUBLE_TYPE;
                    result.value.dval = hypotl(op1.value.dval, op2.value.dval);
                    break;
            }
        case READ_OPER:
        case RAND_OPER:

            break;
        case PRINT_OPER:
            printRetVal(op1);
            printf("\n");
            if (op1.type == INT_TYPE) {
                result.type = INT_TYPE;
                result.value.ival = op1.value.ival;
            } else {
                result.type = DOUBLE_TYPE;
                result.value.dval = op1.value.dval;
            }

            break;
        case EQUAL_OPER:
        case LESS_OPER:
        case GREATER_OPER:
        case CUSTOM_OPER:
            break;



    }
    return result;
}

NUM_TYPE evalBinary(RET_VAL *op1, RET_VAL *op2)
{
    NUM_TYPE type;
    if(op1->type != op2->type)
    {
        type = DOUBLE_TYPE;

        if(op1->type == INT_TYPE)
        {
            op1->type = DOUBLE_TYPE;
            op1->value.dval = (double)op1->value.ival;
        }
        else if (op2->type == INT_TYPE)
        {
            op2->type = DOUBLE_TYPE;
            op2->value.dval = (double)op2->value.ival;
        }
    }
    else if( op1->type == INT_TYPE && op2->type == INT_TYPE)
    {
        type = INT_TYPE;
    }
    else
    {
        type = DOUBLE_TYPE;
    }

    return type;
}


// prints the type and value of a RET_VAL
void printRetVal(RET_VAL val)
{
    // TODO print the type and value of the value passed in. DONE
    switch (val.type)
    {
        case INT_TYPE:
            printf("\nType: INT");
            printf("\nValue: %ld",val.value.ival);
            break;
        case DOUBLE_TYPE:
            printf("\nType: DOUBLE");
            printf("\nValue: %lf", val.value.dval);
            break;
    }


}

