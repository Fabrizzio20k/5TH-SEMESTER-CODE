#ifndef EXPRESSION
#define EXPRESSION

#include <cmath>

class Expression
{
public:
    virtual ~Expression() {}

    //  It's necessary because we need to clone objects without
    // knowing the exact type.
    virtual Expression *clone() const = 0;

    // The value represented by the expression
    virtual float value() = 0;
};

// For addictive expressions
class Plus : public Expression
{
    Expression *m_left, *m_right;

public:
    Plus(Expression *left, Expression *right) : m_left(left), m_right(right) {}

    // Copy constructor
    Plus(const Plus &other)
    {
        m_left = other.m_left->clone();
        m_right = other.m_right->clone();
    }

    virtual ~Plus()
    {
        delete m_left;
        delete m_right;
    }

    Plus &operator=(const Plus &other)
    {
        if (&other != this)
        {
            delete m_left;
            delete m_right;
            m_left = other.m_left->clone();
            m_right = other.m_right->clone();
        }
        return *this;
    }

    virtual Plus *clone() const { return new Plus(*this); }

    virtual float value() { return m_left->value() + m_right->value(); }
};

// For substractive expressions
class Minus : public Expression
{
    Expression *m_left, *m_right;

public:
    Minus(Expression *left, Expression *right) : m_left(left), m_right(right) {}

    // Copy constructor
    Minus(const Minus &other)
    {
        m_left = other.m_left->clone();
        m_right = other.m_right->clone();
    }

    virtual ~Minus()
    {
        delete m_left;
        delete m_right;
    }

    Minus &operator=(const Minus &other)
    {
        if (&other != this)
        {
            delete m_left;
            delete m_right;
            m_left = other.m_left->clone();
            m_right = other.m_right->clone();
        }
        return *this;
    }

    virtual Minus *clone() const { return new Minus(*this); }

    virtual float value() { return m_left->value() - m_right->value(); }
};

// For multiplicative expressions
class Multiply : public Expression
{
    Expression *m_left, *m_right;

public:
    Multiply(Expression *left, Expression *right) : m_left(left), m_right(right) {}

    // Copy constructor
    Multiply(const Multiply &other)
    {
        m_left = other.m_left->clone();
        m_right = other.m_right->clone();
    }

    virtual ~Multiply()
    {
        delete m_left;
        delete m_right;
    }

    Multiply &operator=(const Multiply &other)
    {
        if (&other != this)
        {
            delete m_left;
            delete m_right;
            m_left = other.m_left->clone();
            m_right = other.m_right->clone();
        }
        return *this;
    }

    virtual Multiply *clone() const { return new Multiply(*this); }

    virtual float value() { return m_left->value() * m_right->value(); }
};

// For division expressions
class Divide : public Expression
{
    Expression *m_left, *m_right;

public:
    Divide(Expression *left, Expression *right) : m_left(left), m_right(right) {}

    // Copy constructor
    Divide(const Divide &other)
    {
        m_left = other.m_left->clone();
        m_right = other.m_right->clone();
    }

    virtual ~Divide()
    {
        delete m_left;
        delete m_right;
    }

    Divide &operator=(const Divide &other)
    {
        if (&other != this)
        {
            delete m_left;
            delete m_right;
            m_left = other.m_left->clone();
            m_right = other.m_right->clone();
        }
        return *this;
    }

    virtual Divide *clone() const { return new Divide(*this); }

    virtual float value() { return m_left->value() / m_right->value(); }
};

// For trigonometric expressions
class Trigonometric : public Expression
{
    Expression *m_expr;
    char m_op;

public:
    Trigonometric(Expression *expr, char op) : m_expr(expr), m_op(op) {}

    // Copy constructor
    Trigonometric(const Trigonometric &other)
    {
        m_expr = other.m_expr->clone();
        m_op = other.m_op;
    }

    virtual ~Trigonometric() { delete m_expr; }

    Trigonometric &operator=(const Trigonometric &other)
    {
        if (&other != this)
        {
            delete m_expr;
            m_expr = other.m_expr->clone();
            m_op = other.m_op;
        }
        return *this;
    }

    virtual Trigonometric *clone() const { return new Trigonometric(*this); }

    virtual float value()
    {
        switch (m_op)
        {
        case 's':
            return sin(m_expr->value());
        case 'c':
            return cos(m_expr->value());
        case 't':
            return tan(m_expr->value());
        default:
            return 0;
        }
    }
};

// For logarithmic expressions
class Logarithm : public Expression
{
    Expression *m_expr;
    char m_op;

public:
    Logarithm(Expression *expr, char op) : m_expr(expr), m_op(op) {}

    // Copy constructor
    Logarithm(const Logarithm &other)
    {
        m_expr = other.m_expr->clone();
        m_op = other.m_op;
    }

    virtual ~Logarithm() { delete m_expr; }

    Logarithm &operator=(const Logarithm &other)
    {
        if (&other != this)
        {
            delete m_expr;
            m_expr = other.m_expr->clone();
            m_op = other.m_op;
        }
        return *this;
    }

    virtual Logarithm *clone() const { return new Logarithm(*this); }

    virtual float value()
    {
        switch (m_op)
        {
        case 'l':
            return log(m_expr->value());
        case 'e':
            return exp(m_expr->value());
        default:
            return 0;
        }
    }
};

// For numbers
class Number : public Expression
{
    float m_val;

public:
    Number(float val) : m_val(val) {}

    // Copy constructor
    Number(const Number &other) { m_val = other.m_val; }

    Number &operator=(const Number &other)
    {
        if (&other != this)
        {
            m_val = other.m_val;
        }
        return *this;
    }

    virtual Number *clone() const { return new Number(*this); }

    virtual float value() { return m_val; }
};

#endif