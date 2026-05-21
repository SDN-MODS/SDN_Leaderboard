modded class InGameMenu
{
    protected ButtonWidget m_SDN_LeaderboardBtn;

    override Widget Init()
    {
        layoutRoot = super.Init();
        if (layoutRoot)
        {
            Widget container = layoutRoot.FindAnyWidget("play_panel_root");
            Widget exitBtn = layoutRoot.FindAnyWidget("exitbtn");
            Widget restartBtn = layoutRoot.FindAnyWidget("restartbtn");

            if (container)
            {
                // CRIAÇÃO PROFISSIONAL: Criamos o botão do zero usando a API do DayZ
                // Isso evita clonar layouts inteiros e melhora a compatibilidade
                m_SDN_LeaderboardBtn = ButtonWidget.Cast(GetGame().GetWorkspace().CreateWidgets("gui/layouts/new_ui/menu_ok_button.layout", container));

                if (m_SDN_LeaderboardBtn)
                {
                    m_SDN_LeaderboardBtn.SetName("SDN_leaderboard_btn");

                    // Ajuste de texto
                    TextWidget btnText = TextWidget.Cast(m_SDN_LeaderboardBtn.FindAnyWidget("Text"));
                    if (!btnText) btnText = TextWidget.Cast(m_SDN_LeaderboardBtn.GetChildren()); // Fallback

                    if (btnText)
                    {
                        btnText.SetText("LEADERBOARD");
                    }

                    // Estilização
                    m_SDN_LeaderboardBtn.SetColor(ARGB(255, 220, 20, 20));
                    m_SDN_LeaderboardBtn.SetSort(1); // Garante a ordem correta

                    // Reorganização do container para manter o padrão visual
                    if (restartBtn) restartBtn.SetSort(2);
                    if (exitBtn) exitBtn.SetSort(3);
                }
            }
        }
        return layoutRoot;
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_SDN_LeaderboardBtn)
        {
            GetGame().GetUIManager().HideScriptedMenu(this);
            MissionGameplay mission = MissionGameplay.Cast(GetGame().GetMission());
            if (mission) mission.SDN_ShowLeaderboard();
            return true;
        }
        return super.OnClick(w, x, y, button);
    }
}