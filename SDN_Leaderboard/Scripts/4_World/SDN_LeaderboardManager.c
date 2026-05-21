// ============================================================================
// PASTA: SDN_MODS/SDN_Leaderboard/Scripts/4_World
// ARQUIVO: SDN_LeaderboardManager.c
// ============================================================================

class SDN_LeaderboardManager
{
    static ref SDN_LeaderboardManager m_Instance;

    static SDN_LeaderboardManager GetInstance()
    {
        if (!m_Instance)
        {
            m_Instance = new SDN_LeaderboardManager();
        }
        return m_Instance;
    }

    static const string MOD_FOLDER = "$profile:SDN_MODS/";
    static const string CONFIG_DIR = MOD_FOLDER + "SDN_Leaderboard/";
    static const string CONFIG_PATH = CONFIG_DIR + "Config.json";
    static const string DATA_PATH = CONFIG_DIR + "PlayerData.json";

    ref SDN_LeaderboardConfig m_Config;
    ref array<ref SDN_PlayerStat> m_PlayerStats;
    ref map<string, int> m_KillCooldownTracker;

    ref ScriptInvoker m_OnDataReceived;

    void SDN_LeaderboardManager()
    {
        m_Config = new SDN_LeaderboardConfig;
        m_PlayerStats = new array<ref SDN_PlayerStat>;
        m_KillCooldownTracker = new map<string, int>;
        m_OnDataReceived = new ScriptInvoker();
        Init();
    }

    void Init()
    {
        if (!FileExist(MOD_FOLDER)) MakeDirectory(MOD_FOLDER);
        if (!FileExist(CONFIG_DIR)) MakeDirectory(CONFIG_DIR);

        LoadConfig();
        CheckAutoWipe();
        LoadData();
    }

    void RequestData(CallType type, ParamsReadContext ctx, PlayerIdentity sender, Object target)
    {
        if (type == CallType.Server && sender)
        {
            SendLeaderboardDataToPlayer(sender);
        }
    }

    void SDN_ProcessPayload(SDN_LeaderboardPayload payload)
    {
        if (m_OnDataReceived && payload)
        {
            m_OnDataReceived.Invoke(payload);
        }
    }

    void SendLeaderboardDataToPlayer(PlayerIdentity identity)
    {
        if (!identity) return;

        PlayerBase targetPlayer = PlayerBase.Cast(identity.GetPlayer());
        if (!targetPlayer) return; // Evita Access Violation se o player desconectar rápido

        array<ref SDN_PlayerStat> sortedStats = new array<ref SDN_PlayerStat>;
        for (int i = 0; i < m_PlayerStats.Count(); i++)
        {
            if (m_PlayerStats.Get(i)) sortedStats.Insert(m_PlayerStats.Get(i));
        }

        int n = sortedStats.Count();
        for (int a = 0; a < n - 1; a++)
        {
            for (int b = 0; b < n - a - 1; b++)
            {
                if (sortedStats.Get(b) && sortedStats.Get(b + 1) && sortedStats.Get(b).Kills < sortedStats.Get(b + 1).Kills)
                {
                    SDN_PlayerStat temp = sortedStats.Get(b);
                    sortedStats.Set(b, sortedStats.Get(b + 1));
                    sortedStats.Set(b + 1, temp);
                }
            }
        }

        ScriptRPC rpc = new ScriptRPC();

        int max = m_Config.MaxPlayersToShow;
        if (n < max) max = n;

        rpc.Write(max);
        for (int k = 0; k < max; k++)
        {
            SDN_PlayerStat stat = sortedStats.Get(k);
            if (stat)
            {
                rpc.Write(stat.PlayerName);
                rpc.Write(stat.Kills);
                rpc.Write(stat.Deaths);
                rpc.Write(stat.KDRatio);
                rpc.Write(stat.LongestKill);
            }
        }

        SDN_PlayerStat myStat = null;
        int myRank = -1;
        for (int r = 0; r < n; r++)
        {
            if (sortedStats.Get(r) && sortedStats.Get(r).PlayerUID == identity.GetPlainId())
            {
                myStat = sortedStats.Get(r);
                myRank = r + 1;
                break;
            }
        }

        bool hasMyStat = (myStat != null);
        rpc.Write(hasMyStat);
        if (hasMyStat && myStat)
        {
            rpc.Write(myRank);
            rpc.Write(myStat.PlayerName);
            rpc.Write(myStat.Kills);
            rpc.Write(myStat.Deaths);
            rpc.Write(myStat.KDRatio);
            rpc.Write(myStat.LongestKill);
        }

        // CORREÇÃO CRÍTICA: O alvo deve ser o targetPlayer para que o OnRPC() seja chamado no cliente
        rpc.Send(targetPlayer, 858502, true, identity);
    }

    void SDN_OnRPCData(ParamsReadContext ctx)
    {
        ref SDN_LeaderboardPayload payload = new SDN_LeaderboardPayload();

        int count = 0;
        if (!ctx.Read(count)) return;

        for (int i = 0; i < count; i++)
        {
            string name; int kills, deaths, longest; float kd;
            ctx.Read(name); ctx.Read(kills); ctx.Read(deaths); ctx.Read(kd); ctx.Read(longest);
            payload.TopPlayers.Insert(new SDN_PlayerStat("", name, kills, deaths, kd, longest));
        }

        bool hasMyStat = false;
        if (ctx.Read(hasMyStat) && hasMyStat)
        {
            ctx.Read(payload.MyRank);
            string mName; int mKills, mDeaths, mLongest; float mKd;
            ctx.Read(mName); ctx.Read(mKills); ctx.Read(mDeaths); ctx.Read(mKd); ctx.Read(mLongest);
            payload.MyStat = new SDN_PlayerStat("", mName, mKills, mDeaths, mKd, mLongest);
        }

        SDN_ProcessPayload(payload);
    }

