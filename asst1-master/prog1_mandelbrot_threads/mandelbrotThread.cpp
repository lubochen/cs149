#include <stdio.h>
#include <thread>

#include "CycleTimer.h"

typedef struct {
    float x0, x1;
    float y0, y1;
    unsigned int width;
    unsigned int height;
    int maxIterations;
    int* output;
    int threadId;
    int numThreads;
} WorkerArgs;


extern void mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int numRows,
    int maxIterations,
    int output[]);

extern void __mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int totalRows,
    int maxIterations,
    int output[],
    int numthread,
    int allthread
);
extern void ___mandelbrotSerial(
    float x0, float y0, float x1, float y1,
    int width, int height,
    int startRow, int totalRows,
    int maxIterations,
    int output[],
    int numthread,
    int allthread
);
//
// workerThreadStart --
//s
// Thread entrypoint.
void _workerThreadStart(WorkerArgs * const args) {

    // TODO FOR CS149 STUDENTS: Implement the body of the worker
    // thread here. Each thread should make a call to mandelbrotSerial()
    // to compute a part of the output image.  For example, in a
    // program that uses two threads, thread 0 could compute the top
    // half of the image and thread 1 could compute the bottom half.
    __mandelbrotSerial(       //每个子线程调用单线程函数
    args->x0,args->y0,args->x1,args->y1    //绘制范围   
    ,args->width,args->height    //宽、高
    ,0//args->height*args->threadId/args->numThreads   //该线程开始的高度
    ,args->height//args->height / args->numThreads //每个线程负责的高度
    ,args->maxIterations   //最多迭代次数(256)
    ,args->output     //存储数组
    ,args->threadId
    ,args->numThreads
    );

    //printf("Thread %d finished\n", args->threadId);
}
void workerThreadStart(WorkerArgs * const args) {
double startTime = CycleTimer::currentSeconds();
    mandelbrotSerial(       //每个子线程调用单线程函数
    args->x0,args->y0,args->x1,args->y1    //绘制范围   
    ,args->width,args->height    //宽、高
    ,args->height*args->threadId/args->numThreads   //该线程开始的高度
    ,args->height / args->numThreads //每个线程负责的高度
    ,args->maxIterations   //最多迭代次数(256)
    ,args->output     //存储数组
  
    );
double endTime = CycleTimer::currentSeconds();
double minSerial = endTime - startTime;
//printf("[mandelbrot %d]:\t\t[%.3f] ms\n", 0,minSerial * 1000);
    printf("Thread %d finished:\t\t[%.3f] ms\n", args->threadId,minSerial*1000);
}
//
// MandelbrotThread --
//
// Multi-threaded implementation of mandelbrot set image generation.
// Threads of execution are created by spawning std::threads.
void mandelbrotThread(
    int numThreads,
    float x0, float y0, float x1, float y1,
    int width, int height,
    int maxIterations, int output[])
{
    static constexpr int MAX_THREADS = 32;

    if (numThreads > MAX_THREADS)
    {
        fprintf(stderr, "Error: Max allowed threads is %d\n", MAX_THREADS);
        exit(1);
    }

    // Creates thread objects that do not yet represent a thread.
    std::thread workers[MAX_THREADS];
    WorkerArgs args[MAX_THREADS];

    for (int i=0; i<numThreads; i++) {
      
        // TODO FOR CS149 STUDENTS: You may or may not wish to modify
        // the per-thread arguments here.  The code below copies the
        // same arguments for each thread
        args[i].x0 = x0;
        args[i].y0 = y0;
        args[i].x1 = x1;
        args[i].y1 = y1;
        args[i].width = width;
        args[i].height = height;
        args[i].maxIterations = maxIterations;
        args[i].numThreads = numThreads;
        
        args[i].output = output;
        args[i].threadId = i;
    }

    
    // Spawn the worker threads.  Note that only numThreads-1 std::threads
    // are created and the main application thread is used as a worker
    // as well.
    for (int i=1; i<numThreads; i++) {
    	workers[i] = std::thread(_workerThreadStart, &args[i]);
    }
	_workerThreadStart(&args[0]);
    // join worker threads
    for (int i=1; i<numThreads; i++) {
        workers[i].join();
    }
}

