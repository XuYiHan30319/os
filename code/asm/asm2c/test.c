/*
 * a0 --> a
 * a1 --> b
 * c  <-- a0
 */
int foo(int a, int b)
{
	int c = a + b;
	return c;
}

int foo1(int a, int b)
{
	int c;
	asm volatile(
		"add %[sum],%[add1],%[add2]"
		: [sum] "=r"(c)
		: [add1] "r"(a), [add2] "r"(b));
	return c;
}

int foo2(int a, int b)
{
	int c;
	asm volatile(
		"add %0,%1,%2"
		: "=r"(c)
		: "r"(a), "r"(b));
}

int main()
{
	foo1(3, 4);
}