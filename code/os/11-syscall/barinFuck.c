#include "user_api.h"

char program[1024], paper[1024];
int ptr = 512, stack[1024], sp = 0;

void run()
{
    char *helloworld = "++++++++++[>+>+++>+++++++>++++++++++<<<<-]>>>++.>+.+++++++..+++.<<++.>+++++++++++++++.>.+++.------.--------.<<+.<.";
    myprintfWithoutCanshu("创建了brain**\n");
    int size = 0, pc = 0;
    for (; helloworld[size] != '\0'; ++size)
        program[size] = helloworld[size];

    for (; pc < size; ++pc)
    {
        if (!sp && program[pc] == ']')
        {
            myprintfWithoutCanshu("sp is zero.\n");
            break;
        }
        switch (program[pc])
        {
        case '+':
            ++paper[ptr];
            break;
        case '-':
            --paper[ptr];
            break;
        case '[':
            stack[sp] = pc;
            ++sp;
            break;
        case ']':
            pc = paper[ptr] ? stack[sp - 1] : pc;
            sp -= (!paper[ptr]);
            break;
        case '>':
            ++ptr;
            break;
        case '<':
            --ptr;
            break;
        // case ',':
        //     scanf(&paper[ptr], 1);
        //     break;
        case '.':
            myprintf("%c", paper[ptr]);
            break;
        }
        if (ptr >= 1024 || ptr < 0)
        {
            myprintfWithoutCanshu("paper out of range.\n");
            break;
        }
        if (sp >= 1024)
        {
            myprintfWithoutCanshu("sp out of range.\n");
            break;
        }
    }
    return;
}