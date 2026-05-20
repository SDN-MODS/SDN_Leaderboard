class CfgPatches
{
    class SDN_Leaderboard
    {
        units[]={};
        weapons[]={};
        requiredVersion=0.1;
        requiredAddons[]=
        {
            "DZ_Data",
            "DZ_Scripts",
            "JM_CF_Scripts"
        };
    };
};

class CfgMods
{
    class SDN_Leaderboard
    {
        dir="SDN_Leaderboard";
        picture="";
        action="";
        hideName=1;
        hidePicture=1;
        name="SDN Leaderboard";
        credits="Sua Comunidade";
        author="Seu Nome";
        authorID="0";
        version="1.0";
        extra=0;
        type="mod";
        
        // CORRIGIDO: Agora usa a barra normal '/' idêntico ao exemplo do BLR_Insurance
        // Isso permite que o motor do DayZ mapeie corretamente o arquivo de inputs do PBO
        //inputs="SDN_Leaderboard/data/inputs.xml"; 
        
        dependencies[]=
        {
            "Game",
            "World",
            "Mission"
        };
        
        class defs
        {
            class gameScriptModule
            {
                value="";
                files[]=
                {
                    "SDN_Leaderboard/Scripts/3_Game"
                };
            };
            class worldScriptModule
            {
                value="";
                files[]=
                {
                    "SDN_Leaderboard/Scripts/4_World"
                };
            };
            class missionScriptModule
            {
                value="";
                files[]=
                {
                    "SDN_Leaderboard/Scripts/5_Mission"
                };
            };
        };
    };
};