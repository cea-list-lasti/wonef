#include <cstdlib>
#include <sys/time.h>

class Timer
{
    timeval timer[2];

  public:

    Timer() { start(); }

    timeval start()
    {
        gettimeofday(&this->timer[0], NULL);
        return this->timer[0];
    }

    timeval stop()
    {
        gettimeofday(&this->timer[1], NULL);
        return this->timer[1];
    }

    float duration()
    {
        stop();
        int secs(this->timer[1].tv_sec - this->timer[0].tv_sec);
        int usecs(this->timer[1].tv_usec - this->timer[0].tv_usec);

        if(usecs < 0)
        {
            --secs;
            usecs += 1000000;
        }

        start();

        // simpler than using std::setprecision() for our purpose
        return static_cast<float>(1.0 * secs + 1.0 * (usecs / 10000) / 100.0);
    }
};
