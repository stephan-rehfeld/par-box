
// opencl-tutorial.cpp : Definiert den Einstiegspunkt für die Konsolenanwendung.
//

#include <CL\opencl.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <FreeImage.h>

size_t RoundUp(int groupSize, int globalSize)
{
    int r = globalSize % groupSize;
    if(r == 0)
    {
        return globalSize;
    }
    else
    {
        return globalSize + groupSize - r;
    }
}

int main()
{
	cl_int errNum;
	cl_platform_id* platformIds;
	cl_uint numPlatforms;
	cl_context context;

	char* imageName = "ant.png";

	FreeImage_Initialise();
	
	errNum = clGetPlatformIDs( 0, nullptr, &numPlatforms );
	if( errNum != CL_SUCCESS || numPlatforms <= 0 ) 
	{
		std::cerr << "Failed to find any OpenCL platform." << std::endl;
		return -1;
	}

	platformIds = (cl_platform_id*)alloca( sizeof( cl_platform_id) * numPlatforms );

	errNum = clGetPlatformIDs( numPlatforms, platformIds, nullptr );
	if( errNum != CL_SUCCESS  )
	{
        std::cerr << "Failed to find any OpenCL platform." << std::endl;
        return -1;
	}

	std::cout << "There are " << numPlatforms << " platforms on the system." << std::endl;

	for( size_t i = 0; i < numPlatforms; ++i )
	{
		size_t nameSize = 0;
		errNum = clGetPlatformInfo( platformIds[i], CL_PLATFORM_NAME, 0, nullptr, &nameSize );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request the name of the platform." << std::endl;
			return -1;
		}
		char* name = (char*)alloca( sizeof( char ) * nameSize );

		errNum = clGetPlatformInfo( platformIds[i], CL_PLATFORM_NAME, nameSize, name, nullptr );

		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request the name of the platform." << std::endl;
			return -1;
		}

		size_t versionSize = 0;

		errNum = clGetPlatformInfo( platformIds[i], CL_PLATFORM_VERSION, 0, nullptr, &versionSize );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request the version of the platform." << std::endl;
			return -1;

		}

		char* version = (char*)alloca( sizeof(char) * versionSize );
		errNum = clGetPlatformInfo( platformIds[i], CL_PLATFORM_VERSION, versionSize, version, nullptr );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request the version of the platform." << std::endl;
			return -1;

		}

		std::cout << " [" << i << "] " << name << " with version: " << version << std::endl;		
	}

	size_t choosenPlatform;
	std::cin >> choosenPlatform;

	cl_uint numDevices = 0;

	errNum = clGetDeviceIDs( platformIds[ choosenPlatform ], CL_DEVICE_TYPE_ALL, 0, nullptr, &numDevices ); 
	if( errNum != CL_SUCCESS )
	{
		std::cerr << "Failed to request devices of the platform." << std::endl;
		return -1;

	}

	cl_device_id* deviceIds = (cl_device_id*)alloca( sizeof( cl_device_id ) * numDevices );

	errNum = clGetDeviceIDs( platformIds[ choosenPlatform ], CL_DEVICE_TYPE_ALL, numDevices, deviceIds, nullptr );

	std::cout << "The platform has " << numDevices << " devices." << std::endl;

	for( size_t i = 0; i < numDevices; ++i )
	{
		cl_device_type deviceType;

		errNum = clGetDeviceInfo( deviceIds[i], CL_DEVICE_TYPE, sizeof( cl_device_type ), &deviceType, nullptr );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request device type." << std::endl;
			return -1;
		}

		char* type;
		if( deviceType == CL_DEVICE_TYPE_CPU ) {
			type = "CPU";
		} else if( deviceType == CL_DEVICE_TYPE_GPU ) {
			type = "GPU";
		} else if( deviceType == CL_DEVICE_TYPE_ACCELERATOR ) {
			type = "ACCELERATOR";
		} if( deviceType == CL_DEVICE_TYPE_DEFAULT ) {
			type = "DEFAULT";
		}  

		size_t nameSize = 0;
		clGetDeviceInfo( deviceIds[i], CL_DEVICE_NAME, 0, nullptr, &nameSize );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request device name." << std::endl;
			return -1;
		}

		char* name = (char*)alloca( sizeof( char ) * nameSize );
		clGetDeviceInfo( deviceIds[i], CL_DEVICE_NAME, nameSize, name, nullptr );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request device name." << std::endl;
			return -1;
		}

		cl_bool imageSupport;
		errNum = clGetDeviceInfo( deviceIds[i], CL_DEVICE_IMAGE_SUPPORT, sizeof( cl_bool ), &imageSupport, nullptr );
		if( errNum != CL_SUCCESS )
		{
			std::cerr << "Failed to request image support." << std::endl;
			return -1;
		}

		char* imageSupportString;
		if( imageSupport ) {
			imageSupportString = "with image support";
		}
		else
		{
			imageSupportString = "without image support";
		}

		std::cout << "[" << i << "] " << type << ": " << name << " " << imageSupportString <<  std::endl;
	}

	size_t choosenDevice;
	std::cin >> choosenDevice;

	cl_context_properties contextProperties[] =
	{
		CL_CONTEXT_PLATFORM,
		(cl_context_properties)platformIds[choosenPlatform],
        0
	};

	context = clCreateContextFromType( contextProperties, CL_DEVICE_TYPE_GPU, nullptr, nullptr, &errNum );

	if( errNum != CL_SUCCESS )
	{
        std::cout << "Could not create GPU context, tying CPU...";
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

	std::ifstream kernelFile( "guassian_filter.cl", std::ios::in );

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

	FREE_IMAGE_FORMAT format = FreeImage_GetFileType( imageName );
	FIBITMAP* image = FreeImage_Load( format, imageName );

	FIBITMAP* temp = image;
	image = FreeImage_ConvertTo32Bits( image );
	FreeImage_Unload( temp );

	int width = FreeImage_GetWidth( image );
	int height = FreeImage_GetHeight( image );

	char *buffer = new char[width*height*4];
	memcpy(buffer, FreeImage_GetBits( image ), width * height * 4 );

	FreeImage_Unload( image );

	cl_image_format clImageFormat;
	
	clImageFormat.image_channel_order = CL_RGBA;
	clImageFormat.image_channel_data_type = CL_UNORM_INT8;

	cl_mem clSrcImage = clCreateImage2D( context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, &clImageFormat, width, height, 0, buffer, &errNum );
	if( errNum != CL_SUCCESS )
	{
		std::cerr << "Failed to create source image." << std::endl;
	}

	cl_mem clDstImage = clCreateImage2D( context, CL_MEM_WRITE_ONLY, &clImageFormat, width, height, 0, nullptr, &errNum );
	if( errNum != CL_SUCCESS )
	{
		std::cerr << "Failed to create destination image." << std::endl;
	}

	cl_sampler sampler = clCreateSampler( context, CL_FALSE, CL_ADDRESS_CLAMP_TO_EDGE, CL_FILTER_NEAREST, &errNum );
	if( errNum != CL_SUCCESS )
	{
		std::cerr << "Failed to create sampler." << std::endl;
	}

	

    cl_kernel kernel = clCreateKernel( program, "gaussian_blur", nullptr );
    if( kernel == nullptr )
	{
        std::cerr << "Error creating kernel." << std::endl;

        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
		clReleaseMemObject( clSrcImage );
		clReleaseMemObject( clDstImage );
		clReleaseSampler( sampler );
	    return 0;

	}

	errNum = clSetKernelArg( kernel, 0, sizeof(cl_mem), &clSrcImage );
	errNum |= clSetKernelArg( kernel, 1, sizeof(cl_mem), &clDstImage );
	errNum |= clSetKernelArg( kernel, 2, sizeof(cl_sampler), &sampler );
	errNum |= clSetKernelArg( kernel, 3, sizeof(cl_int), &width );
	errNum |= clSetKernelArg( kernel, 4, sizeof(cl_int), &height );

	if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error setting kernel arguments." << std::endl;
        clReleaseKernel( kernel );
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
		clReleaseMemObject( clSrcImage );
		clReleaseMemObject( clDstImage );
		clReleaseSampler( sampler );
        return -1;
	}


	size_t localWorkSize[2] = { 16, 16 };
    size_t globalWorkSize[2] =  { RoundUp(localWorkSize[0], width),
                                  RoundUp(localWorkSize[1], height) };


	errNum = clEnqueueNDRangeKernel( commandQueue, kernel, 2, nullptr, globalWorkSize, localWorkSize, 0, nullptr, nullptr );

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error queuing kernel for execution. " << std::endl;

        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
		clReleaseMemObject( clSrcImage );
		clReleaseMemObject( clDstImage );
		clReleaseSampler( sampler );
        return -1;
	}

	size_t origin[3] = { 0, 0, 0 };
	size_t region[3] = { width, height, 1 };

	errNum = clEnqueueReadImage( commandQueue, clDstImage, CL_TRUE, origin, region, 0, 0, buffer, 0, nullptr, nullptr ); 

    if( errNum != CL_SUCCESS )
	{
        std::cerr << "Error reading image back to buffer. " << std::endl;
        clReleaseKernel( kernel );
        clReleaseProgram( program );
	    clReleaseCommandQueue( commandQueue );
	    clReleaseContext( context );
		clReleaseMemObject( clSrcImage );
		clReleaseMemObject( clDstImage );
		clReleaseSampler( sampler );
        return -1;
}	

	format = FreeImage_GetFIFFromFilename( imageName );
	
	image = FreeImage_ConvertFromRawBits( (BYTE*)buffer, width, height, width*4, 32, 0xFF000000, 0x00FF0000, 0x0000FF00 );
	BOOL saved = FreeImage_Save( format, image, "blured.png" );
	if( !saved ) {
		std::cerr << "Image not saved" << std::endl;
	}

    std::cout << std::endl;
    std::cout << "Program finished" << std::endl;


    

	clReleaseKernel( kernel );

    clReleaseProgram( program );
	clReleaseCommandQueue( commandQueue );
	clReleaseContext( context );
	clReleaseMemObject( clSrcImage );
	clReleaseMemObject( clDstImage );
	clReleaseSampler( sampler );

	FreeImage_DeInitialise();
    

	return 0;
}

