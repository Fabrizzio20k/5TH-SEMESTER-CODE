int yylex()
{
    char ch;

    do
    {
        ch = cin.peek();
        if (isalpha(ch))
        {
            cin.get();
            if (ch == 's')
            {
                if (cin.peek() == 'i')
                {
                    cin.get();
                    if (cin.peek() == 'n')
                    {
                        cin.get();
                        return SIN;
                    }
                }
            }
            else if (ch == 'c')
            {
                if (cin.peek() == 'o')
                {
                    cin.get();
                    if (cin.peek() == 's')
                    {
                        cin.get();
                        return COS;
                    }
                }
            }
            else if (ch == 't')
            {
                if (cin.peek() == 'a')
                {
                    cin.get();
                    if (cin.peek() == 'n')
                    {
                        cin.get();
                        return TAN;
                    }
                }
            }

            else if (ch == 'l')
            {
                if (cin.peek() == 'o')
                {
                    cin.get();
                    if (cin.peek() == 'g')
                    {
                        cin.get();
                        return LOG;
                    }
                }
            }

            else if (ch == 'e')
            {
                if (cin.peek() == 'x')
                {
                    cin.get();
                    if (cin.peek() == 'p')
                    {
                        cin.get();
                        return EXP;
                    }
                }
            }

            yylval.ident = ch;
            return IDENT;
        }
        else if (isdigit(ch))
        {
            int value = 0;
            while (!cin.eof() && isdigit(ch))
            {
                cin.get();

                value = value * 10 + ch - '0';
                ch = cin.peek();
            }

            yylval.value = value;
            return NUMBER;
        }
        else if (ch == '+' || ch == '-' || ch == '\n' || ch == '*' || ch == '/' || ch == '=')
        {
            cin.get();

            return ch;
        }
        else
            cin.get();

    } while (!cin.eof());

    return -1;
}