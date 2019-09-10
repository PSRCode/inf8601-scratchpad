#include <QDebug>
#include <pthread.h>
#include <unistd.h>

#include <vector>

pthread_cond_t cond;
pthread_mutex_t mutex;
pthread_mutex_t counter_mutex;

/*
 * Ready is the boolean to check on condition.
 * This is necessary since the call the pthread_cond_wait
 * can return spuriously. See "man pthread_cond_wait" page.
 */
bool ready = false;

/*
 * The thread count here is used as a "barrier" ensuring that all threads are
 * considered ready to receive a signal. The "mutex" protects the thread count.
 */
unsigned int thread_count = 0;

void *bidon(void *arg)
{
    (void) arg;

    pthread_mutex_lock(&mutex);

    /*
     * Increment the thread count while mutex is held. This ensure that the
     * principal thread only see valid thread count.
     */
    thread_count++;

    while (ready == false) {
        /*
         * Per "man pthread_cond_wait":
         *  When using condition variables there is always a Boolean predicate
         *  involving shared  variables associated  with each condition wait
         *  that is true if the thread should proceed. Spurious wake‐ ups from
         *  the pthread_cond_timedwait() or pthread_cond_wait() functions may
         *  occur.  Since  the return  from pthread_cond_timedwait() or
         *  pthread_cond_wait() does not imply anything about the value of this
         *  predicate, the predicate should be re-evaluated upon such return.
         * This is why we use the "ready" boolean check here.
         */
        pthread_cond_wait(&cond, &mutex);
    }
    pthread_mutex_unlock(&mutex);
    return 0;
}

int main()
{
    pthread_cond_init(&cond, NULL);
    pthread_mutex_init(&mutex, NULL);

    /*
     * A dynamic container is needed here since we do not know the upper limit.
     * The upper limit is what we are looking for.
     */
    std::vector<pthread_t> threads = {};

    for (;;) {
        pthread_t t;
        int ret = pthread_create(&t, NULL, bidon, NULL);
        if (ret == 0) {
            threads.push_back(t);
        } else {
            qDebug() << "max thread" << threads.size();
            break;
        }
    }

    /*
     * Wait for all thread to wait on the condition. Otherwise, we might miss
     * some. Protecting thread_count with the mutex ensure that the count will
     * be only valid when all threads incremented the value.
     *
     * Other ways might be possible, feel free to contribute back to this
     * project.
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
    /* Wakeup all the waiting threads */
    pthread_cond_broadcast(&cond);
    pthread_mutex_unlock(&mutex);

    for (pthread_t thread : threads) {
        pthread_join(thread, NULL);
    }

    return 0;
}
