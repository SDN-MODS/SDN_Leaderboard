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
                Widget tempRoot = GetGame().GetWorkspace().CreateWidgets("gui/layouts/day_z_ingamemenu.layout", null);
                if (tempRoot)
                {
                    ButtonWidget stolenBtn = ButtonWidget.Cast(tempRoot.FindAnyWidget("optionsbtn"));
                    if (stolenBtn)
                    {
                        stolenBtn.Unlink();
                        m_SDN_LeaderboardBtn = stolenBtn;
                        m_SDN_LeaderboardBtn.SetName("SDN_leaderboard_btn");
                        TextWidget btnText = TextWidget.Cast(m_SDN_LeaderboardBtn.GetChildren());
                        if (btnText) btnText.SetText("LEADERBOARD");
                        m_SDN_LeaderboardBtn.SetColor(ARGB(255, 220, 20, 20));
                    }
                    
                    PanelWidget redLine = PanelWidget.Cast(GetGame().GetWorkspace().CreateWidget(PanelWidgetTypeID, container));
                    redLine.SetColor(ARGB(255, 200, 20, 20));
                    redLine.SetSize(1, 4);
                    
                    if (restartBtn) restartBtn.Unlink();
                    if (exitBtn) exitBtn.Unlink();
                    
                    if (m_SDN_LeaderboardBtn) container.AddChild(m_SDN_LeaderboardBtn);
                    container.AddChild(redLine);
                    if (restartBtn) container.AddChild(restartBtn);
                    if (exitBtn) container.AddChild(exitBtn);
                    
                    tempRoot.Unlink(); 
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