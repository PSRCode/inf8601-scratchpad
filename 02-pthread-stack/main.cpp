#include <QDebug>
#include <pthread.h>

struct thread_arg {
    unsigned long rsp;
};

void *thread(void *args)
{
    register unsigned long rsp asm("rsp");
    struct thread_arg *arg = (struct thread_arg *) args;
    arg->rsp = rsp;
    return 0;
}

size_t get_defaul_stack_guard_size(void)
{
    size_t value;
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    (void) pthread_attr_getguardsize(&attr, &value);
    /*
     * No error checking here, given that you are on linux this should never
     * fail. See "man pthread_attr_getguardsize".
     */

    pthread_attr_destroy(&attr);
    return value;
}

int main(void)
{
    int n = 4;
    pthread_t t[n];
    struct thread_arg args[n];
    size_t stack_size = (1 << 15); /* 32KB */

    qDebug() << "Creating threads with a stack size of " << stack_size << " bytes";

    for (int i = 0; i < n; i++) {
        pthread_attr_t attr;

        pthread_attr_init(&attr);
        pthread_attr_setstacksize(&attr, stack_size);
        pthread_create(&t[i], &attr, thread, &args[i]);
        pthread_attr_destroy(&attr);
    }

    for (int i = 0; i < n; i++) {
        pthread_join(t[i], NULL);
        qDebug() << "Address of the stack of thread #"<< i << ":" << (void *)args[i].rsp;
    }

    for (int i = 0; i < n - 1; i++) {
        unsigned long space = args[i].rsp - args[i + 1].rsp;
        qDebug() << "Space between previous stack" << space << "bytes";
    }

    qDebug() << "The calculated stack size is bigger because of the stack guard.";
    qDebug() << "Default value of the stack guard on your system is:" << get_defaul_stack_guard_size() ;

    return 0;
}
