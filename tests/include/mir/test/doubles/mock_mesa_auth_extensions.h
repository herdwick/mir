/*
 * Copyright © Canonical Ltd.
 *
 * This program is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 or 3,
 * as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef MIR_TEST_DOUBLES_MOCK_GBM_AUTH_EXTENSIONS_H_
#define MIR_TEST_DOUBLES_MOCK_GBM_AUTH_EXTENSIONS_H_

#include "mir/graphics/platform_authentication.h"

namespace mir
{
namespace test
{
namespace doubles
{

struct MockMesaExt : graphics::MesaAuthExtension
{
    MOCK_METHOD0(auth_fd, mir::Fd());
    MOCK_METHOD1(auth_magic, int(unsigned int));
};

}
}
}
#endif /* MIR_TEST_DOUBLES_MOCK_GBM_AUTH_EXTENSIONS_H_ */
