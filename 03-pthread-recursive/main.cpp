#include <QDebug>
#include <pthread.h>

void *thread_bar(void *arg)
{
    (void) arg;
    qDebug() << "bar  bar";
    return 0;
}

void *thread_foo(void *arg)
{
    (void) arg;
    pthread_t bar;

    qDebug() << "foo  before";

    pthread_create(&bar, NULL, thread_bar, NULL);
    pthread_join(bar, NULL);

    qDebug() << "foo  after";
    return 0;
}

int main(void)
{

    pthread_t foo;

    qDebug() << "main begin";
    pthread_create(&foo, NULL, thread_foo, NULL);
    pthread_join(foo, NULL);
    qDebug() << "main done";
    return 0;
}
