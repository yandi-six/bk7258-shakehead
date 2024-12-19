// Copyright 2022-2024 Beken
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

//This is a generated file, don't modify it!

#pragma once


#include "_ota.h"
#include "_ppc.h"
#include "_mpc.h"
#undef MCUBOOT_SIGN_RSA                             
#define MCUBOOT_SIGN_EC256                            1
#define CONFIG_CODE_ENCRYPTED                         0
#define FLASH_VIRTUAL2PHY(virtual_addr)               ((((virtual_addr) >> 5) * 34) + ((virtual_addr) & 31))
#define FLASH_PHY2VIRTUAL(phy_addr)                   ((((phy_addr) / 34) << 5) + ((phy_addr) % 34))
#define CEIL_ALIGN_34(addr)                           (((addr) + 34 - 1) / 34 * 34)

#define FLASH_CEIL_ALIGN(v, align) ((((v) + ((align) - 1)) / (align)) * (align))
#define FLASH_PHY2VIRTUAL_CODE_START(phy_addr) FLASH_CEIL_ALIGN(FLASH_PHY2VIRTUAL(FLASH_CEIL_ALIGN((phy_addr), 34)), CPU_VECTOR_ALIGN_SZ)

