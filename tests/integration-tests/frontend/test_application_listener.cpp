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
 * Authored by: Alan Griffiths <alan@octopull.co.uk>
 */

#include "mir_client/mir_client_library.h"
#include "mir/frontend/application_listener.h"

#include "mir_test/display_server_test_fixture.h"
#include "mir_test/test_client.h"
#include <gmock/gmock.h>
#include <gtest/gtest.h>

namespace mf = mir::frontend;
namespace mt = mir::test;

TEST_F(BespokeDisplayServerTestFixture, application_listener_is_notified)
{
    struct Server : TestingServerConfiguration
    {
        std::shared_ptr<mf::ApplicationListener>
        make_application_listener()
        {
            struct MockApplicationListener : mf::NullApplicationListener
            {
                MOCK_METHOD1(application_connect_called, void (std::string const&));
            };

            auto result = std::make_shared<MockApplicationListener>();

            EXPECT_CALL(*result, application_connect_called(testing::_)).
                Times(1);

            return result;
        }
    } server_processing;

    launch_server_process(server_processing);

    struct Client: TestingClientConfiguration
    {
        void exec()
        {
            mt::TestClient stub_client(mir::test_socket_file());

            stub_client.connect_parameters.set_application_name(__PRETTY_FUNCTION__);
            EXPECT_CALL(stub_client, connect_done()).
                Times(testing::AtLeast(0));

            stub_client.display_server.connect(
                0,
                &stub_client.connect_parameters,
                &stub_client.connection,
                google::protobuf::NewCallback(&stub_client, &mt::TestClient::connect_done));

            stub_client.wait_for_connect_done();
        }
    } client_process;

    launch_client_process(client_process);
}
