/*
 * Copyright © 2012 Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 3 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Authored by: Kevin DuBois <kevin.dubois@canonical.com>
 */

#include "mir/process/process.h"

#include "mir_client/mir_client_library.h"

#include "mir/frontend/protobuf_asio_communicator.h"
#include "mir/frontend/resource_cache.h"
#include "mir/graphics/android/android_buffer.h"
#include "mir/graphics/android/android_alloc_adaptor.h"
#include "mir/compositor/buffer_ipc_package.h"

#include "mir_test/mock_server_tool.h"
#include "mir_test/test_server.h"
#include "mir_test/empty_deleter.h"

#include <hardware/gralloc.h>
#include <GLES2/gl2.h>
#include <gmock/gmock.h>

#include "mir/thread/all.h"
 
namespace mp=mir::process;
namespace mt=mir::test;
namespace mc=mir::compositor;
namespace mga=mir::graphics::android;
namespace geom=mir::geometry;

namespace
{
static int test_width  = 300;
static int test_height = 200;

/* used by both client/server for patterns */ 
bool render_pattern(MirGraphicsRegion *region, bool check)
{
    if (region->pixel_format != mir_pixel_format_rgba_8888 )
        return false;

    int *pixel = (int*) region->vaddr; 
    int i,j;
    for(i=0; i< region->width; i++)
    {
        for(j=0; j<region->height; j++)
        {
            if (check)
            {
                printf("BUFFER %X\n", pixel[j*region->width + i]);
                if (pixel[j*region->width + i] != 0x12345689)
                    return false;
            }
            else
            {
                pixel[j*region->width + i] = 0x12345689;
            }
        }
    }
    return true;
}

bool render_second_pattern(MirGraphicsRegion *region, bool check)
{
    if (region->pixel_format != mir_pixel_format_rgba_8888 )
        return false;

    int *pixel = (int*) region->vaddr; 
    int i,j;
    for(i=0; i< region->width; i++)
    {
        for(j=0; j<region->height; j++)
        {
            //should render red/blue/teal/white square
            //lsb
            if (j < region->height/2)
            { 
                if (i < region->width/2)
                {
                    if (check)
                    {   
                        if (pixel[j*region->width + i] != (int) 0xFFFF0000) return false;
                    }
                    else
                    {
                        pixel[j*region->width + i] = 0xFFFF0000;
                    }
                } 
                else
                { 
                    if (check)
                    {
                        if (pixel[j*region->width + i] != (int)0xFFFFFF00) return false;
                    }
                    else
                    {
                        pixel[j*region->width + i] = 0xFFFFFF00;
                    }
                }
            }
            else
            { 
                if (i < region->width/2)
                {
                    if (check)
                    {
                        if (pixel[j*region->width + i] != (int)0xFFFFFFFF) return false;
                    }
                    else
                    {
                        pixel[j*region->width + i] = 0xFFFFFFFF;
                    }
                } 
                else
                { 
                    if (check)
                    {
                        if (pixel[j*region->width + i] != (int)0xFF0000FF) return false;
                    }
                    else
                    {
                        pixel[j*region->width + i] = 0xFF0000FF;
                    }
                }
            }
        }
    }

    return true;
}
}

