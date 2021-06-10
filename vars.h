#pragma once
#include <vector>
#include <unordered_map>

class vars
{
public:
	bool save( std::string name );
	bool load( std::string file_name );
	void create( std::string name );
	std::vector<std::string> getcfg( );

	struct
	{
		bool enable;
		bool autoscope;
		bool autofire;
		bool resolver;
		int safepoint_type;

		int min_dmg;
		int dmg_override;
		int dmg_bind[ 2 ] = { 0,1 };
		int forceb_bind[ 2 ] = { 0,2 };
		bool override_dmg;
		bool hitboxes[ 7 ];
		bool baim[4];

		bool refine_shot;
		int hitchance;
		int autostop_mode;
		int exploit;
		int exploit_bind[ 2 ] = { 0,2 };
	}ragebot;

	struct
	{
		bool enable;

		bool manual;
		bool at_target;
		bool freestand;

		int pitch = 0;

		int left_key;
		int right_key;

		int inverter[ 2 ] = { 0,2 };

		bool enable_fl;
		int fakeduck_bind[ 2 ] = { 0,1 };
		int type;

		int fjitter;
		int rjitter;

		int st_lag;
		int mv_lag;
		int air_lag;

		int max_lag;
		int min_lag;

		int adaptive;
	}antihit;

	struct
	{
		bool kill_effect;

		bool box;
		bool name;
		bool wpn;
		bool health;
		bool glow;
		
		float box_clr[ 3 ] = { 1.f,1.f,1.f };
		float name_clr[ 3 ] = { 1.f,1.f,1.f };
		float wpn_clr[ 3 ] = { 1.f,1.f,1.f };
		float health_clr[ 3 ] = { 0.f,1.f,0.f };
		float glow_clr[ 4 ] = { 1.f,0.f,0.f,1.f };

		bool flags[ 3 ];

		struct {
			bool hidden_chams;
			bool visible_chams;
			bool desync_chams;
			bool local_chams;

			int material_type;
			int chams_selection;

			int desync_mat_type;
			int local_mat_type;

			float visible_chams_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float desync_chams_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float hidden_chams_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float local_chams_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float overlay_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float desync_overlay_color[ 4 ] = { 1.f,1.f,1.f,1.f };
			float local_overlay_color[ 4 ] = { 1.f,1.f,1.f,1.f };
		}chams;
		bool removals[ 9 ];
		int viewfov = 60;
		int view_x;
		int view_y;
		int view_z;
		int camdist;
		bool fullbright;
		bool grenade_pred;
		bool sv_impacts;
		int nightmode;
	}esp;

	struct
	{
		struct {
			bool enable;

			int primary;
			int secondary;
			bool utility[ 4 ];

		}buybot;

		bool antiuntrusted = true;
		bool logs[ 4 ];
		bool strafer;
		bool bunnyhop;
		int tp_bind[ 2 ] = { 0,2 };
		int sound;
	}misc;

	bool teleport;
	int weapon_config;
	struct {
		int safepoint_type;

		int min_dmg;
		int dmg_override;
		int dmg_bind [ 2 ] = { 0,1 };
		int forceb_bind [ 2 ] = { 0,2 };
		bool override_dmg;
		bool hitboxes [ 7 ];
		bool baim [ 4 ];

		bool refine_shot;
		int hitchance;
		int autostop_mode;
		int exploit;
		int exploit_bind [ 2 ] = { 0,2 };
	}awp,scout,autosniper,heavy,pistols,smgs,rifles;
	int target_selection;
	int desync_type;
	int st_opposite_amt, mv_opposite_amt, air_opposite_amt;
	int general_body_yaw, general_inverse_body_yaw;
	int fake_jitter, real_jitter;
	bool clantag; 
};

extern vars g_var;