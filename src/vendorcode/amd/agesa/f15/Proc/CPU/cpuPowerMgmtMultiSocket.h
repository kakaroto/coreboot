/* $NoKeywords:$ */
/**
 * @file
 *
 * AMD CPU Power Management Multisocket Functions.
 *
 * Contains code for doing power management for multisocket CPUs
 *
 * @xrefitem bom "File Content Label" "Release Content"
 * @e project:      AGESA
 * @e sub-project:  CPU
 * @e \$Revision: 55600 $   @e \$Date: 2011-06-23 12:39:18 -0600 (Thu, 23 Jun 2011) $
 *
 */
/*
 ******************************************************************************
 *
 * Copyright (C) 2012 Advanced Micro Devices, Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *     * Redistributions of source code must retain the above copyright
 *       notice, this list of conditions and the following disclaimer.
 *     * Redistributions in binary form must reproduce the above copyright
 *       notice, this list of conditions and the following disclaimer in the
 *       documentation and/or other materials provided with the distribution.
 *     * Neither the name of Advanced Micro Devices, Inc. nor the names of
 *       its contributors may be used to endorse or promote products derived
 *       from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL ADVANCED MICRO DEVICES, INC. BE LIABLE FOR ANY
 * DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
 * ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 ******************************************************************************
 */

#ifndef _CPU_POWER_MGMT_MULTI_SOCKET_H_
#define _CPU_POWER_MGMT_MULTI_SOCKET_H_


/*---------------------------------------------------------------------------------------
 *          M I X E D   (Definitions And Macros / Typedefs, Structures, Enums)
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                 D E F I N I T I O N S     A N D     M A C R O S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *               T Y P E D E F S,   S T R U C T U R E S,    E N U M S
 *---------------------------------------------------------------------------------------
 */


/*---------------------------------------------------------------------------------------
 *                        F U N C T I O N    P R O T O T Y P E
 *---------------------------------------------------------------------------------------
 */
VOID
RunCodeOnAllSystemCore0sMulti (
  IN       AP_TASK *TaskPtr,
  IN       AMD_CONFIG_PARAMS *StdHeader,
  IN       VOID *ConfigParams
  );

VOID
GetNumberOfSystemPmStepsPtrMulti (
     OUT   UINT8 *NumSystemSteps,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetSystemNbCofMulti (
  IN       UINT32 NbPstate,
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32 *SystemNbCofNumerator,
     OUT   UINT32 *SystemNbCofDenominator,
     OUT   BOOLEAN *SystemNbCofsMatch,
     OUT   BOOLEAN *NbPstateIsEnabledOnAllCPUs,
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

BOOLEAN
GetSystemNbCofVidUpdateMulti (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );

VOID
GetMinNbCofMulti (
  IN       PLATFORM_CONFIGURATION *PlatformConfig,
     OUT   UINT32                 *MinSysNbFreq,
     OUT   UINT32                 *MinP0NbFreq,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

BOOLEAN
GetCurrPciAddrMulti (
     OUT   PCI_ADDR               *PciAddress,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

VOID
ModifyCurrSocketPciMulti (
  IN       PCI_ADDR               *PciAddress,
  IN       UINT32                 Mask,
  IN       UINT32                 Data,
  IN       AMD_CONFIG_PARAMS      *StdHeader
  );

AGESA_STATUS
GetEarlyPmErrorsMulti (
  IN       AMD_CONFIG_PARAMS *StdHeader
  );
#endif  // _CPU_POWER_MGMT_MULTI_SOCKET_H_
