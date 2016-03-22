//
// Copyright (c) 2016, Paul R. Swan
// All rights reserved.
//
// Redistribution and use in source and binary forms, with or without modification,
// are permitted provided that the following conditions are met:
//
// 1. Redistributions of source code must retain the above copyright notice,
//    this list of conditions and the following disclaimer.
//
// 2. Redistributions in binary form must reproduce the above copyright notice,
//    this list of conditions and the following disclaimer in the documentation
//    and/or other materials provided with the distribution.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS
// OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
// MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
// COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
// EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
// HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR
// TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
// EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
//
#include "Arduino.h"
#include "Error.h"
#include "CT11Cpu.h"
#include "PinMap.h"


//
// Pin prefixes
//
// _ - active low
//
// Pin suffixes
//
// i - input
// o - output
// t - tri-state
//

//
// Control Pins
//

static const CONNECTION s_BGND_i     = {  8, "BGND"     };

static const CONNECTION s__BCLR_o    = { 18, "_BCLR"    };
static const CONNECTION s_PUP_i      = { 19, "PUP"      };
static const CONNECTION s_GND_i      = { 20, "GND"      };
static const CONNECTION s_COUT_o     = { 21, "COUT"     };
static const CONNECTION s_XTL1_i     = { 22, "XTL1"     };
static const CONNECTION s_XTL0_i     = { 23, "XTL0"     };
static const CONNECTION s_SEL1_o     = { 24, "SEL1"     };
static const CONNECTION s_SEL0_o     = { 25, "SEL0"     };
static const CONNECTION s_READY_i    = { 26, "READY"    };
static const CONNECTION s_R_WHB_o    = { 27, "R_WHB"    };
static const CONNECTION s_R_WLB_o    = { 28, "R_WLB"    };
static const CONNECTION s__RAS_o     = { 29, "_RAS"     };
static const CONNECTION s__CAS_o     = { 30, "_CAS"     };
static const CONNECTION s_PI_o       = { 31, "PI"       };
static const CONNECTION s_AI10_DMR_i = { 32, "A10_DMR"  };
static const CONNECTION s_AI11_CP3_i = { 33, "A11_CP3"  };
static const CONNECTION s_AI12_CP2_i = { 34, "A12_CP2"  };
static const CONNECTION s_AI13_CP1_i = { 35, "A13_CP1"  };
static const CONNECTION s_AI14_CP0_i = { 36, "A14_CP0"  };
static const CONNECTION s_AI15_VEC_i = { 37, "A15_VEC"  };
static const CONNECTION s_AI16_PF_i  = { 38, "A16_PF"   };
static const CONNECTION s_AI17_HLT_i = { 39, "A17_HLT"  };
static const CONNECTION s_Vcc_i      = { 40, "Vcc"      };


//
// Bus pins
//
static const CONNECTION s_DALHi_iot[] = { { 9, "DAL8"  },
                                          { 7, "DAL9"  },
                                          { 6, "DAL10" },
                                          { 5, "DAL11" },
                                          { 4, "DAL12" },
                                          { 3, "DAL13" },
                                          { 2, "DAL14" },
                                          { 1, "DAL15" } }; // Upper 8 of 16 bits

static const CONNECTION s_DALLo_iot[] = { {17, "DAL0" },
                                          {16, "DAL1" },
                                          {15, "DAL2" },
                                          {14, "DAL3" },
                                          {13, "DAL4" },
                                          {12, "DAL5" },
                                          {11, "DAL6" },
                                          {10, "DAL7" } }; // Lower 8 of 16 bits


CT11Cpu::CT11Cpu(
) : m_busDALHi(g_pinMap40DIL, s_DALHi_iot,  ARRAYSIZE(s_DALHi_iot)),
    m_busDALLo(g_pinMap40DIL, s_DALLo_iot,  ARRAYSIZE(s_DALLo_iot)),
    m_pinPI(g_pinMap40DIL, &s_PI_o),
    m_pinSEL0(g_pinMap40DIL, &s_SEL0_o),
    m_pinSEL1(g_pinMap40DIL, &s_SEL1_o),
    m_pinR_WHB(g_pinMap40DIL, &s_R_WHB_o),
    m_pinR_WLB(g_pinMap40DIL, &s_R_WLB_o),
    m_pin_RAS(g_pinMap40DIL, &s__RAS_o),
    m_pin_CAS(g_pinMap40DIL, &s__CAS_o),
    m_pinCOUT(g_pinMap40DIL, &s_COUT_o),
    m_pinXTL1(g_pinMap40DIL, &s_XTL1_i)
{
};

