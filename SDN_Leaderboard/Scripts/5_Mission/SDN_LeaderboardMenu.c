class SDN_LeaderboardMenu extends UIScriptedMenu
{
    private Widget m_SDN_RowContainer;
    private ButtonWidget m_BtnClose;
    private Widget m_MyStatsFooter;
    private ref array<Widget> m_SDN_RowWidgets;
    private bool m_IsMenuOpen;

    void SDN_LeaderboardMenu() { m_SDN_RowWidgets = new array<Widget>; m_IsMenuOpen = false; }

    override Widget Init()
    {
        layoutRoot = GetGame().GetWorkspace().CreateWidgets("SDN_Leaderboard/GUI/Layouts/SDN_Leaderboard.layout");
        Widget bg = layoutRoot.FindAnyWidget("SDN_Background");
        if (bg) bg.SetColor(ARGB(240, 15, 15, 15)); 
        m_SDN_RowContainer = layoutRoot.FindAnyWidget("SDN_RowContainer");
        m_BtnClose = ButtonWidget.Cast(layoutRoot.FindAnyWidget("SDN_BtnClose"));
        m_MyStatsFooter = layoutRoot.FindAnyWidget("SDN_MyStatsFooter");
        if (m_MyStatsFooter) m_MyStatsFooter.Show(false);
        return layoutRoot;
    }

    override void OnShow()
    {
        super.OnShow();
        m_IsMenuOpen = true;
        GetGame().GetMission().PlayerControlDisable(INPUT_EXCLUDE_ALL);
        GetGame().GetUIManager().ShowUICursor(true);
        GetGame().GetUIManager().ShowCursor(true);
        GetGame().GetInput().ChangeGameFocus(1);
        GetGame().GetMission().GetHud().Show(false);
        SDN_RequestDataFromServer();
    }

    override void OnHide()
    {
        super.OnHide();
        m_IsMenuOpen = false;
        GetGame().GetUIManager().ShowCursor(false);
        GetGame().GetUIManager().ShowUICursor(false);
        GetGame().GetInput().ResetGameFocus();
        GetGame().GetMission().PlayerControlEnable(true);
        GetGame().GetMission().GetHud().Show(true);
        SDN_ClearList();
        if (layoutRoot) layoutRoot.Unlink();
    }

    override void Update(float timeslice)
    {
        super.Update(timeslice);
        if (GetGame().GetInput().LocalPress("UAUIBack", false)) Close();
    }

    override bool OnClick(Widget w, int x, int y, int button)
    {
        if (w == m_BtnClose) { Close(); return true; }
        return super.OnClick(w, x, y, button);
    }

    void SDN_ClearList()
    {
        if (m_SDN_RowWidgets) { foreach (Widget row : m_SDN_RowWidgets) { if (row) row.Unlink(); } m_SDN_RowWidgets.Clear(); }
    }

    void SDN_RequestDataFromServer()
    {
        SDN_ClearList();
        if (m_MyStatsFooter) m_MyStatsFooter.Show(false);
        GetRPCManager().SendRPC("SDN_Leaderboard", "RequestData", null, true, null);
    }

    void SDN_ReceiveData(ParamsReadContext ctx)
    {
        SDN_ClearList();
        int count = 0;
        if (!ctx.Read(count)) return;
        for (int i = 0; i < count; i++)
        {
            string name = ""; int kills = 0; int deaths = 0; float kd = 0.0; int longest = 0;
            ctx.Read(name); ctx.Read(kills); ctx.Read(deaths); ctx.Read(kd); ctx.Read(longest);
            SDN_CreatePlayerRow(i + 1, name, kills, deaths, kd, longest);
        }
        bool hasMyStat = false;
        if (ctx.Read(hasMyStat) && hasMyStat)
        {
            int myRank = 0; string myName = ""; int myKills = 0; int myDeaths = 0; float myKD = 0.0; int myLongest = 0;
            ctx.Read(myRank); ctx.Read(myName); ctx.Read(myKills); ctx.Read(myDeaths); ctx.Read(myKD); ctx.Read(myLongest);
            if (m_MyStatsFooter)
            {
                m_MyStatsFooter.Show(true);
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyRankValue")).SetText("#" + myRank.ToString());
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyPlayerName")).SetText(myName);
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyKillsValue")).SetText(myKills.ToString());
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyDeathsValue")).SetText(myDeaths.ToString());
                string kdStr = myKD.ToString();
                if (kdStr.Length() > 4) kdStr = kdStr.Substring(0, 4);
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyKDValue")).SetText(kdStr);
                TextWidget.Cast(layoutRoot.FindAnyWidget("SDN_MyLongestShotValue")).SetText(myLongest.ToString() + "m");
            }
        }
    }

    private void SDN_CreatePlayerRow(int rank, string name, int kills, int deaths, float kd, int longestKill)
    {
        if (!m_SDN_RowContainer) return;
        Widget rowInstance = GetGame().GetWorkspace().CreateWidgets("SDN_Leaderboard/GUI/Layouts/SDN_Leaderboard_Row.layout", m_SDN_RowContainer);
        if (!rowInstance) return;
        if (rank % 2 == 0) rowInstance.SetColor(ARGB(220, 25, 25, 25)); else rowInstance.SetColor(ARGB(220, 45, 45, 45));
        TextWidget txtRank = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_RankValue"));
        TextWidget txtName = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_PlayerName"));
        TextWidget txtKills = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_KillsValue"));
        TextWidget txtDeaths = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_DeathsValue"));
        TextWidget txtKD = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_KDValue"));
        TextWidget txtLongest = TextWidget.Cast(rowInstance.FindAnyWidget("SDN_LongestShotValue"));
        if (txtRank) txtRank.SetText("#" + rank.ToString()); if (txtName) txtName.SetText(name); if (txtKills) txtKills.SetText(kills.ToString()); if (txtDeaths) txtDeaths.SetText(deaths.ToString()); if (txtLongest) txtLongest.SetText(longestKill.ToString() + "m"); 
        string kdStr = kd.ToString(); if (kdStr.Length() > 4) kdStr = kdStr.Substring(0, 4); if (txtKD) txtKD.SetText(kdStr); 
        m_SDN_RowWidgets.Insert(rowInstance);
    }
    bool SDN_IsMenuOpen() { return m_IsMenuOpen; }
}