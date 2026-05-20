// ============================================================================
// PASTA: SDN_MODS/SDN_Leaderboard/Scripts/4_World
// ARQUIVO: SDN_PlayerBase.c
// ============================================================================

modded class PlayerBase
{
    int m_SDN_SpawnTime; 

    override void Init()
    {
        super.Init();
        
        if (GetGame().IsServer())
        {
            m_SDN_SpawnTime = GetGame().GetTime();
        }
    }

    // CORREÇÃO: Assinatura da função restaurada para o padrão Vanilla exato
    override void EEKilled(Object killer)
    {
        super.EEKilled(killer);

        if (GetGame().IsServer())
        {
            PlayerBase pb_Killer;
            Class.CastTo(pb_Killer, killer);

            if (pb_Killer && pb_Killer != this && pb_Killer.GetIdentity() && this.GetIdentity())
            {
                float killDistance = vector.Distance(pb_Killer.GetPosition(), this.GetPosition());
                string weaponName = "Unknown";
                EntityAI sourceEntity = EntityAI.Cast(killer);
                if (sourceEntity)
                {
                    weaponName = sourceEntity.GetType();
                }

                SDN_LeaderboardManager.GetInstance().ProcessKillEvent(pb_Killer, this, weaponName, killDistance);
            }
        }
    }

    // ================== ANTENA DE REDE (RPC) ==================
    override void OnRPC(PlayerIdentity sender, int rpc_type, ParamsReadContext ctx)
    {
        super.OnRPC(sender, rpc_type, ctx);

        // O Servidor escuta o pedido do painel
        if (rpc_type == 858501 && GetGame().IsServer())
        {
            SDN_LeaderboardManager.GetInstance().SendLeaderboardDataToPlayer(sender);
        }
        
        // O Cliente escuta a resposta do Servidor e notifica a Interface
        if (rpc_type == 858502 && GetGame().IsClient())
        {
            if (SDN_LeaderboardManager.GetInstance() && SDN_LeaderboardManager.GetInstance().m_OnDataReceived)
            {
                SDN_LeaderboardManager.GetInstance().m_OnDataReceived.Invoke(ctx);
            }
        }
    }
}