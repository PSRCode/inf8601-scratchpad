#include <QDebug>
#include <pthread.h>

void *routine(void *arg)
{
    (void) arg;

    qDebug() << "hello!" << arg << *((int *)arg);
    return (void *) 123;
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    pthread_t thread;
    pthread_attr_t attr;
    int arg = 42;
    int ret;

    /*
     * attr must be initialized.
     * The pthread API uses opaque objects. Always use init, getter,setter
     * functions.
     */
    pthread_attr_init(&attr);
    int err = pthread_create(&thread, &attr, routine, &arg);
    qDebug() << "pthread_create=" << err;

    err = pthread_join(thread, (void **)&ret);
    qDebug() << "pthread_join=" << err;
    qDebug() << "pthread_join retval=" << ret;

    return 0;
}
