// g++ --std=c++14 -I. -o life -O3 life.cpp # or clang++
#include <life.h>
#include <random>
#include <iostream>
#include <chrono>
#include <thread>
using namespace std;

// 1) To update at a given position, you only need to know the total neighbourhood
//    count (including the present position, but independent of its value):
//    0-2:0; 3:1; 4:=; 5-9:0
// 2) To obtain the sum of the values in a 3x3 neighbourhood of each pixel, you only
//    have to perform 4 additions and two copies.

shared_ptr<Matrix> neighbourhood_count(const Matrix& world)
{
    Matrix vert_count(world); // deep copy
    vert_count.add_rowshift(world, -1);
    vert_count.add_rowshift(world, 1);
    shared_ptr<Matrix> count = make_shared<Matrix>(vert_count);
    count->add_colshift(vert_count, -1);
    count->add_colshift(vert_count, 1);
    return count;
}

void update(Matrix* world)
{
    auto count = neighbourhood_count(*world);
    transform(begin(world->data()), end(world->data()),
              begin(count->data()), begin(world->data()),
              // [](uint8_t w, uint8_t c){return (c == 3 ? 1 : (c == 4 ? w : 0));});
              [](uint8_t w, uint8_t c){return uint8_t(c == 3 or (c == 4 and w));});
}

void print(const Matrix& m)
{
    uint32_t R = m.getRows();
    uint32_t C = m.getCols();
    for (int r = 0; r < R; r++)
    {
        for (int c = 0; c < C; c++)
        {
            putchar(m.get(r,c) ? 'O' : ' ');
        }
        if (r != R - 1)
        {
            putchar('\n');
        }
    }
    cout << flush;
}

void init(Matrix* m, double fraction, int seed)
{
    default_random_engine generator;
    bernoulli_distribution dist(fraction);
    generator.seed(seed);
    auto rnd = bind(dist, generator);
    generate(begin(m->data()), end(m->data()), rnd);
}

#include <cstdlib>
#include <cstdio>
#include <vector>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <linux/fb.h>
#include <sys/mman.h>

int main_txt(int generations, int delay, double fraction, int seed)
{
    struct winsize size;
    ioctl(STDOUT_FILENO,TIOCGWINSZ,&size);

    Matrix m(size.ws_row, size.ws_col);
    init(&m, fraction, seed);
    print(m);

    for (int i = 0; i < generations; i++)
    {
        this_thread::sleep_for(std::chrono::milliseconds(delay));
        update(&m);
        print(m);
    }
    return 0;
}

// fbfd is the open (R/W) file descriptor of the framebuffer
int main_fb(int fbfd, int generations, int delay, double fraction, int seed)
{
    // Get variable screen information
    struct fb_var_screeninfo vinfo;
    if (ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        exit(3);
    }
    
    if (vinfo.bits_per_pixel != 32) {
        exit(5);
    }

    Matrix m(vinfo.yres, vinfo.xres);
    init(&m, fraction, seed);

    long screensize = vinfo.xres * vinfo.yres * 4;

    // Map the device to memory
    uint32_t* fbp = (uint32_t*)mmap(0, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    if (long(fbp) == -1) {
        exit(4);
    }

    uint32_t val = 0x0000ff00; // aarrggbb, assuming bits_per_pixel == 32

    // write matrix to framebuffer
    auto setter = [&val](auto x){return val * x;}; // val * x; x ? val : 0; {}
    transform(begin(m.data()), end(m.data()), fbp, setter);
    
    for (int i = 0; i < generations; i++)
    {
        this_thread::sleep_for(chrono::milliseconds(delay));
        update(&m);
        transform(begin(m.data()), end(m.data()), fbp, setter);
        msync(fbp, screensize, MS_SYNC);
    }
        
    // cleanup
    munmap(fbp, screensize);
    close(fbfd);
    return 0;
}


int main(int argc, char* argv[])
{
    if (argc > 1 and argv[1][0] == '?')
    {
        printf("Usage: "
               "%s <generations> <delay (ms)> <init fraction> <seed> <prefer_txt>\n",
               argv[0]);
        exit(0);
    }
    
    int generations = (argc > 1) ? atoi(argv[1]) : 10;
    int delay = (argc > 2) ? atoi(argv[2]) : 200;
    double fraction = (argc > 3) ? atof(argv[3]) : 0.3;
    int seed = (argc > 4) ? atoi(argv[4]) : 0;
    bool prefer_txt = (argc > 5) ? bool(atoi(argv[5])) : false;
    
    if (not prefer_txt)
    {
        // Try to open the framebuffer for reading and writing
        int fbfd = open("/dev/fb0", O_RDWR);
        if (fbfd != -1) {
            return main_fb(fbfd, generations, delay, fraction, seed);
        }
    }
    return main_txt(generations, delay, fraction, seed);
}

