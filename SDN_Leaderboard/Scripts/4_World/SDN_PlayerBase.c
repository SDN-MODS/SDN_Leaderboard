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

    // CORREÇÃO: Assinatura da função atualizada e lógica de atribuição aprimorada
    override void EEKilled(TotalDamageResult damageResult, Object killer, EntityAI source, bool isHeadshot)
    {
        super.EEKilled(damageResult, killer, source, isHeadshot);

        if (GetGame().IsServer())
        {
            PlayerBase pb_Killer = null;

            // 1. Tenta identificar o assassino direto
            Class.CastTo(pb_Killer, killer);

            // 2. Se falhar (ex: explosivos/armadilhas), busca o dono do objeto (source)
            if (!pb_Killer && source)
            {
                pb_Killer = PlayerBase.Cast(source.GetHierarchyRootPlayer());
            }

            if (pb_Killer && pb_Killer != this && pb_Killer.GetIdentity() && this.GetIdentity())
            {
                float killDistance = vector.Distance(pb_Killer.GetPosition(), this.GetPosition());

                string weaponName = "Unknown";
                if (source)
                {
                    weaponName = source.GetType();
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