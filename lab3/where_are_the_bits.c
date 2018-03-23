// where_are_the_bits.c ... determine bit-field order
// COMP1521 Lab 03 Exercise
// Written by ...

#include <stdio.h>
#include <stdlib.h>

struct _bit_fields {
   unsigned int a : 4,
                b : 8,
                c : 20;
};

int main(void)
{
   struct _bit_fields x;
   
   printf("%ul\n",sizeof(x));
   unsigned int *ptr;

   x.a = 0;
   x.b = 0;
   x.c = 0;

   printf("Before:\n");

   printf("x.a is %d\n",x.a);
   printf("x.b is %d\n",x.b);
   printf("x.c is %d\n",x.c);

   ptr = (unsigned int *)&x;
   (*ptr)++;

   printf("After:\n");

   printf("x.a is %d\n",x.a);
   printf("x.b is %d\n",x.b);
   printf("x.c is %d\n",x.c);

   if (x.a == 1) {
      printf("This is the second order CBA\n");
   } else if (x.c == 1) {
      printf("This is the first order ABC\n");
   }

   return 0;
}
