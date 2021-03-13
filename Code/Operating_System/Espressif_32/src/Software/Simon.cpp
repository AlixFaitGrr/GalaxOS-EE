#include "Software/Simon.hpp"

Simon_Class *Simon_Class::Instance_Pointer = NULL;

Simon_Class::Simon_Class() : Software_Class(Simon_Handle),
                             Speed(0)
{
    Xila.Task_Create(Main_Task, "Simon Task", Memory_Chunk(5), NULL, &Task_Handle);
}

Simon_Class::~Simon_Class()
{
    if (Instance_Pointer != this)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = NULL;
}

Software_Class *Simon_Class::Load()
{
    if (Instance_Pointer != NULL)
    {
        delete Instance_Pointer;
    }
    Instance_Pointer = new Simon_Class();
    return Instance_Pointer;
}

void Simon_Class::Main_Task(void *pvParameters)
{
    while (1)
    {
        switch (Instance_Pointer->Get_Instruction())
        {
        case 0:
            Xila.Delay(30);
            break;
        case Xila.Watchdog:
            Xila.Feed_Watchdog();
            break;
        case Xila.Close:
            Instance_Pointer->Save_Registry();
            delete Instance_Pointer;
            Xila.Task_Delete();
            break;
        case Xila.Open:
            Instance_Pointer->Load_Registry();
            Xila.Display.Set_Current_Page(F("Simon"));
            Instance_Pointer->Refresh_Interface();
            break;
        case Xila.Maximize:
            Xila.Display.Set_Current_Page(F("Simon"));
            Instance_Pointer->Refresh_Interface();
            break;
        case Xila.Minimize:
            break;
        case Instruction('M', 'i'):
            Xila.Software_Minimize(Simon_Handle);
            break;
        case Instruction('C', 'l'):
            Xila.Software_Close(Simon_Handle);
            break;
        case Instruction('R', 'e'):
            Instance_Pointer->Press(Red);
            break;
        case Instruction('G', 'r'):
            Instance_Pointer->Press(Green);
            break;
        case Instruction('B', 'l'):
            Instance_Pointer->Press(Blue);
            break;
        case Instruction('Y', 'e'):
            Instance_Pointer->Press(Yellow);
            break;
        case Instruction('S', 't'): // start / stop
            if (Instance_Pointer->Speed == 0)
            {
                Instance_Pointer->Generate_Sequence();
                Xila.Display.Set_Text(F("STARTSTOP_BUT"), F("Stop"));
                Instance_Pointer->Show_Sequence();
            }
            else
            {
                Instance_Pointer->Game_Over();
                Xila.Display.Set_Text(F("STARTSTOP_BUT"), F("Start"));
            }
            break;

        default:
            break;
        }
    }
}

void Simon_Class::Refresh_Interface()
{
    Xila.Display.Set_Value(F("SCORE1_NUM"), Scores[0]);
    Xila.Display.Set_Value(F("SCORE2_NUM"), Scores[1]);
    Xila.Display.Set_Value(F("SCORE3_NUM"), Scores[2]);
    Xila.Display.Set_Value(F("SCORE4_NUM"), Scores[3]);
    Xila.Display.Set_Value(F("SCORE5_NUM"), Scores[4]);
    Xila.Display.Set_Value(F("SCORE6_NUM"), Scores[5]);
    Xila.Display.Set_Value(F("SCORE7_NUM"), Scores[6]);
    Xila.Display.Set_Value(F("SCORE_NUM"), Current_Level[1]);
}

void Simon_Class::Load_Registry()
{
    memset(Scores, '\0', sizeof(Scores));
    File Temporary_File = Xila.Drive->open(Simon_Registry_Path);
    DynamicJsonDocument Simon_Registry(256);
    if (deserializeJson(Simon_Registry, Temporary_File) != DeserializationError::Ok)
    {
        Xila.Event_Dialog(F("Failed to load registry."), Xila.Error);
        Temporary_File.close();
        return;
    }
    Scores[0] = Simon_Registry["Scores"][0];
    Scores[1] = Simon_Registry["Scores"][1];
    Scores[2] = Simon_Registry["Scores"][2];
    Scores[3] = Simon_Registry["Scores"][3];
    Scores[4] = Simon_Registry["Scores"][4];
    Scores[5] = Simon_Registry["Scores"][5];
    Scores[6] = Simon_Registry["Scores"][6];
    Temporary_File.close();
}

void Simon_Class::Save_Registry()
{
    File Temporary_File = Xila.Drive->open(Simon_Registry_Path, FILE_WRITE);
    DynamicJsonDocument Simon_Registry(256);
    Simon_Registry["Scores"][0] = Scores[0];
    Simon_Registry["Scores"][1] = Scores[1];
    Simon_Registry["Scores"][2] = Scores[2];
    Simon_Registry["Scores"][3] = Scores[3];
    Simon_Registry["Scores"][4] = Scores[4];
    Simon_Registry["Scores"][5] = Scores[5];
    Simon_Registry["Scores"][6] = Scores[6];

    if (serializeJson(Simon_Registry, Temporary_File) == 0)
    {
        Xila.Event_Dialog(F("Failed to save registry."), Xila.Error);
        Temporary_File.close();
        return;
    }
    Temporary_File.close();
}

