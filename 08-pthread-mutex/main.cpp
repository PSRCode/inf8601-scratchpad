#include <QDebug>
#include <pthread.h>

class Temperature {
public:
    Temperature(int i) : m_temp(i) {
        pthread_mutexattr_t attr;
        pthread_mutexattr_init(&attr);
        pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
        pthread_mutex_init(&m_mutex, &attr);
        pthread_mutexattr_destroy(&attr);
    }

    void set(int temp) {
        pthread_mutex_lock(&m_mutex);
        m_temp = temp;
        pthread_mutex_unlock(&m_mutex);
    }

    int get() {
        pthread_mutex_lock(&m_mutex);
        int current_temp = m_temp;
        /*
         * A second lock on the same mutex is only possible due to the use of
         * PTHREAD_MUTEX_RECURSIVE. Otherwise the default type (NORMAL) will
         * deadlock on consecutive lock of the same mutex.
         */
        pthread_mutex_lock(&m_mutex);
        return current_temp;
    }

    void increment() {
        pthread_mutex_lock(&m_mutex);
        int new_temp = m_temp + 1;
        set(new_temp);
        pthread_mutex_unlock(&m_mutex);
    }

private:
    int m_temp;
    pthread_mutex_t m_mutex;
};

int main(int argc, char *argv[])
{
    (void) argc;
    (void) argv;

    Temperature t(42);
    qDebug() << "temperature before" << t.get();
    t.increment();
    qDebug() << "temperature after " << t.get();
}
