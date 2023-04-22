#include "ast.hh"

#include <string>
#include <vector>

NodeBinOp::NodeBinOp(NodeBinOp::Op ope, Node *leftptr, Node *rightptr)
{
    type = BIN_OP;
    op = ope;
    left = leftptr;
    right = rightptr;
    if(left->data_type == "short" && right->data_type == "short")
    {
        data_type = "short";
    }
    else if(left->data_type == "int" && right->data_type == "int")
    {
        data_type = "int";
    }
    else if(left->data_type == "long" || right->data_type == "long")
    {
        data_type = "long";
    }
    else if((left->data_type == "int" && right->data_type=="short")||(left->data_type == "short" && right->data_type=="int"))
    {
        data_type = "int";
    }
   
}

NodeTernary::NodeTernary(NodeTernary::Op ope, Node *leftptr, Node *rightptr, Node *midptr)
{
    type = TERNARY;
    op = ope;
    left = leftptr;
    right = rightptr;
    mid = midptr;
}

std::string
NodeBinOp::to_string()
{
    std::string out = "(";
    switch (op)
    {
    case PLUS:
        out += '+';
        break;
    case MINUS:
        out += '-';
        break;
    case MULT:
        out += '*';
        break;
    case DIV:
        out += '/';
        break;
    }

    out += ' ' + left->to_string() + ' ' + right->to_string() + ')';

    return out;
}

std::string NodeTernary::to_string()
{
    std::string out = "(?:";

    out += ' ' + left->to_string() + ' ' + right->to_string() + ' ' + mid->to_string() + ')';

    return out;
}

NodeInt::NodeInt(long val)
{
    type = INT_LIT;
    if(val<INT16_MAX)
    {
        data_type = "short";
    }
    else if(val<INT32_MAX)
    {
        data_type = "int";
    }
    else
    {
        data_type = "long";
    }
    value = val;
}

std::string NodeInt::to_string()
{
    return std::to_string(value);
}

NodeStmts::NodeStmts()
{
    type = STMTS;
    list = std::vector<Node *>();
}

void NodeStmts::push_back(Node *node)
{
    list.push_back(node);
}

std::string NodeStmts::to_string()
{
    std::string out = "(begin";
    for (auto i : list)
    {
        out += " " + i->to_string();
    }

    out += ')';

    return out;
}

NodeLet:: NodeLet(std::string id, Node *expr)
{
    type = LET;
    identifier = id;
    expression = expr;
}

std::string NodeLet::to_string()
{
    return "(let " + identifier + " " + expression->to_string() + ")";
}

NodeAssn::NodeAssn(std::string id, Node *expr)
{
    type = ASSN;
    identifier = id;
    expression = expr;
}

std::string NodeAssn::to_string()
{
    return "(assign " + identifier + " " + expression->to_string() + ")";
}

NodeDebug::NodeDebug(Node *expr)
{
    type = DBG;
    expression = expr;
}

std::string NodeDebug::to_string()
{
    return "(dbg " + expression->to_string() + ")";
}

NodeIdent::NodeIdent(std::string ident)
{
    identifier = ident;
}
std::string NodeIdent::to_string()
{
    return identifier;
}
