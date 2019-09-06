#include <QDebug>
#include <pthread.h>
#include <unistd.h>

pthread_cond_t cond;
pthread_mutex_t mutex;
pthread_mutex_t counter_mutex;

/*
 * Ready is the boolean to check on condition.
 * This is necessary since the call the pthread_cond_wait
 * can return spuriously. See "man pthread_cond_wait" page.
 */
bool ready = false;
int thread_count = 0;

void *bidon(void *arg)
{
    (void) arg;
    bool incremented = false;
    pthread_mutex_lock(&mutex);
    do {
        if (!incremented) {
            thread_count++;
            incremented = true;
        }
        pthread_cond_wait(&cond, &mutex);
    } while (ready == false);
    pthread_mutex_unlock(&mutex);
    return 0;
}

int main(int argc, char *argv[])
{
    (void) argc; (void) argv;

    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    QVector<pthread_t> threads;

    for (;;) {
        pthread_t t;
        int ret = pthread_create(&t, NULL, bidon, NULL);
        if (ret == 0) {
            threads.append(t);
        } else {
            qDebug() << "max thread" << threads.size();
            break;
        }
    }

    /*
     * Wait for all thread to wait on the condition.
     * Otherwise, we might miss some.
     * Protecting thread_count with the mutex ensure
     * that the count will be only valid when all threads
     * incremented the value.
     */
    do {
        pthread_mutex_lock(&mutex);
        if (thread_count == threads.size()) {
            pthread_mutex_unlock(&mutex);
            break;
        }
        pthread_mutex_unlock(&mutex);
        usleep(1);
    } while (true);

    pthread_mutex_lock(&mutex);
    ready = true;
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);
    foreach (pthread_t thread, threads) {
        pthread_join(thread, NULL);
    }
    return 0;
}
