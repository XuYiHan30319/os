extern int printf(const char *s, ...);
extern void scanf(char *c, int a);

char program[1024], paper[1024];
int ptr = 512, stack[1024], sp = 0;

void run()
{
    char *helloworld = "++++++++++[>+>+++>+++++++>++++++++++<<<<-]>>>++.>+.+++++++..+++.<<++.>+++++++++++++++.>.+++.------.--------.<<+.<.";
    printf("创建了brainFuck\n");
    int size = 0, pc = 0;
    // scanf(program, 1024);
    for (; helloworld[size] != '\0'; ++size)
        program[size] = helloworld[size];
    printf("%s\n", program);

    for (; pc < size; ++pc)
    {
        if (!sp && program[pc] == ']')
        {
            printf("sp is zero.\n");
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
        case ',':
            scanf(&paper[ptr], 1);
            break;
        case '.':
            printf("%c", paper[ptr]);
            break;
        }
        if (ptr >= 1024 || ptr < 0)
        {
            printf("paper out of range.\n");
            break;
        }
        if (sp >= 1024)
        {
            printf("sp out of range.\n");
            break;
        }
    }
    printf("\n");
    return;
}