namespace mir
{
namespace test
{
/* client code */
static void connected_callback(MirConnection *connection, void* context)
{
    MirConnection** tmp = (MirConnection**) context;
    *tmp = connection;
}
static void create_callback(MirSurface *surface, void*context)
{
    MirSurface** surf = (MirSurface**) context;
    *surf = surface;
}

static void next_callback(MirSurface *, void*)
{
}

struct TestClient
{

static void sig_handle(int)
{
}

static int render_single()
{
    if (signal(SIGCONT, sig_handle) == SIG_ERR)
        return -1;
    pause();

    /* only use C api */
    MirConnection* connection = NULL;
    MirSurface* surface;
    MirSurfaceParameters surface_parameters;

     /* establish connection. wait for server to come up */
    while (connection == NULL)
    {
        mir_wait_for(mir_connect("./test_socket_surface", "test_renderer",
                                     &connected_callback, &connection));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    /* make surface */
    surface_parameters.name = "testsurface";
    surface_parameters.width = test_width;
    surface_parameters.height = test_height;
    surface_parameters.pixel_format = mir_pixel_format_rgba_8888;
    mir_wait_for(mir_surface_create( connection, &surface_parameters,
                                      &create_callback, &surface));
    MirGraphicsRegion graphics_region;
    /* grab a buffer*/
    mir_surface_get_graphics_region( surface, &graphics_region);

    /* render pattern */
    render_pattern(&graphics_region, false);

    mir_wait_for(mir_surface_release(surface, &create_callback, &surface));

    /* release */
    mir_connection_release(connection);
    return 0;
}

static int render_double()
{
    if (signal(SIGCONT, sig_handle) == SIG_ERR)
        return -1;
    pause();

    /* only use C api */
    MirConnection* connection = NULL;
    MirSurface* surface;
    MirSurfaceParameters surface_parameters;
    MirGraphicsRegion graphics_region;

     /* establish connection. wait for server to come up */
    while (connection == NULL)
    {
        mir_wait_for(mir_connect("./test_socket_surface", "test_renderer",
                                     &connected_callback, &connection));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    /* make surface */
    surface_parameters.name = "testsurface";
    surface_parameters.width = test_width;
    surface_parameters.height = test_height;
    surface_parameters.pixel_format = mir_pixel_format_rgba_8888;

    mir_wait_for(mir_surface_create( connection, &surface_parameters,
                                      &create_callback, &surface));
    mir_surface_get_graphics_region( surface, &graphics_region);
    render_pattern(&graphics_region, false);

    mir_wait_for(mir_surface_next_buffer(surface, &next_callback, (void*) NULL));
    mir_surface_get_graphics_region( surface, &graphics_region);
    render_second_pattern(&graphics_region, false);

    mir_wait_for(mir_surface_release(surface, &create_callback, &surface));

    /* release */
    mir_connection_release(connection);
    return 0;
}

static int render_accelerated()
{
    if (signal(SIGCONT, sig_handle) == SIG_ERR)
        return -1;
    pause();

    /* only use C api */
    MirConnection* connection = NULL;
    MirSurface* surface;
    MirSurfaceParameters surface_parameters;

     /* establish connection. wait for server to come up */
    while (connection == NULL)
    {
        mir_wait_for(mir_connect("./test_socket_surface", "test_renderer",
                                     &connected_callback, &connection));
        std::this_thread::sleep_for(std::chrono::milliseconds(10));
    }
    /* make surface */
    surface_parameters.name = "testsurface";
    surface_parameters.width = test_width;
    surface_parameters.height = test_height;
    surface_parameters.pixel_format = mir_pixel_format_rgba_8888;
    surface_parameters.acceleration = mir_opengl_acceleration;

    mir_wait_for(mir_surface_create( connection, &surface_parameters,
                                      &create_callback, &surface));
    
	int major, minor, n;
	EGLDisplay disp;
    EGLContext context;
    EGLSurface egl_surface;
	EGLConfig egl_config;
    EGLint attribs[] = {
        EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
        EGL_GREEN_SIZE, 8,
        EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
        EGL_NONE };
    EGLint context_attribs[] = { EGL_CONTEXT_CLIENT_VERSION, 2, EGL_NONE };

    EGLNativeWindowType native_window = mir_get_egl_type(surface);
   
    printf("PID %i\n", gettid()); 
	disp = eglGetDisplay(EGL_DEFAULT_DISPLAY);
    printf("Here 1 %i\n", (int) disp);
    eglInitialize(disp, &major, &minor);
    printf("Here 2 %i %i\n", major, minor);
	
	if( eglChooseConfig(disp, attribs, &egl_config, 1, &n) == EGL_FALSE)
        printf("error choose\n");;
    printf("chooseconfig %i\n", n);
    printf("chooseconfig num %i\n", (int) egl_config);
    egl_surface = eglCreateWindowSurface(disp, egl_config, native_window, NULL);
    printf("Here 4 EGL SURFACE: 0x%X\n",(int) egl_surface);
    context = eglCreateContext(disp, egl_config, EGL_NO_CONTEXT, context_attribs);
    printf("Here 5 EGL CONTEXT: 0x%X\n", (int) context);
    auto rc = eglMakeCurrent(disp, egl_surface, egl_surface, context);
    printf("Here 6 %i\n", rc);

    glClearColor(1.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    eglSwapBuffers(disp, egl_surface);
    mir_wait_for(mir_surface_release(surface, &create_callback, &surface));

    /* release */
    mir_connection_release(connection);
    return 0;

}

static int exit_function()
{
    return EXIT_SUCCESS;
}
};

/* server code */
struct MockServerGenerator : public mt::MockServerTool
{
    MockServerGenerator(const std::shared_ptr<mc::BufferIPCPackage>& pack, int id)
     : package(pack),
        next_received(false),
        next_allowed(false),
        package_id(id)
    {

    }

    void create_surface(google::protobuf::RpcController* /*controller*/,
                 const mir::protobuf::SurfaceParameters* request,
                 mir::protobuf::Surface* response,
                 google::protobuf::Closure* done)
    {
        response->mutable_id()->set_value(13); // TODO distinct numbers & tracking
        response->set_width(test_width);
        response->set_height(test_height);
        response->set_pixel_format(request->pixel_format());
        response->mutable_buffer()->set_buffer_id(package_id);

        unsigned int i;
        response->mutable_buffer()->set_fds_on_side_channel(1);
        for(i=0; i<package->ipc_fds.size(); i++)
            response->mutable_buffer()->add_fd(package->ipc_fds[i]);
        for(i=0; i<package->ipc_data.size(); i++)
            response->mutable_buffer()->add_data(package->ipc_data[i]);

        std::unique_lock<std::mutex> lock(guard);
        surface_name = request->surface_name();
        wait_condition.notify_one();

        done->Run();
    }

    virtual void next_buffer(
        ::google::protobuf::RpcController* /*controller*/,
        ::mir::protobuf::SurfaceId const* /*request*/,
        ::mir::protobuf::Buffer* response,
        ::google::protobuf::Closure* done)
    {
        {
            std::unique_lock<std::mutex> lk(next_guard);
            next_received = true;
            next_cv.notify_all();

            while (!next_allowed) {
                allow_cv.wait(lk);
            }
            next_allowed = false;
        }

        response->set_buffer_id(package_id);
        unsigned int i;
        response->set_fds_on_side_channel(1);
        for(i=0; i<package->ipc_fds.size(); i++)
            response->add_fd(package->ipc_fds[i]);
        for(i=0; i<package->ipc_data.size(); i++)
            response->add_data(package->ipc_data[i]);

        done->Run();
    }

    void wait_on_next_buffer()
    {
        std::unique_lock<std::mutex> lk(next_guard);
        while (!next_received)
            next_cv.wait(lk);
        next_received = false;
    }

    void allow_next_continue()
    {
        std::unique_lock<std::mutex> lk(next_guard);
        next_allowed = true;
        allow_cv.notify_all();
        lk.unlock();
    }

    void set_package(const std::shared_ptr<mc::BufferIPCPackage>& pack, int id)
    {
        package = pack;
        package_id = id;
    }

    std::shared_ptr<mc::BufferIPCPackage> package;

    std::mutex next_guard;
    std::condition_variable next_cv;
    std::condition_variable allow_cv;
    bool next_received;
    bool next_allowed;

//    std::mutex allow_guard;

    int package_id;
};

bool check_buffer(std::shared_ptr<mc::BufferIPCPackage> package, const hw_module_t *hw_module)
{
    native_handle_t* handle;
    handle = (native_handle_t*) malloc(sizeof(int) * ( 3 + package->ipc_data.size() + package->ipc_fds.size() ));
    handle->numInts = package->ipc_data.size();
    handle->numFds  = package->ipc_fds.size();
    int i;
    for(i = 0; i< handle->numFds; i++)
        handle->data[i] = package->ipc_fds[i];
    for(; i < handle->numFds + handle->numInts; i++)
        handle->data[i] = package->ipc_data[i-handle->numFds];

    int *vaddr;
    int usage = GRALLOC_USAGE_SW_READ_OFTEN | GRALLOC_USAGE_SW_WRITE_OFTEN;
    gralloc_module_t *grmod = (gralloc_module_t*) hw_module;
    grmod->lock(grmod, handle, usage, 0, 0, test_width, test_height, (void**) &vaddr); 

    MirGraphicsRegion region;
    region.vaddr = (char*) vaddr;
    region.width = test_width;
    region.height = test_height;
    region.pixel_format = mir_pixel_format_rgba_8888; 

    auto valid = render_pattern(&region, true);
    auto valid2 = render_second_pattern(&region, true);
    grmod->unlock(grmod, handle);

    return valid || valid2; 
}

}
}

struct TestClientIPCRender : public testing::Test
{
    /* kdub -- some of the (less thoroughly tested) android blob drivers annoyingly keep
       static state about what process they are in. Once you fork, this info is invalid,
       yet the driver uses the info and bad things happen.
       Fork all needed processes before touching the blob! */
    static void SetUpTestCase() {
        render_single_client_process = mp::fork_and_run_in_a_different_process(
            mt::TestClient::render_single,
            mt::TestClient::exit_function);

        render_double_client_process = mp::fork_and_run_in_a_different_process(
            mt::TestClient::render_double,
            mt::TestClient::exit_function);

        second_render_with_same_buffer_client_process
             = mp::fork_and_run_in_a_different_process(
                            mt::TestClient::render_double,
                            mt::TestClient::exit_function);

        render_accelerated_process 
             = mp::fork_and_run_in_a_different_process(
                            mt::TestClient::render_accelerated,
                            mt::TestClient::exit_function);
    }

