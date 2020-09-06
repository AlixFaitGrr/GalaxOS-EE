#ifndef GALAXOS_CORE_H_INCLUDED
#define GALAXOS_CORE_H_INCLUDED

#include "Configuration.hpp"

//----------------------------------------------------------------------------//
//                          Include Necessary Libraries                       //
//----------------------------------------------------------------------------//

#include "Arduino.h"

#if SD_MODE == 0
#include "SD_MMC.h"
#include "FS.h"
#else
#include <SD.h>
#endif

#include "time.h"
#include "Update.h"
#include <ArduinoJson.h> //used to store
#include "WiFi.h"

//----------------------------------------------------------------------------//
//                          Include All Project File                          //
//----------------------------------------------------------------------------//

// Other part of the core

#include "Software.hpp"

// Driver files
#include "Display.hpp"  // Nextion display driver (maybe create a library for each driver)
#include "Keyboard.hpp" // PS2 keyboard driver
#include "Sound.hpp"

//----------------------------------------------------------------------------//
//                                Define Const                                //
//----------------------------------------------------------------------------//

// 
#define MAXIMUM_SOFTWARE 15

// Page ID Index
#define PAGE_SPLASH_A 0
#define PAGE_SPLASH_B 1
#define PAGE_ARDUINO_HOME 2
#define PAGE_DESK 19
#define PAGE_EVENT 20
#define PAGE_IGOS 27
#define PAGE_PIANO 38
#define PAGE_MENU_1 

// Color

#define COLOR_DARK_GREY 16904
#define COLOR_LIGHT_GREY 33808
#define COLOR_WHITE 65535

// Event Index (used to interract with the event handler)

// Informations
#define INFORMATION_GOOD_CREDENTIALS 39548

// Questions
#define QUESTION_DO_YOU_WANT_TO_CLOSE_ALL_RUNNING_SOFTWARE 3565

// Warnings
#define WARNING_WRONG_PASSWORD 28364
#define WARNING_WRONG_USERNAME 54112
#define WARNING_DO_YO_REALLY_WANT_TO_DELETE_THIS_ITEM 43345

// Errors
#define ERROR_FAILLED_TO_INTIALIZE_SD_CARD 10896
#define ERROR_SOME_SYSTEM_FILES_ARE_MISSING 49361
#define ERROR_SOME_SYSTEM_FILES_ARE_CORRUPTED 60041
#define ERROR_SOME_USER_SETTINGS_FILES_ARE_MISSING 25814
#define ERROR_SOME_USER_SETTINGS_FILES_ARE_CORRUPTED 12733
#define ERROR_THE_FILE_DO_NOT_EXIST 7018
#define ERROR_CANNOT_CREATE_SYSTEM_QUEUE 17496
#define ERROR_UNEXPECTED_RETURN_COMMAND 46201
#define ERROR_CANNOT_WRITE_DATA_TO_DISK_RAM 8942
#define ERROR_INVALID_SOFTWARE_ID 4562
#define ERROR_CANNOT_OPEN_REGISTRY_FILE 684
#define ERROR_REGISTRY_FILE_DOES_NOT_EXIST 8404
#define ERROR_TO_MUCH_OPENNED_SOFTWARE 7519

// Alignement
#define STYLE_LEFT_ALIGNMENT 0
#define STYLE_CENTER_ALIGNMENT 1
#define STYLE_RIGHT_ALIGNMENT 2
#define STYLE_JUSTIFIED_ALIGNMENT 3

// Nextion command
#define CODE_COMMAND 42                     // * : Command
#define CODE_COMMAND_NEW 35                 // # : Command
#define CODE_VARIABLE_BYTE 66               // B : 1 byte
#define CODE_VARIABLE_INTEGER 73            // I : 2 bytes
#define CODE_VARIABLE_LONG 76               // L : 4 bytes
#define CODE_VARIABLE_LONG_LONG 108         // l : 8 bytes
#define CODE_VARIABLE_STRING 0x53           // S : String (undefined size)

// System's path
#define SYSTEM_PATH "/GALAXOS/"
#define REGISTRY_PATH "REGISTRY/"
#define GLOBAL_VIRTUAL_MEMORY_FILE "/GALAXOS/MEMORY/VARIABLE.GSF"

// GRF : Galax'OS Registry File
// GEF : Galax'OS Executable File
// GSF : Galax'OS Sound File


//----------------------------------------------------------------------------//
//                         Define GalaxOS Core Class                          //
//----------------------------------------------------------------------------//

class GalaxOS_Class
{
protected:
    // Virtual Memory File

    File Virtual_Memory_File;
    uint8_t Split_Number[8];
    SemaphoreHandle_t Virtual_Memory_Semaphore;


    byte C_MIDI;

    int C_Frequency;

