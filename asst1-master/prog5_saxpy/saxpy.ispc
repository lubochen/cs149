
export void saxpy_ispc(uniform int N,
                       uniform float scale,
                            uniform float X[],
                            uniform float Y[],
                            uniform float result[])
{
	result[0]=scale*X[0]+Y[0];
    foreach (i = 1 ... N) {           
        result[i] =  scale * X[i] + Y[i];
    }
}

task void saxpy_ispc_task(uniform int N,
                               uniform int span,
                               uniform float scale,
                               uniform float X[], 
                               uniform float Y[],
                               uniform float result[],
                               uniform int temp
                               )
{

    uniform int indexStart = taskIndex * span;
    uniform int indexEnd = min(N, indexStart + span);
    
 //   foreach(i=0...span){
 //   	result[i*temp+taskIndex]=scale*X[i*temp+taskIndex]+Y[i*temp+taskIndex]; 
 //   }
    
    foreach (i = indexStart ... indexEnd) {
        result[i] = scale * X[i] + Y[i];
    }
}

export void saxpy_ispc_withtasks(uniform int N,
                               uniform float scale,
                               uniform float X[],
                               uniform float Y[],
                               uniform float result[])
{

    uniform int span = N / 64;  // 64 tasks
    launch[N/span] saxpy_ispc_task(N, span, scale, X, Y, result,N/span);
}