    void SetUp() {
        size = geom::Size{geom::Width{test_width}, geom::Height{test_height}};
        pf = geom::PixelFormat::rgba_8888;

        /* allocate an android buffer */
        int err;
        struct alloc_device_t *alloc_device_raw;
        err = hw_get_module(GRALLOC_HARDWARE_MODULE_ID, &hw_module);
        if (err < 0)
            throw std::runtime_error("Could not open hardware module");
        gralloc_open(hw_module, &alloc_device_raw);
        auto alloc_device = std::shared_ptr<struct alloc_device_t> ( alloc_device_raw, mir::EmptyDeleter());
        auto alloc_adaptor = std::make_shared<mga::AndroidAllocAdaptor>(alloc_device);

        android_buffer = std::make_shared<mga::AndroidBuffer>(alloc_adaptor, size, pf);
        second_android_buffer = std::make_shared<mga::AndroidBuffer>(alloc_adaptor, size, pf);

        package = android_buffer->get_ipc_package();
        for(auto it = package->ipc_data.begin(); it != package->ipc_data.end();it++)
            printf("data: %i\n", *it);
        second_package = second_android_buffer->get_ipc_package();

    }

    void TearDown()
    {
        test_server->comm.stop();
    }

    mir::protobuf::Connection response;

    std::shared_ptr<mt::TestServer> test_server;
    std::shared_ptr<mt::MockServerGenerator> mock_server;

