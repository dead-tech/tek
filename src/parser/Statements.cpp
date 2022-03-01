#include "Statements.hpp"

namespace tek::parser {

    PrintStatement::PrintStatement(Statement::ExpressionPtr expression) : expression{ std::move(expression) } {}

    std::string PrintStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_print_statement(*this);
    }

    void PrintStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_print_statement(*this); }

    ExpressionStatement::ExpressionStatement(Statement::ExpressionPtr expression) : expression{ std::move(expression) }
    {}

    std::string ExpressionStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_expression_statement(*this);
    }

    void ExpressionStatement::accept(StatementVisitor<void> &visitor)
    {
        return visitor.visit_expression_statement(*this);
    }

    VarStatement::VarStatement(tokenizer::Token name, Statement::ExpressionPtr initializer)
      : name{ std::move(name) }, initializer{ std::move(initializer) }
    {}

    std::string VarStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_var_statement(*this);
    }

    void VarStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_var_statement(*this); }

    BlockStatement::BlockStatement(BlockStatement::StatementsVec statements) : statements{ std::move(statements) } {}

    std::string BlockStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_block_statement(*this);
    }

    void BlockStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_block_statement(*this); }

    IfStatement::IfStatement(
      Statement::ExpressionPtr  condition,
      IfStatement::StatementPtr then_branch,
      IfStatement::StatementPtr else_branch)
      : condition{ std::move(condition) }, then_branch{ std::move(else_branch) }, else_branch{ std::move(else_branch) }
    {}

    std::string IfStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_if_statement(*this);
    }

    void IfStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_if_statement(*this); }

    WhileStatement::WhileStatement(Statement::ExpressionPtr condition, WhileStatement::StatementPtr body)
      : condition{ std::move(condition) }, body{ std::move(body) }
    {}

    std::string WhileStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_while_statement(*this);
    }

    void WhileStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_while_statement(*this); }

    ForStatement::ForStatement(
      ForStatement::StatementPtr initializer,
      Statement::ExpressionPtr   condition,
      ForStatement::StatementPtr body)
      : initializer{ std::move(initializer) }, condition{ std::move(condition) }, body{ std::move(body) }
    {}

    std::string ForStatement::accept(StatementVisitor<std::string> &visitor)
    {
        return visitor.visit_for_statement(*this);
    }

    void ForStatement::accept(StatementVisitor<void> &visitor) { return visitor.visit_for_statement(*this); }
}// namespace tek::parser
