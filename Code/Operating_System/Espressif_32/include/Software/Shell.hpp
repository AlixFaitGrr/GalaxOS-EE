///
/// @file Shell.hpp
/// @author Alix ANNERAUD (alix.anneraud@outlook.fr)
/// @brief Xila's user interface header file.
/// @version 0.1.0
/// @date 21-05-2020
/// @details Xila user interface software.
///
/// @section License
///
/// Copyright (c) 2020 Alix ANNERAUD
///
/// Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:
/// The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.
///
/// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
///
/// @copyright Copyright (c) 2021
///

#ifndef SHELL_HPP_INCLUDED
#define SHELL_HPP_INCLUDED

#include "Xila.hpp"

#define Default_Background -1

///
/// @brief Shell class
///
/// @details Main inter
///
class Shell_Class : public Software_Class
{
protected:
    // -- Attributes

    ///
    /// @brief Instance pointer, help to keep track of current instance.
    ///
    static Shell_Class *Instance_Pointer;

    float Temporary_Float;

    // Login page variable

    // -- File Manager variable

    char Current_Item_Name[13];
    char Current_Path[192];

    // -- Hardware

    uint8_t Brightness;

    uint16_t Standby_Display_Time;

    // -- Preferences system attributes

    bool Autologin;         // -- Shared with install wizard

    int32_t GMT_Offset;      // -- Shared with install wizard
    int16_t Daylight_Offset; // -- Shared with install wizard

    char NTP_Server[sizeof(Xila.Time.NTP_Server)];
    char Name[25]; // -- Shared with install wizard

    char Username[9];    // -- Shared with install wizard and login
    char Password_1[25]; // -- Shared with install wizard and login
    char Password_2[25]; // -- Shared with install wizard

    // -- Preferences network attributes

    char WiFi_Name[25];     // -- Shared with install wizard
    char WiFi_Password[65]; // -- Shared with install wizard

    char Temporary_Char[14];

    // -- Desk variables

    ///
    /// @brief Desk background
    ///
    /// @details Desk image when < 0 and Custom color when > 0 (color code itself).
    ///
    int32_t Desk_Background;

    ///
     /// @brief Pages
     /// 
    enum Pages
    {
        About = Xila.Display.Shell_Pages,
        Color_Picker, // color picker
        Desk,
        Drawer,
        Event,
        File_Manager_Main,
        File_Manager_Detail,
        Preferences_Hardware,
        Install,
        Keyboard,
        Keypad,
        Login,
        Load,
        Preferences_Network,
        Preferences_Personal,
        Shutdown,
        Preferences_System,
        Welcome,
    };

    uint8_t Offset;

    // -- Methods

    //

    void Logout();

    //

    inline void System_Update();

    // -- Preferences methods

    inline void Open_Preferences_Hardware();
    inline void Open_Preferences_Network();
    inline void Open_Preferences_System();
    inline void Open_Preferences_Personal();

    void Refresh_Preferences_Hardware();
    void Refresh_Preferences_Personal();
    void Refresh_Preferences_Network();
    void Refresh_Preferences_System();

    void Preferences_Hardware_Instructions();
    void Preferences_Network_Instructions();
    void Preferences_Personal_Instructions();
    void Preferences_System_Instructions();

    // -- Desk methods

    void Refresh_Desk();
    void Desk_Instructions();

    void Dock(uint8_t, uint8_t);

    // -- Global scope

    uint16_t Current_Command;
    void Main_Instructions();

    // -- Dialog boxes method

    inline void Open_Color_Picker();
    inline void Open_Keypad();
    inline void Open_Keyboard();

    inline void Color_Picker_Instructions();
    inline void Event_Instructions();
    inline void Keyboard_Instructions();
    inline void Keypad_Instructions();

    // -- Custom dialog boxes (Shell cannot)

    Xila_Class::Event Keyboard_Dialog(char *, size_t, bool = false);
    Xila_Class::Event Keypad_Dialog(float &Number_To_Get);
    Xila_Class::Event Event_Dialog(const __FlashStringHelper *, uint8_t, const __FlashStringHelper * = NULL, const __FlashStringHelper * = NULL, const __FlashStringHelper * = NULL);
    Xila_Class::Event Color_Picker_Dialog(uint16_t &);
    Xila_Class::Event Open_File_Dialog(File &);
    Xila_Class::Event Save_File_Dialog(File &);
    Xila_Class::Event Open_Folder_Dialog(File &);

    // -- Shutdown

    inline void Open_Shutdown();
    void Shutdown_Instructions();

    // -- Desk

    void Open_Desk();

    void Login_Instructions();
    void Install_Instructions();
    void Load_Instructions();
    void File_Manager_Instructions();

    // -- Drawer

    void Open_Drawer();
    void Refresh_Drawer();
    void Open_From_Drawer(uint8_t);
    void Drawer_Instructions();

    // -- Registry management

    Xila_Class::Event Save_Registry();
    Xila_Class::Event Load_Registry();

    // -- Installation wizard methods

    void Refresh_Install();

    inline void Open_Install();

    //

    void Open_Item();

    void Open_Login();

    void Open_Load(uint8_t Mode);

    void Refresh_Login();

    File Selected_Item;

    File Temporary_Item;

    uint8_t Operation;

    enum File_Manager_Operation
    {
        Browse,
        New_File,
        New_Folder,
        Delete,
        Rename,
        Copy,
        Cut,
        Detail
    };

    // -- Related to Xila.Instruction
    uint8_t Mode;

    // -- File manager methods

    void Select_Item();
    void Open_File_Manager();

    void Refresh_Footerbar();
    void Refresh_File_Manager();
    void Make_File();
    void Go_Parent();

    void Refresh_File_Manager_Detail();
    void Open_File_Manager_Detail();

    //

    void Set_Variable(const void *, uint8_t, uint8_t, uint8_t = 0);

    static void Main_Task(void *);

public:
    static Software_Class *Load_Shell();

    static void Startup();

    enum Images
    {
        Empty_16 = Xila.Display.Shell_Images,
        File_16,
        Folder_16,
        Copy_24,
        Cut_24,
        Home_24,
        New_File_24,
        New_Folder_24,
        Paste_24,
        Refresh_24,
        Rename_24,
        Root_24,
        Trash_24,
        Empty_32,
        File_Manager_32,
        Preferences_32,
        Shutdown_32,
        Alix_Fait_Grr_Icon_64,
        Background_Picture_480,
    };

    Shell_Class();
    ~Shell_Class();
};

Software_Handle_Class Shell_Handle("Shell", Shell_Class::Empty_32, Shell_Class::Load_Shell, Shell_Class::Startup);

#endif