    const hw_module_t    *hw_module;
    geom::Size size;
    geom::PixelFormat pf; 
    std::shared_ptr<mp::Process> client_process;
    std::shared_ptr<mc::BufferIPCPackage> package;
    std::shared_ptr<mc::BufferIPCPackage> second_package;
    std::shared_ptr<mga::AndroidBuffer> android_buffer;
    std::shared_ptr<mga::AndroidBuffer> second_android_buffer;

    static std::shared_ptr<mp::Process> render_single_client_process;
    static std::shared_ptr<mp::Process> render_double_client_process;
    static std::shared_ptr<mp::Process> second_render_with_same_buffer_client_process;
    static std::shared_ptr<mp::Process> render_accelerated_process;
};
std::shared_ptr<mp::Process> TestClientIPCRender::render_single_client_process;
std::shared_ptr<mp::Process> TestClientIPCRender::render_double_client_process;
std::shared_ptr<mp::Process> TestClientIPCRender::second_render_with_same_buffer_client_process;
std::shared_ptr<mp::Process> TestClientIPCRender::render_accelerated_process;

TEST_F(TestClientIPCRender, test_render_single)
{
    /* start a server */
    mock_server = std::make_shared<mt::MockServerGenerator>(package, 14);
    test_server = std::make_shared<mt::TestServer>("./test_socket_surface", mock_server);
    test_server->comm.start();

    /* activate client */
    render_single_client_process->cont();

    /* wait for client to finish */
    EXPECT_TRUE(render_single_client_process->wait_for_termination().succeeded());

    /* check content */
    EXPECT_TRUE(mt::check_buffer(mock_server->package, hw_module));
}

TEST_F(TestClientIPCRender, test_render_double)
{
    /* start a server */
    mock_server = std::make_shared<mt::MockServerGenerator>(package, 14);
    test_server = std::make_shared<mt::TestServer>("./test_socket_surface", mock_server);
    test_server->comm.start();

    /* activate client */
    render_double_client_process->cont();

    /* wait for next buffer */
    mock_server->wait_on_next_buffer();
    EXPECT_TRUE(mt::check_buffer(package, hw_module));

    mock_server->set_package(second_package, 15);

    mock_server->allow_next_continue();
    /* wait for client to finish */
    EXPECT_TRUE(render_double_client_process->wait_for_termination().succeeded());

    /* check content */
    EXPECT_TRUE(mt::check_buffer(second_package, hw_module));
}

TEST_F(TestClientIPCRender, test_second_render_with_same_buffer)
{
    /* start a server */
    mock_server = std::make_shared<mt::MockServerGenerator>(package, 14);
    test_server = std::make_shared<mt::TestServer>("./test_socket_surface", mock_server);
    test_server->comm.start();

    /* activate client */
    second_render_with_same_buffer_client_process->cont();

    /* wait for next buffer */
    mock_server->wait_on_next_buffer();
    mock_server->allow_next_continue();

    /* wait for client to finish */
    EXPECT_TRUE(second_render_with_same_buffer_client_process->wait_for_termination().succeeded());

    /* check content */
    EXPECT_TRUE(mt::check_buffer(package, hw_module));
}

TEST_F(TestClientIPCRender, test_accelerated_render)
{
    /* start a server */
    mock_server = std::make_shared<mt::MockServerGenerator>(package, 14);
    test_server = std::make_shared<mt::TestServer>("./test_socket_surface", mock_server);
    test_server->comm.start();

    /* activate client */
    render_accelerated_process->cont();

    /* wait for next buffer */
//    mock_server->wait_on_next_buffer();
//    mock_server->allow_next_continue();

    /* wait for client to finish */
    EXPECT_TRUE(render_accelerated_process->wait_for_termination().succeeded());

    /* check content */
    EXPECT_TRUE(mt::check_buffer(package, hw_module));
}
