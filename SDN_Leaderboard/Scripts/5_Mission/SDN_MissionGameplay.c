// ============================================================================
// PASTA: SDN_Leaderboard/Scripts/5_Mission
// ARQUIVO: SDN_MissionGameplay.c
// DESCRIÇÃO: Teclas removidas. Agora é acionado diretamente pelo menu do ESC.
// ============================================================================

modded class MissionGameplay
{
    private ref SDN_LeaderboardMenu m_SDNLeaderboardMenu;

    void MissionGameplay()
    {
        if (SDN_LeaderboardManager.GetInstance() && SDN_LeaderboardManager.GetInstance().m_OnDataReceived)
        {
            SDN_LeaderboardManager.GetInstance().m_OnDataReceived.Insert(SDN_UpdateLeaderboardUI);
        }
    }

    void ~MissionGameplay()
    {
        if (SDN_LeaderboardManager.GetInstance() && SDN_LeaderboardManager.GetInstance().m_OnDataReceived)
        {
            SDN_LeaderboardManager.GetInstance().m_OnDataReceived.Remove(SDN_UpdateLeaderboardUI);
        }
    }

    // Recebe o Payload do Manager e injeta no Menu
    void SDN_UpdateLeaderboardUI(SDN_LeaderboardPayload payload)
    {
        if (m_SDNLeaderboardMenu && m_SDNLeaderboardMenu.SDN_IsMenuOpen())
        {
            m_SDNLeaderboardMenu.SDN_ReceiveData(payload);
        }
    }

    // NOVA FUNÇÃO: Chamada exclusivamente pelo botão no Menu ESC
    void SDN_ShowLeaderboard()
    {
        if (!m_SDNLeaderboardMenu)
        {
            m_SDNLeaderboardMenu = new SDN_LeaderboardMenu();
            m_SDNLeaderboardMenu.Init();
        }

        if (!m_SDNLeaderboardMenu.SDN_IsMenuOpen())
        {
            GetGame().GetUIManager().ShowScriptedMenu(m_SDNLeaderboardMenu, NULL);
        }
    }
}