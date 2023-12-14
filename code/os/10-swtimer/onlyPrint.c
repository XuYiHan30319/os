#include "os.h"

char *compile()
{
    const char code[1024] = "int main()\n{\nprintf(\"helloworld\");\n}\n";
    printf("%s", code);
}