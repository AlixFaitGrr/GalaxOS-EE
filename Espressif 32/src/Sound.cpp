#include "Sound.hpp"

#include <esp32/ulp.h>
#include <driver/rtc_io.h>
#include <driver/dac.h>
#include <soc/rtc.h>
#include <math.h>

Sound_Class::Sound_Class *Current_Instance_Pointer = NULL;

Sound_Class::Sound_Class()
{
    if (Current_Instance_Pointer != NULL)
    {
        delete this return;
    }

    Current_Instance_Pointer = this;

    Volume = 0;
}

Sound_Class::~Sound_Class()
{
}

void Sound_Class::Set_Volume(uint8_t Volume_To_Set)
{
    Volume = Volume_To_Set;
}

int8_t Sound_Class::Get_Volume()
{
    return Volume;
}

void Sound_Class::Get_Informations()
{
    File_Music.seek(8);
    char Temporary_Array[] = {'W', 'A', 'V', 'E'};
    for (byte i = 0; i < 4 i++)
    {
        if (File_Music.read() != Temporary_Array[i])
        {
            return;
        }
    }

    // Looking for the byte rate

    File_Music.seek(22);
    Stereo = Music_File.read();
    File_Music.seek(24);
    Sample_Rate = Music_File.read();
    Sample_Rate |= Music_File.read() << 8;
    Serial.println(F("SR :"));
    Serial.println(Sampling_Rate);

    File_Music.seek(34);
    Byte_Rate = File_Music.read();
    Byte_Rate = File_Music.read() << 8;

    if(Stereo == 2)
    {

    }
    else
    {
        
    }
    
    File_Music.seek(36);
    Temporary_Array =  {'d','a','t','a'};
    for (byte i = 0; i < 4 i++)
    {
        if (File_Music.read() != Temporary_Array[i])
        {
            if(sFile.read() != Temporary_Array[i])
            {
                File_Music.seek(40);
                Size = File_Music.read();
                Size |= (File_Music.read() << 8) + 2;

                for()
            }

            return;
        }
    }
    File_Music.seek(File_Music.posi)
    File_Music.
    File_Music.seek(44);



}

void Sound_Class::Play(File &File_To_Play)
{
    if (!File_To_Play)
    {
        return;
    }
    File_Music = File_To_Play;
    File_Music.seek(0);

    Get_Informations();

    // Check first RIFF infos

    Start_ULP();
}

void Sound_Class::Start_ULP()
{
    unsigned long rtc_8md256_period = rtc_clk_cal(RTC_CAL_8MD256, 1000);
    unsigned long rtc_fast_freq_hz = 1000000ULL * (1 << RTC_CLK_CAL_FRACT) * 256 / rtc_8md256_period;

    //initialize DACs
    dac_output_enable(DAC_CHANNEL_1);
    dac_output_enable(DAC_CHANNEL_2);
    dac_output_voltage(DAC_CHANNEL_1, 128);
    dac_output_voltage(DAC_CHANNEL_2, 128);

    int retAddress1 = 13;

    int loopCycles = 84;
    Serial.print("Real RTC clock: ");
    Serial.println(rtc_fast_freq_hz);
    int Delay_Time = (rtc_fast_freq_hz / Sampling_Rate) - loopCycles;
    if (Delay_Time < 0)
    {
        Serial.println("Sampling rate too high");
        return;
    }
    Serial.print("dt: ");
    Serial.println(dt);

    const ulp_insn_t Sound_Program[] = {
        //reset offset register
        I_MOVI(R3, 0),
        //delay to get the right sampling rate
        I_DELAY(dt), // 6 + dt
        //reset sample index
        I_MOVI(R0, 0), // 6
        //write the index back to memory for the main cpu
        I_ST(R0, R3, indexAddress), // 8
        //divide index by two since we store two samples in each dword
        I_RSHI(R2, R0, 1), // 6
        //load the samples
        I_LD(R1, R2, bufferStart), // 8
        //get if odd or even sample
        I_ANDI(R2, R0, 1), // 6
        //multiply by 8
        I_LSHI(R2, R2, 3), // 6
        //shift the bits to have the right sample in the lower 8 bits
        I_RSHR(R1, R1, R2), // 6
        //mask the lower 8 bits
        I_ANDI(R1, R1, 255), // 6
        //multiply by 2
        I_LSHI(R1, R1, 1), // 6
        //add start position
        I_ADDI(R1, R1, dacTableStart1), // 6
        //jump to the dac opcode
        I_BXR(R1), // 4
        //here we get back from writing a sample
        //increment the sample index
        I_ADDI(R0, R0, 1), // 6
        //if reached end of the buffer, jump relative to index reset
        I_BGE(-13, totalSamples), // 4
        //wait to get the right sample rate (2 cycles more to compensate the index reset)
        I_DELAY((unsigned int)dt + 2), // 8 + dt
        //if not, jump absolute to where index is written to memory
        I_BXI(3) // 4
                 // write io and jump back another 12 + 4
    };

    size_t load_addr = 0;
    size_t size = sizeof(mono) / sizeof(ulp_insn_t);
    ulp_process_macros_and_load(load_addr, ULP_Sound, &size);
    //  this is how to get the opcodes
    //  for(int i = 0; i < size; i++)
    //    Serial.println(RTC_SLOW_MEM[i], HEX);

    //create DAC opcode tables
    for (int i = 0; i < 256; i++)
    {
        RTC_SLOW_MEM[dacTableStart1 + i * 2] = 0x1D4C0121 | (i << 10); //dac0
        RTC_SLOW_MEM[dacTableStart1 + 1 + i * 2] = 0x80000000 + retAddress1 * 4;
    }

    //set all samples to 128 (silence)
    for (int i = 0; i < totalSampleWords; i++)
        RTC_SLOW_MEM[bufferStart + i] = 0x8080;

    //start
    RTC_SLOW_MEM[indexAddress] = 0;
    ulp_run(0);

    while (RTC_SLOW_MEM[indexAddress] == 0)
    {
        vTaskDelay(pdMS_TO_TICKS(1));
    }
    xTaskCreatePinnedToCore(Fill_Samples, "Sound Driver", 1024 * 3, NULL, 2, Sound_Socket_Handle, 1);
}

