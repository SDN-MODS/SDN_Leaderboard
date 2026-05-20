modded class MissionServer
{
    override void OnMissionStart()
    {
        super.OnMissionStart();
        
        // Mensagem de depuração à prova de balas
        Print("======================================================");
        Print("[SDN_LEADERBOARD] >>> ON_MISSION_START CHAMADO! <<<");
        Print("======================================================");
        
        // Dispara a criação das pastas e JSONs
        SDN_LeaderboardManager.GetInstance();
    }
}