void Simon_Class::Press(uint8_t Color)
{
    Serial.print(Current_Level[0]);
    Serial.print("|");
    Serial.println(Current_Level[1]);

    if (Speed == 0)
    {
        return;
    }

    switch (Color)
    {
    case Red:
        Xila.Sound.Tone(523, Speed);
        break;
    case Green:
        Xila.Sound.Tone(659, Speed);
        break;
    case Blue:
        Xila.Sound.Tone(784, Speed);
        break;
    case Yellow:
        Xila.Sound.Tone(988, Speed);
        break;
    default:
        break;
    }

    if (Sequence[Current_Level[1]] != Color)
    {
        Verbose_Print_Line("Wrong color");
        Xila.Event_Dialog(F("Game over !"), Xila.Information);
        Refresh_Interface();
        Game_Over();
        return;
    }

    Current_Level[1]++;
    Refresh_Interface();
    Verbose_Print_Line("Correct color");

    if (Current_Level[1] == Current_Level[0])
    {
        Verbose_Print_Line("Sequence ending");
        if (Current_Level[1] >= MAXIMUM_LEVEL)
        {
            Win();
            return;
        }
        Current_Level[1] = 0;
        Current_Level[0]++;
        Xila.Display.Set_Value(F("LEVEL_NUM"), Current_Level[0]);
        Speed -= 20;
        Show_Sequence();
    }
}

void Simon_Class::Reset()
{
    Current_Level[0] = 1;
    Current_Level[1] = 0;
    Speed = 0;
}

void Simon_Class::Win()
{
    Xila.Event_Dialog(F("Well done ! You have max out the game."), Xila.Information);
    Scores[7] = Current_Level[0];
    Refresh_Interface();
    Save_Registry();
    Refresh_Interface();
    Reset();
}

void Simon_Class::Sort_Scores()
{
    uint8_t Temporary_Byte;
    for (uint8_t i = 1; i < sizeof(Scores); i++)
    {
        if (Scores[i] < Scores[i - 1])
        {
            Temporary_Byte = Scores[i - 1];
            Scores[i - 1] = Scores[i];
            Scores[i] = Temporary_Byte;
            i = 1;
        }
    }

    Serial.print(Scores[0]);
    Serial.print(Scores[1]);
    Serial.print(Scores[2]);
    Serial.print(Scores[3]);
    Serial.print(Scores[4]);
    Serial.print(Scores[5]);
    Serial.print(Scores[6]);
    Serial.print(Scores[7]);

    /*
    for (uint8_t i = 0; i < (sizeof(Scores) - 1); i++)
    {
        for (uint8_t j = 0; i < (sizeof(Scores) - (i + 1)); j++)
        {
            if (Scores[j] > Scores[j + 1])
            {
                uint8_t k = Scores[j];
                Scores[j] = Scores[j + 1];
                Scores[j + 1] = k;
            }
        }
    }*/
}

void Simon_Class::Game_Over()
{
    Scores[7] = Current_Level[0];
    Sort_Scores();
    Save_Registry();
    Refresh_Interface();
    Reset();
}

void Simon_Class::Show_Sequence()
{
    for (uint8_t i = 0; i < Current_Level[0]; i++)
    {
        switch (Sequence[i])
        {
        case Red:
            Xila.Display.Set_Background_Color(F("RED_BUT"), 40960);
            Xila.Sound.Tone(523, Speed);
            Xila.Delay(Speed);
            Xila.Display.Set_Background_Color(F("RED_BUT"), 57344);
            Xila.Delay(200);
            break;
        case Green:
            Xila.Display.Set_Background_Color(F("GREEN_BUT"), 17408);
            Xila.Sound.Tone(659, Speed);
            Xila.Delay(Speed);
            Xila.Display.Set_Background_Color(F("GREEN_BUT"), 34308);
            Xila.Delay(200);
            break;
        case Blue:
            Xila.Display.Set_Background_Color(F("BLUE_BUT"), 780);
            Xila.Sound.Tone(784, Speed);
            Xila.Delay(Speed);
            Xila.Display.Set_Background_Color(F("BLUE_BUT"), 1300);
            Xila.Delay(200);
            break;
        case Yellow:
            Xila.Display.Set_Background_Color(F("YELLOW_BUT"), 48000);
            Xila.Sound.Tone(988, Speed);
            Xila.Delay(Speed);
            Xila.Display.Set_Background_Color(F("YELLOW_BUT"), 64896);
            Xila.Delay(200);

            break;
        default:
            Xila.Event_Dialog(F("Exception in the generated sequence."), Xila.Error);
            Refresh_Interface();
            Reset();
            break;
        }
    }
}

void Simon_Class::Generate_Sequence()
{
    Current_Level[0] = 1;
    Current_Level[1] = 0;
    Speed = 1000;
    for (uint8_t i = 0; i < (MAXIMUM_LEVEL - 1); i++)
    {
        Sequence[i] = uint8_t(esp_random() % 4);
    }
}