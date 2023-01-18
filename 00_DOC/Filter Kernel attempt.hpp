#pragma once

#include "Worker.hpp"
#define CL_TARGET_OPENCL_VERSION 300
#include <CL/cl.h>

#define __lw_min(a,b) ((a) < (b) ? (a) : (b))
#define __lw_max(a,b) ((a) > (b) ? (a) : (b))
#define __ldetail (2)

/*m_work_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos){
            int ym = __lw_min(pos[0] + __ldetail, m_temp_stor.size().height - 1);
            int xm = __lw_min(pos[1] + __ldetail, m_temp_stor.size().width - 1);
            cv::Vec3b &cmp = m_temp_stor.at<cv::Vec3b>(pos);

            for(int y = __lw_max(pos[0] - __ldetail, 0); y <= ym; y += __ldetail)
            {
                for(int x = __lw_max(pos[1] - __ldetail, 0); x <= xm; x += __ldetail)
                {
                    if(cmp != m_temp_stor.at<cv::Vec3b>(y, x))
                    {
                        pixel = cv::Vec3b{255,255,255};
                        return;
                    }
                }
            }
        });*/

const static char *PROGRAM =
    "#define __lw_min(a,b) ((a) < (b) ? (a) : (b))\n"
    "#define __lw_max(a,b) ((a) > (b) ? (a) : (b))\n"
    "#define __ldetail (2)\n"
    "__kernel void neighbors(__global uchar3* dat, __global uchar3* out){"
    "int id=get_global_id(0);"/*
    "int posx=id%" __x_str_(STREAM_WIDTH) ";"
    "int posy=id/" __x_str_(STREAM_WIDTH) ";"
    "int ym=__lw_min(posy+__ldetail," __x_str_(STREAM_HEIGHT) "-1);"
    "int xm=__lw_min(posx+__ldetail," __x_str_(STREAM_WIDTH) "-1);"
    "uchar3 cmp=dat[id];"
    "for(int y=__lw_max(posy-__ldetail,0);y<=ym;y+=__ldetail)"
    "for(int x=__lw_max(posx-__ldetail,0);x<=xm;x+=__ldetail){"
    "uchar3 tmp=dat[x+y*" __x_str_(STREAM_WIDTH) "];"
    "if(cmp.x!=tmp.x||cmp.y!=tmp.y||cmp.z!=tmp.z){"
    "out[id]=(uchar3)(255);return;}}"
    "out[id]=cmp;}"*/
    "out[id]=dat[id]+(uchar3)(1);}"
;

/**
 * @brief Project's Filter namespace
*/
namespace Filter {

/**
 * @class LineWorker : public Worker
 * @brief Temporary line colorization worker
*/
class LineWorker : public Worker
{
protected:
    cv::Mat m_temp_stor;
    cl_device_id device;
    cl_platform_id platform;
    cl_context context;
    cl_program program;
    cl_mem i_buff, o_buff;
    cl_kernel kernel;
    cl_command_queue queue;
    size_t global_size = 1024, local_size = 64;
    cl_int err;

public:
    explicit LineWorker(std::string id): Worker{std::move(id), MatIOType::VEC_4_CHAR_8, MatIOType::VEC_4_CHAR_8}
    {
        clog(info) << this->get_id() << " initialized!" << std::endl;
    }

    void start_up() override
    {
        if(clGetPlatformIDs(1, &platform, NULL) != CL_SUCCESS)
        {
            throw std::runtime_error("Could not identify a CL platform!");
        }
        if((err = clGetDeviceIDs(platform, CL_DEVICE_TYPE_GPU, 1, &device, NULL)) != CL_SUCCESS)
        {
            throw std::runtime_error("Could not get GPU device! Code: " + std::to_string(err));
        }

        //clGetDeviceInfo(device, CL_DEVICE_MAX_WORK_GROUP_SIZE, 8, &local_size, NULL);
        //local_size = global_size / local_size;
        //local_size = global_size / local_size;

        context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Context! Code: " + std::to_string(err));
        }

        program = clCreateProgramWithSource(context, 1, &PROGRAM, NULL, &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Program! Code: " + std::to_string(err));
        }

