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

    // CORREÇÃO PARA DAYZ 1.26: EEKilled foi renomeado para EOnKilled na engine
    override void EOnKilled(Object killer)
    {
        super.EOnKilled(killer);

        if (GetGame().IsServer())
        {
            PlayerBase pb_Killer = null;

            // 1. Tenta identificar o assassino direto
            Class.CastTo(pb_Killer, killer);

            // 2. Se falhar (ex: explosivos/armadilhas), busca o dono do objeto
            if (!pb_Killer && killer)
            {
                EntityAI killer_ent = EntityAI.Cast(killer);
                if (killer_ent)
                {
                    pb_Killer = PlayerBase.Cast(killer_ent.GetHierarchyRootPlayer());
                }
            }

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

        // O Cliente escuta a resposta do Servidor
        if (rpc_type == 858502 && GetGame().IsClient())
        {
            if (SDN_LeaderboardManager.GetInstance())
            {
                SDN_LeaderboardManager.GetInstance().SDN_OnRPCData(ctx);
            }
        }
    }
}