//
// The idle function sets up the pins into the correct direction (input/output)
// and idle state ready for the next bus cycle.
//
PERROR
CT11Cpu::idle(
)
{
    pinMode(g_pinMap40DIL[s_BGND_i.pin],          INPUT);
    pinMode(g_pinMap40DIL[s_PUP_i.pin],           INPUT);
    pinMode(g_pinMap40DIL[s_GND_i.pin],           INPUT);

    m_pinCOUT.digitalWrite(LOW);
    m_pinCOUT.pinMode(OUTPUT);

    m_pinXTL1.pinMode(INPUT);
    pinMode(g_pinMap40DIL[s_XTL0_i.pin],          INPUT);

    m_pinSEL1.digitalWrite(LOW);
    m_pinSEL1.pinMode(OUTPUT);

    m_pinSEL0.digitalWrite(LOW);
    m_pinSEL0.pinMode(OUTPUT);

    pinMode(g_pinMap40DIL[s_READY_i.pin],         INPUT);

    m_pinR_WHB.digitalWrite(HIGH);
    m_pinR_WHB.pinMode(OUTPUT);

    m_pinR_WLB.digitalWrite(HIGH);
    m_pinR_WLB.pinMode(OUTPUT);

    m_pin_RAS.digitalWrite(HIGH);
    m_pin_RAS.pinMode(OUTPUT);

    m_pin_CAS.digitalWrite(HIGH);
    m_pin_CAS.pinMode(OUTPUT);

    m_pinPI.digitalWrite(LOW);
    m_pinPI.pinMode(OUTPUT);

    pinMode(g_pinMap40DIL[s_AI10_DMR_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI11_CP3_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI12_CP2_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI13_CP1_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI14_CP0_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI15_VEC_i.pin],        INPUT);
    pinMode(g_pinMap40DIL[s_AI16_PF_i.pin],         INPUT);
    pinMode(g_pinMap40DIL[s_AI17_HLT_i.pin],        INPUT);

    pinMode(g_pinMap40DIL[s_Vcc_i.pin],             INPUT);

    // Use the pullup input as the float to detect shorts to ground.
    m_busDALHi.pinMode(INPUT_PULLUP);
    m_busDALLo.pinMode(INPUT_PULLUP);

    return errorSuccess;
}

//
// The check function performs a basic pin check that all the outputs can be pulled high
// by the pullup resistor as a way to detect a GND short or pulled output. It also validates
// the default state of the control pins would allow the CPU to execute instructions.
//
// If check fails the pins are left in the failing state. If check succeeds the pins are reset
// to idle state.
//
// On the T11 the timing & configuration of the external bus cycle is configured as part of reset
// based on reading in the MR (Mode Register). As a result of that the "check" function also
// performs MR verification that a supported bus configuration is selected.
//
// Only the mode settings applicable to Atari System 2 are currently supported:
// DAL(MR) = 0011 0110 1111 1111 (0x36FF)
// => Processor Clock (COUT)
// => Standard Cycle
// => Normal Read/Write
// => Static Memory
// => 16-bit
//
//
PERROR
CT11Cpu::check(
)
{
    PERROR error = errorSuccess;

    // The ground pin (with pullup) should be connected to GND (LOW)
    CHECK_VALUE_EXIT(error, s_BGND_i, LOW);
    CHECK_VALUE_EXIT(error, s_GND_i, LOW);

    // The Vcc pin should be high (power is on).
    CHECK_VALUE_EXIT(error, s_Vcc_i, HIGH);

    // The power-up (reset) pin should be low.
    CHECK_VALUE_EXIT(error, s_PUP_i, LOW);

    // The READY input is not supported and it should be HIGH (ready)
    CHECK_VALUE_EXIT(error, s_READY_i, HIGH);

    // Loop to detect a clock by sampling and detecting both high and lows.
    {
        UINT16 hiCount = 0;
        UINT16 loCount = 0;

        for (int i = 0 ; i < 1000 ; i++)
        {
            int value = ::digitalRead(g_pinMap40DIL[s_XTL1_i.pin]);

            if (value == LOW)
            {
                loCount++;
            }
            else
            {
                hiCount++;
            }
        }

        if (loCount == 0)
        {
            CHECK_VALUE_EXIT(error, s_XTL1_i, LOW);
        }
        else if (hiCount == 0)
        {
            CHECK_VALUE_EXIT(error, s_XTL1_i, HIGH);
        }
    }

    // Check that the mode register is properly set and the data bus is properly
    // pulled high (the T11 is explicit in pulling high the databus and thus this
    // is an important check on the T11).
    {
        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALHi, s_DALHi_iot, 0xFF);
        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALLo, s_DALLo_iot, 0xFF);

        ::digitalWrite(g_pinMap40DIL[s__BCLR_o.pin], LOW);
        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALHi, s_DALHi_iot, 0x36);
        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALLo, s_DALLo_iot, 0xFF);
        ::digitalWrite(g_pinMap40DIL[s__BCLR_o.pin], HIGH);

        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALHi, s_DALHi_iot, 0xFF);
        CHECK_BUS_VALUE_UINT8_EXIT(error, m_busDALLo, s_DALLo_iot, 0xFF);
    }

    // Perform the ASPI cycle to read in the control pin state on the AI lines.
    {
        m_pin_CAS.digitalWriteLOW();
        m_pinPI.digitalWriteHIGH();

        CHECK_VALUE_EXIT(error, s_AI10_DMR_i, HIGH);
        CHECK_VALUE_EXIT(error, s_AI15_VEC_i, HIGH);
        CHECK_VALUE_EXIT(error, s_AI16_PF_i,  HIGH);
        CHECK_VALUE_EXIT(error, s_AI17_HLT_i, HIGH);

        m_pin_CAS.digitalWriteHIGH();
        m_pinPI.digitalWriteLOW();
    }