        if((err = clBuildProgram(program, 1, &device, NULL, NULL, NULL)) != CL_SUCCESS)
        {
            size_t lsz = 0;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                0, NULL, &lsz);
            char *log = new char[lsz + 1];
            log[lsz] = 0;
            clGetProgramBuildInfo(program, device, CL_PROGRAM_BUILD_LOG,
                lsz + 1, log, NULL);
            std::string op = "Could not build CL Program! Reason:\n";
            op += log;
            delete[] log;
            throw std::runtime_error(op);
        }

        i_buff = clCreateBuffer(context, CL_MEM_READ_ONLY | CL_MEM_ALLOC_HOST_PTR,
            4 * STREAM_WIDTH * STREAM_HEIGHT, NULL, &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Input buffer! Code: " + std::to_string(err));
        }

        o_buff = clCreateBuffer(context, CL_MEM_WRITE_ONLY | CL_MEM_ALLOC_HOST_PTR,
            4 * STREAM_WIDTH * STREAM_HEIGHT, NULL, &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Output buffer! Code: " + std::to_string(err));
        }

        queue = clCreateCommandQueueWithProperties(context, device, {0}, &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Command queue! Code: " + std::to_string(err));
        }

        kernel = clCreateKernel(program, "neighbors", &err);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not create CL Kernel! Code: " + std::to_string(err));
        }

        err = clSetKernelArg(kernel, 0, sizeof(cl_mem), &i_buff);
        err |= clSetKernelArg(kernel, 0, sizeof(cl_mem), &o_buff);
        if(err != CL_SUCCESS)
        {
            throw std::runtime_error("Could not bind Kernel params!");
        }

        clog(info) << "LW Kernel successfully set up! G" << global_size << ", L" << local_size << std::endl;
    }

    void work() override
    {
        m_temp_stor = m_work_matrix.clone();

        if(err = clEnqueueWriteBuffer(queue, i_buff, CL_TRUE, 0, 4 * STREAM_WIDTH * STREAM_HEIGHT,
            m_temp_stor.ptr<uchar>(), 0, NULL, NULL) != CL_SUCCESS)
        {
            clog(log_type::err) << "LW Input buffer could not be written! Code: " << err << std::endl;
            return;
        }
        
        if(err = clEnqueueNDRangeKernel(queue, kernel, 1, NULL, &global_size, &local_size, 0, NULL, NULL) != CL_SUCCESS)
        {
            clog(log_type::err) << "LW Kernel could not be enqueued! Code: " << err << std::endl;
            return;
        }
        else clog(log_type::warn) << "yoo???" << std::endl;

        if(err = clEnqueueReadBuffer(queue, o_buff, CL_TRUE, 0, 4 * STREAM_WIDTH * STREAM_HEIGHT,
            m_work_matrix.ptr<uchar>(), 0, NULL, NULL) != CL_SUCCESS)
        {
            clog(log_type::err) << "LW Output buffer could not be read! Code: " << err << std::endl;
        }

        /*m_work_matrix.forEach<cv::Vec3b>([&](cv::Vec3b &pixel, const int *pos){
            int ym = __lw_min(pos[0] + __ldetail, m_temp_stor.size().height - 1);
            int xm = __lw_min(pos[1] + __ldetail, m_temp_stor.size().width - 1);
            cv::Vec3b &cmp = m_temp_stor.at<cv::Vec3b>(pos);

            for(int y = __lw_max(pos[0] - __ldetail, 0); y <= ym; y += __ldetail)
            {
                for(int x = __lw_max(pos[1] - __ldetail, 0); x <= xm; x += __ldetail)
                {
                    if(cmp != m_temp_stor.at<cv::Vec3b>(y, x))
                    {
                        pixel = cv::Vec3b{255,255,255};
                        return;
                    }
                }
            }
        });*/
        
    }

    void post() override
    {
        clReleaseContext(context);
        clReleaseKernel(kernel);
        clReleaseCommandQueue(queue);
        clReleaseProgram(program);
        clReleaseDevice(device);
        clReleaseMemObject(i_buff);
        clReleaseMemObject(o_buff);
    }
};

};
