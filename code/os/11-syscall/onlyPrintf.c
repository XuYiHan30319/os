#include "os.h"
#include "user_api.h"

void *memset(void *s, int c, size_t n)
{
    unsigned char *p = s;
    while (n--)
    {
        ;
        *p++ = (unsigned char)c;
    }
    return s;
}

void *memcpy(void *dest, const void *src, size_t n)
{
    char *csrc = (char *)src;
    char *cdest = (char *)dest;
    for (size_t i = 0; i < n; i++)
    {
        ;
        cdest[i] = csrc[i];
    }
    return dest;
}

char *append(char *s1, const char *s2)
{
    int len = 0;
    while (*(s1 + len) != '\0')
    {
        len++;
    }
    int i = 0;
    while (*(s2 + i) != '\0')
    {
        *(s1 + len) = *(s2 + i);
        i++;
        len++;
    }
    s1[len] = '\0';
    return s1;
}

char *appendchar(char *s1, const char s2)
{
    int len = 0;
    while (*(s1 + len) != '\0')
    {
        len++;
    }
    s1[len] = s2;
    s1[len + 1] = '\0';
    return s1;
}

int if_equal_to_myprintfWithoutCanshu(const char *s)
{
    const char *target = "myprintfWithoutCanshu";
    for (int i = 0; i < 21; i++)
    {
        if (*(s + i) != target[i])
        {
            return 0;
        }
    }
    return 1;
}

char *compile()
{
    const char *code = "void func1()\n{\nmyprintfWithoutCanshu(\"helloworld\");\nmyprintfWithoutCanshu(\"这是第二条语句\");\n}\n";
    char s[10][100]; // 最多10个printf
    int scount = 0;
    int len = 0; // 统计长度

    while (code[len] != '\0')
    {
        len++;
    }
    printf("code的长度为%d\n", len);
    for (int i = 0; i < len; i++)
    {
        char temp = code[i];
        if (i < len - 21 && if_equal_to_myprintfWithoutCanshu(&code[i]))
        {
            i += 22;
            int slen = 0;
            while (code[i] != ')')
            {
                s[scount][slen] = code[i];
                slen++;
                i++;
            };
            s[scount][slen] = '\0';
            // printf("%s\n", s[scount]);
            scount++;
        }
    };
    printf("\n\n.extern  myprintfWithoutCanshu\n.global func1\n");
    printf(".data\n");
    for (int i = 0; i < scount; i++)
    {
        printf("str%d :.string %s\n", i, s[i]);
    };
    printf(".text\nfunc1 :\n");
    for (int i = 0; i < scount; i++)
    {
        printf("la a0,str%d\n", i);
        ;
        printf("call myprintfWithoutCanshu\n");
        ;
    };
    printf("ret\n.end\n");
}
// .extern  myprintfWithoutCanshu
// .global func1

// .data
//     display_str :.string "Hello, RVO323S!\n"
//     display_str2 :.string "Hello, RVO323S!\n"

// .text
// func1 :
//     la a0,display_str
//     call myprintfWithoutCanshu

//     ret