    int Low_RAM_Threshold;

    //User attribute
    char *Current_Username;

    String Temporary_String;

    Software_Class *Open_Software_Pointer[6];
    Software_Handle_Class *Software_Handle_Pointer[MAXIMUM_SOFTWARE];

    uint8_t Event_Reply;

    Software_Class *Software_Pointer[6]; //current running software

    char Tag;

    struct tm Time;

    char WiFi_SSID[20];
    char WiFi_Password[20];

    xTaskHandle Nextion_Serial_Receive_Handle;

    xTaskHandle Ressource_Monitor_Handle;
    xTaskHandle GalaxOS_Core_Handle;

    //Software management

    Software_Class *Get_Software_Pointer(const char *);
    Software_Handle_Class *Get_Software_Handle_Pointer(const char *Software_Name);
    void Set_Software_Pointer(const char*, Software_Class *);
    void Set_Software_Handle_Pointer(const char*, Software_Handle_Class *);

    void Open_Software(const char*);
    void Close_Software(const char*);

public:

    GalaxOS_Class();
    ~GalaxOS_Class();

    // Drivers
    // Display
    Nextion_Display_Class Display;
    // Sound
    Sound_Class Sound;
    // Input
    Keyboard_Class Keyboard;
    // Disk
    #if SD_MODE == 0
        fs::SDMMCFS Drive;
    #else
        fs::SDFS Drive;
    #endif
    // WiFi



    void Start();
    void Save_System_State(); //Save system state in a file, in case of binary loading or hiberte, in order to restore the last system state. Start routine check always if a "GOSH.GSF"
    void Restore_System_State();

    void Synchronise_Time();

    // Software Management

    void Set_Load_Function(const char *Software_Name, void (*Load_Function_To_Set)()); // Used by softwa

    // Display callback function
    void Incomming_String_Data_From_Display(String &Received_Data);
    void Incomming_Numeric_Data_From_Display(uint32_t const &Received_Data);
    void Incomming_Event_From_Display(uint16_t);

    // Serial communication macro
    void Horizontal_Separator();
    void Print_Line(const char *Text_To_Print, uint8_t Alignement);

    //
    byte Get_Speaker_Pin();
    int Get_C_Frequency();
    byte Get_C_MIDI();

    //
    void Set_Variable(char const &Tag, String const &String_To_Set);
    void Get_Variable(char const &Tag, String &String_To_Get);

    void Set_Variable(char const &Tag, const char *String_To_Set);
    void Get_Variable(char const &Tag, char *String_To_Get);

    void Set_Variable(char const &Tag, uint8_t const &Number_To_Set);
    void Get_Variable(char const &Tag, uint8_t &Number_To_Set);

    void Set_Variable(char const &Tag, uint16_t const &Number_To_Set);
    void Get_Variable(char const &Tag, uint16_t &Number_To_Set);

    void Set_Variable(char const &Tag, uint32_t const &Number_To_Set);
    void Get_Variable(char const &Tag, uint32_t &Number_To_Set);

    void Set_Variable(char const &Tag, uint64_t const &Number_To_Set);
    void Get_Variable(char const &Tag, uint64_t &Number_To_Set);

    char *Get_Current_Username()
    {
        return Current_Username;
    }

    void Registry_Find(File &Registry_File, const char *Key_Name, char *Key_Value_To_Get, uint16_t const &Column = 0);
    char *Registry_Read(File &Registry_File, uint16_t const &Line_Number, uint16_t const &Column_Number);
    void Registry_Write(const __FlashStringHelper *Path, const __FlashStringHelper *Key_Name, String &Key_Value_To_Get);
    void Registry_Add(const __FlashStringHelper *Path, const __FlashStringHelper *Key_Name, String &Key_Value_To_Set);
    void Registry_Modify(const __FlashStringHelper *Path, const __FlashStringHelper *Key_Name, String &Key_Value_To_Set);
    void Registry_Delete(const __FlashStringHelper *Path, const __FlashStringHelper *Key_Name);

    void Open_File(File &File_To_Open);

    void Load_System_Files();
    void Load_User_Files();

    uint16_t Check_Credentials(const char *Username_To_Check, const char *Password_To_Check);

    //services
    void Desk_Execute(uint16_t const& Command);

    uint8_t Event_Handler(uint16_t const &Type, String const &Extra_Informations = "");
    friend void Ressource_Monitor(void *pvParameters);
    friend class Shell_Class;

    void Nextion_Upload_Firmware(String const &Path);
};

//GalaxOS class's method (FreeRTOS seems to not support class/struct)
void Ressource_Monitor(void *pvParameters);

void Periodic_Main(byte Type);
void Piano(int Frequency, int Note);
void Pictureader();

void UltraSonic(int USTrig, int USEcho);

#endif