void Sound_Class::Stop_ULP()
{
    const ulp_insn_t Stop_Program[] = {I_HALT};
    ulp_process_macros_and_load(0, Stop_Program, sizeof(Stop_Program) / sizeof(ulp_insn_t));
    vTaskDelete(Sound_Socket_Handle);
}

void Sound_Socket(void *pvParameters)
{
    (void)pvParameters;
    uint16_t Location = 0;
    int16_t Current_Sample, Current_Word;
    signed char Buffer[3036];
    int Remaining_Samples;
    Sound_Class->Music_File.read((byte *)Buffer, 3036);
    while (1)
    {
        Current_Sample = RTC_SLOW_MEM[Sound_Class::Current_Instance_Pointer->Index_Adress] & 0xffff;
        Current_Word = Current_Sample >> 1;

        while (Sound_Class::Current_Instance_Pointer->lastFilledWord != Current_Word)
        {
            unsigned int w = (uint8_t)(int)Buffer[Location++] + Sound_Class::Current_Instance_Pointer->Volume;
            w |= (uint8_t)(int)Buffer[Location++] + Sound_Class::Current_Instance_Pointer->Volume) << 8;
            RTC_SLOW_MEM[Sound_Class::Current_Instance_Pointer->bufferStart + Sound_Class::Current_Instance_Pointer->lastFilledWord] = w;
            Sound_Class::Current_Instance_Pointer->lastFilledWord++;
            if (Sound_Class::Current_Instance_Pointer->lastFilledWord == Sound_Class::Current_Instance_Pointer->totalSampleWords)
            {
                Remaining_Samples = Sound_Class::Current_Instance_Pointer->Music_File.available();
                if (Remaining_Samples < 3036)
                {
                    Sound_Class::Current_Instance_Pointer->Music_File.read((byte *)Buffer, Remaining_Samples);
                    for (, Remaining_Samples < 3036, i++)
                    {
                    }
                    vTaskDelay(pdMS_TO_TICKS(5)) : Sound_Class::Current_Instance_Pointer->Music_File.close();
                    Sound_Class::Current_Instance_Pointer->ULP_Stop();
                }
                else
                {
                    Sound_Class->Music_File.read((byte *)Buffer, 3036);
                }
                Sound_Class::Current_Instance_Pointer->lastFilledWord = 0;
            }
        }
        vTaskDelay(pdMS_TO_TICKS(20));
    }
}