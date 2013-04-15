/*
 * Copyright © 2013 Canonical Ltd.
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

#include "src/server/graphics/android/hwc10_device.h"
#include "src/server/graphics/android/hwc11_device.h"
#include "src/server/graphics/android/hwc_layerlist.h"
#include "mir_test_doubles/mock_fb_device.h"
#include "mir_test_doubles/mock_hwc_composer_device_1.h"
#include "mir_test_doubles/mock_android_buffer.h"
#include "mir_test_doubles/mock_hwc_organizer.h"

#include <thread>
#include <chrono>
#include <stdexcept>
#include <memory>
#include <gtest/gtest.h>

namespace mga=mir::graphics::android;
namespace mtd=mir::test::doubles;
namespace geom=mir::geometry;

template<class T>
std::shared_ptr<mga::HWCCommonDevice> make_hwc_device(std::shared_ptr<hwc_composer_device_1> const& hwc_device,
                                                std::shared_ptr<mga::HWCLayerOrganizer> const& organizer,
                                                std::shared_ptr<mga::FBDevice> const& fbdev);

template <>
std::shared_ptr<mga::HWCCommonDevice> make_hwc_device<mga::HWC10Device>(
                                                std::shared_ptr<hwc_composer_device_1> const& hwc_device,
                                                std::shared_ptr<mga::HWCLayerOrganizer> const& organizer,
                                                std::shared_ptr<mga::FBDevice> const& fbdev)
{
    return std::make_shared<mga::HWC10Device>(hwc_device, organizer, fbdev);
}

template <>
std::shared_ptr<mga::HWCCommonDevice> make_hwc_device<mga::HWC11Device>(
                                                std::shared_ptr<hwc_composer_device_1> const& hwc_device,
                                                std::shared_ptr<mga::HWCLayerOrganizer> const& organizer,
                                                std::shared_ptr<mga::FBDevice> const& fbdev)
{
    return std::make_shared<mga::HWC11Device>(hwc_device, organizer, fbdev);
}

namespace
{
struct HWCDummyLayer : public mga::HWCLayerBase
{
    HWCDummyLayer() = default;
};
}

template<typename T>
class HWCCommon : public ::testing::Test
{
protected:
    virtual void SetUp()
    {
        mock_device = std::make_shared<testing::NiceMock<mtd::MockHWCComposerDevice1>>();
        mock_fbdev = std::make_shared<mtd::MockFBDevice>();
        mock_organizer = std::make_shared<mtd::MockHWCOrganizer>();
    }

    std::shared_ptr<mtd::MockHWCOrganizer> mock_organizer;
    std::shared_ptr<mtd::MockHWCComposerDevice1> mock_device;
    std::shared_ptr<mtd::MockFBDevice> mock_fbdev;
};

typedef ::testing::Types<mga::HWC10Device, mga::HWC11Device> HWCDeviceTestTypes;
TYPED_TEST_CASE(HWCCommon, HWCDeviceTestTypes);

TYPED_TEST(HWCCommon, test_proc_registration)
{
    using namespace testing;

    hwc_procs_t const* procs;
    EXPECT_CALL(*(this->mock_device), registerProcs_interface(this->mock_device.get(), _))
        .Times(1)
        .WillOnce(SaveArg<1>(&procs));

    auto device = make_hwc_device<TypeParam>(this->mock_device, this->mock_organizer, this->mock_fbdev);

    EXPECT_NE(nullptr, procs->invalidate);
    EXPECT_NE(nullptr, procs->vsync);
    EXPECT_NE(nullptr, procs->hotplug);
}

TYPED_TEST(HWCCommon, test_vsync_activation_comes_after_proc_registration)
{
    using namespace testing;

    InSequence sequence_enforcer;
    EXPECT_CALL(*this->mock_device, registerProcs_interface(this->mock_device.get(),_))
        .Times(1);
    EXPECT_CALL(*this->mock_device, eventControl_interface(this->mock_device.get(), 0, HWC_EVENT_VSYNC, 1))
        .Times(1)
        .WillOnce(Return(0));

    auto device = make_hwc_device<TypeParam>(this->mock_device, this->mock_organizer, this->mock_fbdev);
    testing::Mock::VerifyAndClearExpectations(this->mock_device.get());
}

TYPED_TEST(HWCCommon, test_vsync_activation_failure_throws)
{
    using namespace testing;

    EXPECT_CALL(*this->mock_device, eventControl_interface(this->mock_device.get(), 0, HWC_EVENT_VSYNC, 1))
        .Times(1)
        .WillOnce(Return(-EINVAL));

    EXPECT_THROW({
        auto device = make_hwc_device<TypeParam>(this->mock_device, this->mock_organizer, this->mock_fbdev);
    }, std::runtime_error);
}

namespace
{
static mga::HWCDevice *global_device;
void* waiting_device(void*)
{
    global_device->wait_for_vsync();
    return NULL;
}
}

TYPED_TEST(HWCCommon, test_vsync_hook_waits)
{
    auto device = make_hwc_device<TypeParam>(this->mock_device, this->mock_organizer, this->mock_fbdev);
    global_device = device.get();

    pthread_t thread;
    pthread_create(&thread, NULL, waiting_device, NULL);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    void* retval;
    auto error = pthread_tryjoin_np(thread, &retval);
    ASSERT_EQ(EBUSY, error);

    device->notify_vsync();
    error = pthread_join(thread, &retval);
    ASSERT_EQ(0, error);

}

#if 0
TYPED_TEST(HWCCommon, test_vsync_hook_from_hwc_unblocks_wait)
{
    using namespace testing;

    hwc_procs_t const* procs;
    EXPECT_CALL(*mock_device, registerProcs_interface(mock_device.get(), _))
        .Times(1)
        .WillOnce(SaveArg<1>(&procs));

    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    global_device = &device;

    pthread_t thread;
    pthread_create(&thread, NULL, waiting_device, NULL);

    std::this_thread::sleep_for(std::chrono::milliseconds(5));

    void* retval;
    auto error = pthread_tryjoin_np(thread, &retval);
    ASSERT_EQ(EBUSY, error);

    procs->vsync(procs, 0, 0);
    error = pthread_join(thread, &retval);
    ASSERT_EQ(0, error);
}

TYPED_TEST(HWCCommon, test_hwc_turns_on_display_after_proc_registration)
{
    using namespace testing;
    InSequence sequence_enforcer;
    EXPECT_CALL(*mock_device, registerProcs_interface(mock_device.get(),_))
        .Times(1);
    EXPECT_CALL(*mock_device, blank_interface(mock_device.get(), HWC_DISPLAY_PRIMARY, 0))
        .Times(1);

    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    testing::Mock::VerifyAndClearExpectations(mock_device.get());
}

TYPED_TEST(HWCCommon, test_hwc_throws_on_blank_error)
{
    using namespace testing;

    EXPECT_CALL(*mock_device, blank_interface(mock_device.get(), HWC_DISPLAY_PRIMARY, 0))
        .Times(1)
        .WillOnce(Return(-1));

    EXPECT_THROW({
        mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    }, std::runtime_error);
}

TYPED_TEST(HWCCommon, test_hwc_display_is_deactivated_on_destroy)
{
    auto device = std::make_shared<mga::HWC11Device>(mock_device, mock_organizer, mock_fbdev);

    EXPECT_CALL(*mock_device, blank_interface(mock_device.get(), HWC_DISPLAY_PRIMARY, 1))
        .Times(1);
    EXPECT_CALL(*mock_device, eventControl_interface(mock_device.get(), HWC_DISPLAY_PRIMARY, HWC_EVENT_VSYNC, 0))
        .Times(1);
    device.reset();
}

TYPED_TEST(HWCCommon, hwc_device_reports_2_fbs_available_by_default)
{
    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    EXPECT_EQ(2u, device.number_of_framebuffers_available());
}

TYPED_TEST(HWCCommon, hwc_device_reports_abgr_8888_by_default)
{
    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    EXPECT_EQ(geom::PixelFormat::abgr_8888, device.display_format());
}

TYPED_TEST(HWCCommon, hwc_device_set_next_frontbuffer_adds_to_layerlist)
{
    std::shared_ptr<mga::AndroidBuffer> mock_buffer = std::make_shared<mtd::MockAndroidBuffer>();
    EXPECT_CALL(*mock_organizer, set_fb_target(mock_buffer))
        .Times(1);
 
    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    device.set_next_frontbuffer(mock_buffer);
}

TYPED_TEST(HWCCommon, hwc_device_set_next_frontbuffer_posts)
{
    std::shared_ptr<mga::AndroidBuffer> mock_buffer = std::make_shared<mtd::MockAndroidBuffer>();
    EXPECT_CALL(*mock_fbdev, post(mock_buffer))
        .Times(1);

    mga::HWC11Device device(mock_device, mock_organizer, mock_fbdev);
    device.set_next_frontbuffer(mock_buffer);
}
#endif