    void CheckAutoWipe()
    {
        if (m_Config.AutoWipeDays <= 0) return;

        int cYear, cMonth, cDay;
        GetYearMonthDay(cYear, cMonth, cDay);

        if (m_Config.LastWipeYear == 0)
        {
            m_Config.LastWipeYear = cYear;
            m_Config.LastWipeMonth = cMonth;
            m_Config.LastWipeDay = cDay;
            SaveConfig();
            return;
        }

        int daysPassed = (cYear - m_Config.LastWipeYear) * 365 + (cMonth - m_Config.LastWipeMonth) * 30 + (cDay - m_Config.LastWipeDay);

        if (daysPassed >= m_Config.AutoWipeDays)
        {
            m_PlayerStats.Clear();
            SaveData();

            m_Config.LastWipeYear = cYear;
            m_Config.LastWipeMonth = cMonth;
            m_Config.LastWipeDay = cDay;
            SaveConfig();
        }
    }

    bool IsValidKill(PlayerBase killer, PlayerBase victim, string weaponClass, float distance)
    {
        if (!m_Config.EnableMod) return false;
        if (!killer || !victim || killer == victim) return false;

        string killerUID = killer.GetIdentity().GetPlainId();
        string victimUID = victim.GetIdentity().GetPlainId();

        foreach (string blacklisted : m_Config.BlacklistedWeapons)
        {
            if (weaponClass.Contains(blacklisted)) return false;
        }

        int currentTime = GetGame().GetTime();
        if (m_Config.MinVictimPlaytimeSeconds > 0)
        {
            int timeAliveSeconds = (currentTime - victim.m_SDN_SpawnTime) / 1000;
            if (timeAliveSeconds < m_Config.MinVictimPlaytimeSeconds) return false;
        }

        if (m_Config.KillCooldownSeconds > 0)
        {
            string trackKey = killerUID + "_" + victimUID;
            if (m_KillCooldownTracker.Contains(trackKey))
            {
                int lastKillTime = m_KillCooldownTracker.Get(trackKey);
                int timeSinceLastKill = (currentTime - lastKillTime) / 1000;

                if (timeSinceLastKill < m_Config.KillCooldownSeconds) return false;
            }
            m_KillCooldownTracker.Set(trackKey, currentTime);
        }

        return true;
    }

    void ProcessKillEvent(PlayerBase killer, PlayerBase victim, string weaponClass, float distance)
    {
        if (!killer || !victim || !killer.GetIdentity() || !victim.GetIdentity()) return;

        SDN_PlayerStat vStat = GetOrCreatePlayerStat(victim.GetIdentity().GetPlainId(), victim.GetIdentity().GetName());
        vStat.Deaths++;
        vStat.KDRatio = CalculateKD(vStat.Kills, vStat.Deaths);

        if (!IsValidKill(killer, victim, weaponClass, distance))
        {
            SaveData();
            return;
        }

        SDN_PlayerStat kStat = GetOrCreatePlayerStat(killer.GetIdentity().GetPlainId(), killer.GetIdentity().GetName());
        kStat.Kills++;
        kStat.KDRatio = CalculateKD(kStat.Kills, kStat.Deaths);

        int distInt = Math.Round(distance);
        if (distInt > kStat.LongestKill)
        {
            kStat.LongestKill = distInt;
        }

        SaveData();
    }

    float CalculateKD(int k, int d)
    {
        if (d > 0) return (float)k / (float)d;
        return (float)k;
    }

    SDN_PlayerStat GetOrCreatePlayerStat(string uid, string name)
    {
        foreach (SDN_PlayerStat stat : m_PlayerStats)
        {
            if (stat.PlayerUID == uid)
            {
                if (stat.PlayerName != name) stat.PlayerName = name;
                return stat;
            }
        }
        SDN_PlayerStat newStat = new SDN_PlayerStat(uid, name, 0, 0, 0.0, 0);
        m_PlayerStats.Insert(newStat);
        return newStat;
    }

    void LoadConfig()
    {
        if (FileExist(CONFIG_PATH))
        {
            JsonFileLoader<SDN_LeaderboardConfig>.JsonLoadFile(CONFIG_PATH, m_Config);
        }
        else
        {
            m_Config = new SDN_LeaderboardConfig();
            SaveConfig();
        }
    }

    void SaveConfig()
    {
        JsonFileLoader<SDN_LeaderboardConfig>.JsonSaveFile(CONFIG_PATH, m_Config);
    }

    void LoadData()
    {
        if (FileExist(DATA_PATH))
        {
            JsonFileLoader<array<ref SDN_PlayerStat>>.JsonLoadFile(DATA_PATH, m_PlayerStats);
        }
        else
        {
            SaveData();
        }
    }

    void SaveData()
    {
        JsonFileLoader<array<ref SDN_PlayerStat>>.JsonSaveFile(DATA_PATH, m_PlayerStats);
    }
}