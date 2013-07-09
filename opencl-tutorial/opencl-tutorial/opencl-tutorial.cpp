// opencl-tutorial.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <CL\opencl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#define ARRAY_SIZE 10000

int main()
{
	cl_int errNum;
	cl_platform_id firstPlatformId;
	cl_uint numPlatforms;
	cl_context context;
	
	errNum = clGetPlatformIDs( 1, &firstPlatformId, &numPlatforms );

	if( errNum != CL_SUCCESS || numPlatforms <= 0 )
	{
        std::cerr << "Failed to find any OpenCL platform." << std::endl;
        return -1;
	}

	std::cout << "There are " << numPlatforms << " platforms on the system." << std::endl;

	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)firstPlatformId,
        0
	};

	context = clCreateContextFromType( contextProperties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &errNum );

	if( errNum != CL_SUCCESS )
	{
        std::cout << "Could not create GPU contest, tying CPU...";
		context = clCreateContextFromType( contextProperties, CL_DEVICE_TYPE_CPU, nullptr, nullptr, &errNum );
		if( errNum != CL_SUCCESS )
		{
            std::cout << "FAILED" << std::endl;
            return -1;
		}
		else
		{
            std::cout << "DONE" << std::endl;
		}
	}

    size_t devicesBufferSize = -1;

	errNum = clGetContextInfo( context, CL_CONTEXT_DEVICES, 0, nullptr, &devicesBufferSize );

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Failed to get device IDs";
        clReleaseContext( context );
        return -1;
	}

	if( devicesBufferSize <= 0 )
	{
        std::cerr << "No device is available.";
        clReleaseContext( context );
        return -1;
	}

	cl_device_id* devices = new cl_device_id[devicesBufferSize/sizeof(cl_device_id)];

	errNum = clGetContextInfo( context, CL_CONTEXT_DEVICES, devicesBufferSize, devices, nullptr );

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Failed to get device IDs";
        clReleaseContext( context );
        return -1;
	}

	cl_command_queue commandQueue = clCreateCommandQueue( context, devices[0], 0, nullptr );

    if( commandQueue == nullptr )
	{
        std::cerr << "Failed to create command queue for device 0.";
        clReleaseContext( context );
        return -1;
	}

	cl_device_id device = devices[0];

    delete [] devices;

	std::ifstream kernelFile( "HelloWorld.cl", std::ios::in );

	if( !kernelFile.is_open() )
	{
        std::cerr << "Failed to open file for reading." << std::endl;
        clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

	std::ostringstream oss;
	oss << kernelFile.rdbuf();

    std::string srcStdString = oss.str();
	const char *srcString = srcStdString.c_str();

	cl_program program = clCreateProgramWithSource( context, 1, (const char**)&srcString, nullptr, nullptr );

    if( program == nullptr )
	{
        std::cerr << "Failed to create CL program from source." << std::endl;
        clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

	errNum = clBuildProgram( program, 0, nullptr, nullptr, nullptr, nullptr );

	if( errNum != CL_SUCCESS )
	{
        char buildLog[16384];
		clGetProgramBuildInfo( program, device, CL_PROGRAM_BUILD_LOG, sizeof( buildLog ), buildLog, nullptr );

        std::cerr << "Failed to buid program" << std::endl;
        std::cerr << buildLog;
		clReleaseProgram( program );
        clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

    float result[ARRAY_SIZE];
    float a[ARRAY_SIZE];
    float b[ARRAY_SIZE];

	for( unsigned int i = 0; i < ARRAY_SIZE; ++i )
	{
        a[i] = i;
        b[i] = i*2;
	}

	cl_mem memObjects[3] = {0,0,0};

	memObjects[0] = clCreateBuffer( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, a, nullptr );
    memObjects[1] = clCreateBuffer( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, sizeof(float) * ARRAY_SIZE, b, nullptr );
	memObjects[2] = clCreateBuffer( context, CL_MEM_READ_WRITE, sizeof( float ) * ARRAY_SIZE, nullptr, nullptr );

	if( memObjects[0] == nullptr || memObjects[1] == nullptr || memObjects[2] == nullptr )
	{
        std::cerr << "Error creating memory objects." << std::endl;
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

    cl_kernel kernel = clCreateKernel( program, "hello_kernel", nullptr );
    if( kernel == nullptr )
	{
        std::cerr << "Error creating kernel." << std::endl;
        for( unsigned int i = 0; i < 3; ++i )
	    {
		    clReleaseMemObject( memObjects[i] );
	    }
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
	    return 0;

	}

	errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &memObjects[0] );
    errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &memObjects[1] );
    errNum |= clSetKernelArg( kernel, 2, sizeof(cl_mem), &memObjects[2] );

	if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error setting kernel arguments." << std::endl;
        clReleaseKernel( kernel );
        for( unsigned int i = 0; i < 3; ++i )
	    {
		    clReleaseMemObject( memObjects[i] );
	    }
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}


	size_t globaldWorkSize[1] = {ARRAY_SIZE};
	size_t localWorkSize[1] = {1};

	errNum = clEnqueueNDRangeKernel( commandQueue, kernel, 1, nullptr, globaldWorkSize, localWorkSize, 0, nullptr, nullptr );

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error queuing kernel for execution. " << std::endl;
        clReleaseKernel( kernel );
        for( unsigned int i = 0; i < 3; ++i )
	    {
		    clReleaseMemObject( memObjects[i] );
	    }
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

	errNum = clEnqueueReadBuffer( commandQueue, memObjects[2], CL_TRUE, 0, ARRAY_SIZE * sizeof( float ), result, 0, nullptr, nullptr );

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error reading result buffer. " << std::endl;
        clReleaseKernel( kernel );
        for( unsigned int i = 0; i < 3; ++i )
	    {
		    clReleaseMemObject( memObjects[i] );
	    }
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
        return -1;
	}

	for( unsigned int i = 0; i < ARRAY_SIZE; ++i )
	{
        std::cout << result[i] << " ";
	}
    std::cout << std::endl;
    std::cout << "Program finished" << std::endl


    

	clReleaseKernel( kernel );
    for( unsigned int i = 0; i < 3; ++i )
	{
		clReleaseMemObject( memObjects[i] );
	}
    clReleaseProgram( program );
	clReleaseCommandQueue( commandQueue );
	clReleaseContext( context );
    

	return 0;
}

