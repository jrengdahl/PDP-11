#ifndef THREADS11_H
#define THREADS11_H

typedef int *PORT;
typedef int THREADFUNCTION(int, int);

// a stack must be declared as an array of int

int wait(PORT *port);
int wake(PORT *port, int arg);
void spawn(THREADFUNCTION *code, int *stack_end, ...);

#define THREADSTACK(stack) (&stack[sizeof(stack)/2])

#define rendezvous_test(stack)          stack[sizeof(stack)/2-1]
#define rendezvous_return_value(stack)  stack[sizeof(stack)/2-2]


#endif // THREADS11_H
