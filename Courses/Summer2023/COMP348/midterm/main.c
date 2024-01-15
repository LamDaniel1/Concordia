// Q2:

// #include<stdio.h> 

// int main() {
//     q1();
    
// }

// void q1() {
//     char c[] = "GEEK2018";
//     char *p =c;
//     printf("%c,%c", *p,*(p+p[3]-p[1])); 
// }

// Q3:

// void q2(int *p, int * const q) 
// { 
//   p = q; 
//   *p = 2;
// } 

// int i = 0, j = 1; 

// int main() 
// { 
//   q2(&i, &j); 
//   printf("%d %d", i, j); 
//   return 0; 
// }

// Q4:

// # include <stdio.h>
// void fun(int *ptr)
// {
//     ptr++;
// }
 
// int main()
// {
//   int y[] = {10, 20, 30};
//   fun(y);
//   printf("%d", *(y+1));
//   return 0;
// }

// Q6:

// #include<stdio.h>
// int main()
// {
//     int arr[] = {10, 20, 30, 40, 50, 60};
//     int *ptr1 = arr;
//     int *ptr2 = arr + 5;
//     printf("Difference: %d.", (ptr2 - ptr1));
//     printf("In bytes: %d",  (char*)ptr2 - (char*) ptr1);
//     return 0;
// }

// Q7: 

// #include<stdio.h>
// int main()
// {
//   char *ptr = "helloworld";
//   char *ptr2 = "string 2";
//   printf(ptr + 1);
//   return 0;
// }

// Q8:

// #include <stdio.h>
// void fun(int x) { 
//     x = 30; 
// }

// int main()
// {
//   int y = 20;
//   fun(y);
//   printf("%d", y);
//   return 0;
// }

// Q9:

// #include <stdio.h>
// void main()
// {
//     char s[] = "2s"; // "%s"
//     char s2[] = "s2"; // "%2"
//     *s = '%';
//     *s2 = '%';
//     printf(s, s2); // printf("%s", "%2");
//     // printf(s2, s);
// }

// Q10:

// #include <stdio.h>

// IN C, 0 = FALSE, anything that is not 0 = TRUE

// const char* strchr(const char* s, int c) {
//     for(; *s; s++) // for (nothing initialized, 1 )
//         if(*s == c) return s;
//     if(*s == c) return s;
//     return NULL;
// }
// const char* nvl(const char* s) {
//     return s == NULL? "NULL": s;
// }

// int main() {
//     printf("|%s|", nvl(strchr("123", 0)));
//     return 0;
// }

// FALL 2022 - PAST MIDTERM QUESTIONS:

// Q1:

// void fun(int* ptr) {
//     --ptr;
// }

// main() {
//     int y[] = {1,2,3};
//     printf("%p\n", y);
//     fun(&y[1]);
//     printf("%p\n", y);
//     printf("%d", *(y+1));
//     return 0;
// }


// Q2:

// int main() {
//     static float x[100];
//     printf("Enter a number: ", x);
//     scanf("%d", x);
//     printf("Your number is %d", *x); // ANS: 7 - Reads 7 and encounters a space so x = 7
// }

// SIDE NOTE:

// #include <stdio.h>

// int main() {
//     printf("%d", NULL); // Prints 0 cause NULL = ((void *)0) so NULL = 0 if read as number
// }

// Q3:

// #include <stdlib.h>
// #include <string.h>
// #include <stdio.h>

// int main() {
//     char* p= malloc(10);
//     if (!p) {
//         exit(0);
//     }
//     strcpy(p, "1234");
//     p = realloc(p, 100);
//     if (!p) {
//         exit(0);
//     }
//     printf("%c", *p); // PRINTS 1: "%c" prints one char and memory is allocated properly in malloc and realloc allocates more than enough memory

// }

// Q4: 
