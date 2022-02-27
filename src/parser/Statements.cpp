#include "Statements.hpp"

namespace tek::parser {

PrintStatement::PrintStatement(Statement::ExpressionPtr expression) : expression{ std::move(expression) } {}

std::string PrintStatement::accept(StatementVisitor<std::string> &visitor)
{
    return visitor.visit_print_statement(*this);
}

void PrintStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_print_statement(*this); }

ExpressionStatement::ExpressionStatement(Statement::ExpressionPtr expression) : expression{ std::move(expression) } {}

std::string ExpressionStatement::accept(StatementVisitor<std::string> &visitor)
{
    return visitor.visit_expression_statement(*this);
}

void ExpressionStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_expression_statement(*this); }

VarStatement::VarStatement(tokenizer::Token name, Statement::ExpressionPtr initializer)
  : name{ std::move(name) }, initializer{ std::move(initializer) }
{}

std::string VarStatement::accept(StatementVisitor<std::string> &visitor) { return visitor.visit_var_statement(*this); }

void VarStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_var_statement(*this); }

BlockStatement::BlockStatement(BlockStatement::StatementsVec statements) : statements{ std::move(statements) } {}

std::string BlockStatement::accept(StatementVisitor<std::string> &visitor)
{
    return visitor.visit_block_statement(*this);
}

void BlockStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_block_statement(*this); }

}// namespace tek::parser
