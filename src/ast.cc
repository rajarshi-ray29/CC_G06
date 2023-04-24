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

NodeIf::NodeIf(Node *condptr, Node *ifptr, Node *elseptr)
{
    type = IF;
    cond = condptr;
    ifstmt = ifptr;
    elsestmt = elseptr;
}

// NodeElse::NodeElse(Node *elseptr)
// {
//     type = ELSE;
//     elsestmt = elseptr;
// }

std::string
NodeBinOp::to_string()
{
    // of they are both ints, we can do the operation

    if(left -> data_type == "int" && right -> data_type == "int")
    {
        if(op == PLUS)
        {
            return std::to_string(std::stoi(left->to_string()) + std::stoi(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stoi(left->to_string()) - std::stoi(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stoi(left->to_string()) * std::stoi(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stoi(left->to_string()) / std::stoi(right->to_string()));
        }
    }

    // if they are both shorts, we can do the operation
    if(left -> data_type == "short" && right -> data_type == "short")
    {
        if(op == PLUS)
        {
            return std::to_string(std::stoi(left->to_string()) + std::stoi(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stoi(left->to_string()) - std::stoi(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stoi(left->to_string()) * std::stoi(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stoi(left->to_string()) / std::stoi(right->to_string()));
        }
    }

    // if they are both longs, we can do the operation
    if(left -> data_type == "long" && right -> data_type == "long")
    {
        if(op == PLUS)
        {
            return std::to_string(std::stol(left->to_string()) + std::stol(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stol(left->to_string()) - std::stol(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stol(left->to_string()) * std::stol(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stol(left->to_string()) / std::stol(right->to_string()));
        }
    }

    // if they are int and short, we can do the operation or if they are short and int we can do the operation
    if((left -> data_type == "int" && right -> data_type == "short") || (left -> data_type == "short" && right -> data_type == "int"))
    {
        if(op == PLUS)
        {
            return std::to_string(std::stoi(left->to_string()) + std::stoi(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stoi(left->to_string()) - std::stoi(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stoi(left->to_string()) * std::stoi(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stoi(left->to_string()) / std::stoi(right->to_string()));
        }
    }

    // if they are long and int, we can do the operation or if they are int and long we can do the operation
    if((left -> data_type == "long" && right -> data_type == "int") || (left -> data_type == "int" && right -> data_type == "long"))
    {
        if(op == PLUS)
        {
            return std::to_string(std::stol(left->to_string()) + std::stol(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stol(left->to_string()) - std::stol(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stol(left->to_string()) * std::stol(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stol(left->to_string()) / std::stol(right->to_string()));
        }
    }

    // if they are long and short, we can do the operation or if they are short and long we can do the operation

    if((left -> data_type == "long" && right -> data_type == "short") || (left -> data_type == "short" && right -> data_type == "long"))
    {
        if(op == PLUS)
        {
            return std::to_string(std::stol(left->to_string()) + std::stol(right->to_string()));
        }
        if(op == MINUS)
        {
            return std::to_string(std::stol(left->to_string()) - std::stol(right->to_string()));
        }
        if(op == MULT)
        {
            return std::to_string(std::stol(left->to_string()) * std::stol(right->to_string()));
        }
        if(op == DIV)
        {
            return std::to_string(std::stol(left->to_string()) / std::stol(right->to_string()));
        }
    }
    
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

std::string NodeIf::to_string()
{
    // if condition is 0 out is only else statement

    if(cond->to_string() == "0")
    {
        return elsestmt->to_string();
    }
    if(cond->to_string() == "1")
    {
        return ifstmt->to_string();
    }
    
    std::string out = "(if-else " + cond->to_string() + ' ' + ifstmt->to_string();
    if (elsestmt)
    {
        out += ' ' + elsestmt->to_string();
    }

    out += ')';

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

NodeLet::NodeLet(std::string id, std::string daty, Node *expr)
{
    type = LET;
    identifier = id;
    expression = expr;
    data_type = daty;
}

std::string NodeLet::to_string()
{
    return "(let (" + identifier + " " + data_type + ") " + expression->to_string() + ")";
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