Exit:
    return error;
}


PERROR
CT11Cpu::memoryReadWrite(
    UINT32 address,
    UINT16 *data,
    bool   read
)
{
    PERROR error = errorSuccess;
    bool interruptsDisabled = false;
    UINT16 data16 = 0;
    UINT32 physicalAddress = address << 1;

    bool lo        = (address & 0x010000) ? true : false;
    bool hi        = (address & 0x020000) ? true : false;
 // bool readySync = (address & 0x100000) ? true : false;

    // Drive the address
    m_busDALLo.pinMode(OUTPUT);
    m_busDALLo.digitalWrite((physicalAddress >> 0)  & 0xFF);

    m_busDALHi.pinMode(OUTPUT);
    m_busDALHi.digitalWrite((physicalAddress >> 8)  & 0xFF);

    // Drive the byte lane write strobes
    if (!read)
    {
        if (lo)
        {
            m_pinR_WLB.digitalWriteLOW();
        }

        if (hi)
        {
            m_pinR_WHB.digitalWriteLOW();
        }
    }

    // Critical timing section
    noInterrupts();
    interruptsDisabled = true;

    // Assert RAS to latch the address.
    m_pin_RAS.digitalWriteLOW();

    // Pulse COUT
    m_pinCOUT.digitalWriteHIGH();
    m_pinCOUT.digitalWriteLOW();

    if (read)
    {
        m_busDALLo.pinMode(INPUT);
        m_busDALHi.pinMode(INPUT);
    }
    else
    {
        if (lo)
        {
            // 8 & 16-bit cycle.
            m_busDALLo.digitalWrite((*data >> 0) & 0xFF);
        }

        if (hi)
        {
            if (lo)
            {
                // 16-bit cycle.
                m_busDALHi.digitalWrite((*data >> 8) & 0xFF);
            }
            else
            {
                // 8-bit cycle
                m_busDALHi.digitalWrite((*data >> 0) & 0xFF);
            }
        }
    }

    // Assert CAS to signal data
    m_pin_CAS.digitalWriteLOW();

    // Assert PI to latch the AI lines
    m_pinPI.digitalWriteHIGH();

    // Pulse COUT
    m_pinCOUT.digitalWriteHIGH();
    m_pinCOUT.digitalWriteLOW();

    // Read in the data
    if (read)
    {
        UINT16 tempData16 = 0;

        if (lo)
        {
            // 8 & 16-bit cycle.
            m_busDALLo.digitalRead(&tempData16);
            data16 |= (tempData16 & 0xFF) << 0;
        }

        if (hi)
        {
            m_busDALHi.digitalRead(&tempData16);

            if (lo)
            {
                // 16-bit cycle.
                data16 |= (tempData16 & 0xFF) << 8;
            }
            else
            {
                // 8-bit cycle
                data16 |= (tempData16 & 0xFF) << 0;
            }
        }
    }

    // Deassert CAS, PI & RAS
    m_pin_CAS.digitalWriteHIGH();
    m_pinPI.digitalWriteLOW();
    m_pin_RAS.digitalWriteHIGH();

    // Restore data state
    if (!read)
    {
        if (lo)
        {
            m_busDALLo.pinMode(INPUT_PULLUP);
            m_pinR_WLB.digitalWriteHIGH();
        }

        if (hi)
        {
            m_busDALHi.pinMode(INPUT_PULLUP);
            m_pinR_WHB.digitalWriteHIGH();
        }
    }

Exit:

    if (interruptsDisabled)
    {
        interrupts();
    }

    *data = (UINT8) data16;

    return error;
}


PERROR
CT11Cpu::memoryRead(
    UINT32 address,
    UINT8  *data
)
{
    PERROR error;
    UINT16 data16;

    error =  memoryReadWrite(address, &data16, true);
    *data = (UINT8) data16;
    return error;
}


PERROR
CT11Cpu::memoryWrite(
    UINT32 address,
    UINT8  data
)
{
    PERROR error;
    UINT16 data16 = data;

    error = memoryReadWrite(address, &data16, false);
    return error;
}


PERROR
CT11Cpu::waitForInterrupt(
    Interrupt interrupt,
    UINT16    timeoutInMs
)
{
    PERROR error = errorNotImplemented;
    return error;
}

//
// TBD.
//
PERROR
CT11Cpu::acknowledgeInterrupt(
    UINT8 *response
)
{
    PERROR error = errorSuccess;

    *response = 0;

    return error;
}
