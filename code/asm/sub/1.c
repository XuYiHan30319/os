int a = 0x87654321;

int main()
{
    int b = a & 0x0000FFFF, c = (a >> 16) & 0x0000FFFF;
}