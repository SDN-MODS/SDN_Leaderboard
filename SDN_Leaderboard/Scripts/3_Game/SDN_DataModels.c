// ============================================================================
// PASTA: SDN_Leaderboard/Scripts/3_Game
// ARQUIVO: SDN_DataModels.c
// ============================================================================

class SDN_LeaderboardConfig
{
    int EnableMod;
    int MaxPlayersToShow;

    int KillCooldownSeconds;
    int EnableAntiSameIP;
    int MinVictimPlaytimeSeconds;
    int AutoWipeDays;

    int LastWipeYear;
    int LastWipeMonth;
    int LastWipeDay;

    ref TStringArray BlacklistedWeapons;

    void SDN_LeaderboardConfig()
    {
        EnableMod = 1;
        MaxPlayersToShow = 10;
        KillCooldownSeconds = 120;
        EnableAntiSameIP = 1;
        MinVictimPlaytimeSeconds = 300;
        AutoWipeDays = 30;
        LastWipeYear = 0;
        LastWipeMonth = 0;
        LastWipeDay = 0;

        BlacklistedWeapons = new TStringArray;
        BlacklistedWeapons.Insert("SurvivorBase");
        BlacklistedWeapons.Insert("OffroadHatchback");
        BlacklistedWeapons.Insert("CivilianSedan");
        BlacklistedWeapons.Insert("Hatchback_02");
        BlacklistedWeapons.Insert("Truck_01_Covered");
        BlacklistedWeapons.Insert("Infected");
    }
}

class SDN_PlayerStat
{
    string PlayerUID;
    string PlayerName;
    int Kills;
    int Deaths;
    float KDRatio;
    int LongestKill;

    void SDN_PlayerStat(string uid = "", string name = "", int kills = 0, int deaths = 0, float kd = 0, int longest = 0)
    {
        PlayerUID = uid;
        PlayerName = name;
        Kills = kills;
        Deaths = deaths;
        KDRatio = kd;
        LongestKill = longest;
    }
}

// CLASSE DE PAYLOAD: Corrigida e pronta para serialização RPC
class SDN_LeaderboardPayload
{
    ref array<ref SDN_PlayerStat> TopPlayers;
    ref SDN_PlayerStat MyStat;
    int MyRank;

    void SDN_LeaderboardPayload()
    {
        TopPlayers = new array<ref SDN_PlayerStat>;
        MyRank = 0;
    }
}