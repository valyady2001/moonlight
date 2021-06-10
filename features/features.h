#pragma once
#include <sdk/classes/base_entity.h>
#include <sdk/classes/usercmd.h>
#include <sdk/math/math.h>
#include <sdk/globals.h>
#include <render/render.h>
#include <vars.h>
#include "hook.h"
#include <optional>
#include <functional>
#include <unordered_map>
#include <deque>
#include <unordered_set>

extern bool build_matrix( base_entity* pl , matrix_t* out , int max_bones , int mask , float seed );
extern bool setup_bones( base_entity* target , int boneMask , float currentTime , matrix_t* pBoneToWorldOut );
class c_nadepoint {
public:
	c_nadepoint( ) {
		m_valid = false;
	}

	c_nadepoint( vec3_t start , vec3_t end , bool plane , bool valid , vec3_t normal , bool detonate ) {
		m_start = start;
		m_end = end;
		m_plane = plane;
		m_valid = valid;
		m_normal = normal;
		m_detonate = detonate;
	}

	vec3_t m_start , m_end , m_normal;
	bool m_valid , m_plane , m_detonate;
};

enum nade_throw_act {
	ACT_NONE ,
	ACT_THROW ,
	ACT_LOB ,
	ACT_DROP
};

class C_Hitbox
{
public:
	int hitboxID;
	bool isOBB;
	vec3_t mins;
	vec3_t maxs;
	float radius;
	int bone;
};
enum hitgroups {
	hitgroup_generic = 0 ,
	hitgroup_head = 1 ,
	hitgroup_chest = 2 ,
	hitgroup_stomach = 3 ,
	hitgroup_leftarm = 4 ,
	hitgroup_rightarm = 5 ,
	hitgroup_leftleg = 6 ,
	hitgroup_rightleg = 7 ,
	hitgroup_gear = 10
};
struct bullet_info_s {
	bullet_info_s( vec3_t firepos , vec3_t impactpos , float impacttime , color col ) {
		fire_pos = firepos;
		impact_pos = impactpos;
		impact_time = impacttime;
		clr = col;
	}
	vec3_t fire_pos;
	vec3_t impact_pos;
	float impact_time;
	color clr;
};

struct {
	float m_flFrametime;
	float m_flCurtime;
	float m_flVelocityModifier;

	vec3_t m_vecVelocity;
	vec3_t m_vecOrigin;
	int m_fFlags;
} m_stored_variables;

struct box {
	int x , y , w , h;
	box( ) = default;
	box( int x , int y , int w , int h ) {
		this->x = x;
		this->y = y;
		this->w = w;
		this->h = h;
	}
};
enum // MANUAL AA MODE
{
	freestanding_aa = 0 ,
	left_aa ,
	right_aa ,
	back_aa
};
namespace features
{
	namespace autowall
	{
		struct penetration_input_t {
			base_entity* m_from;
			base_entity* m_target;
			vec3_t m_pos;
			float m_damage;
			float m_damage_pen;
			bool m_can_pen;
		};

		struct penetration_output_t {
			base_entity* m_target;
			float m_damage;
			int m_hitgroup;
			int m_hitbox;
			bool m_pen;

			__forceinline penetration_output_t( ) : m_target{ nullptr } , m_damage{ 0.f } , m_hitgroup{ -1 } , m_pen{ false } { }
		};

		float scale( base_entity* player , float& damage , float armor_ratio , int hitgroup );
		bool trace_to_exit( const vec3_t& start , const vec3_t& dir , vec3_t& out , trace_t* enter_trace , trace_t* exit_trace );
		void clip_trace_to_player( const vec3_t& start , const vec3_t& end , uint32_t mask , trace_t* tr , base_entity* player , float min );

		/*
		 * changes iPenetration to updated value
		 * returns TRUE if we should stop processing more hits after this one
		 * returns FALSE if we can continue processing
		*/
		bool handle_bullet_penetration( base_entity* player_from ,
			float& penetration ,
			int& enter_material ,
			trace_t& enter_trace ,
			vec3_t& dir ,
			float penetration_modifier ,
			float damage_modifier ,
			float penetration_power ,
			int& penetration_count ,
			vec3_t& start ,
			float distance ,
			float current_distance ,
			float& current_damage );

		bool run( penetration_input_t* in , penetration_output_t* out );
		bool run( base_entity* target , vec3_t point , float dmg , penetration_output_t* out );
	}

	namespace antiaim
	{
		void get_manual_aa_input( );
		void run( );
		float server_curtime( );
		bool lby_update( );
		inline int side;
	}

	namespace animations
	{
		struct realstate
		{
			std::array<float , 24> sent_poses;
			std::array<animlayer , 15> layers;
			float proper_abs;

			bool update;
		};

		struct fake_state_t {
			bool m_should_update_fake = false;
			std::array<float , 20> pose_params;
			std::array<animlayer , 13> anim_layers;
			animstate* m_fake_state = nullptr;
			float m_fake_rotation = 0.f;
			bool init_fake_anim = false;
			float m_fake_spawntime = 0.f;
			matrix_t m_fake_matrix[ 128 ];
			bool m_got_fake_matrix = false;
		};

		inline fake_state_t fake_state;
		inline realstate _realstate;

		float jump_fall( base_entity* pl , float air_time );
		void update_local( );
		void update_desync( );

		void extrapolate( base_entity* pEntity , vec3_t& vecOrigin , vec3_t& vecVelocity , int& fFlags , bool bOnGround );

		inline float desync_delta;
		////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

		inline bool update_matrix;
		inline bool enable_bones;

		/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	}

	namespace lagcomp
	{
		struct records
		{
			records( ) = default;
			records( base_entity* e )
			{
				simtime = e->simtime( );
				dormant = e->dormant( );
				mins = e->mins( );
				maxs = e->maxs( );
				orig = e->origin( );
				abs_orig = e->abs_origin( );
				vel = e->velocity( );
				choked = time_to_ticks( e->simtime( ) - e->old_simtime( ) );
				choked = std::clamp( choked , 0 , 14 );
				valid = choked >= 0 && choked <= 14;
				lby = e->lower_body_yaw( );
				flags = e->flags( );
				abs_ang = e->abs_angles( );
				eflags = e->eflags( );
				duck = e->duck_amount( );
				e->GetAnimLayers( layers );
				abs_vel = e->abs_velocity( );
				eye = e->angles( );
				e->GetPoseParameters( poses );
				
			}
			vec3_t mins , maxs , orig , abs_orig , abs_ang , vel , abs_vel , eye;
			int choked , flags , eflags;
			float simtime , lby , duck;
			bool dormant , valid;
			animlayer layers[ 13 ];
			float poses[ 24 ];
			matrix_t bones[ 128 ] , right[ 128 ] , left[ 128 ];
		};
		bool is_valid( float simtime );
		records* get_records( base_entity* e , bool last );
		inline std::deque<records> rec[ 64 ];
		float lerp_time( );
		void update_ticks();
		inline bool updated[ 64 ];
		inline float spawn_time[ 64 ];
	}

	namespace ragebot
	{
		struct target
		{
			target( base_entity* pl )
			{
				e = pl;
			}
			base_entity* e;
		};
		inline std::vector<target>data;
		void handle_misses( );
		inline int misses[ 64 ];
		void run( usercmd* cmd );
		inline bool can_stop;
		inline bool hitted [ 64 ];
		class ShotSnapshot
		{
		public:
			base_entity* entity;
			float time;
			float first_processed_time;
			bool weapon_fire , bullet_impact;
			int damage;
			int hitbox;
			int backtrack;
			lagcomp::records* record;
			vec3_t eyeangles , impact , start;
		};
		inline std::vector<ShotSnapshot> shot_snapshots;
		inline ShotSnapshot last_hitted[ 65 ];
		inline lagcomp::records backup_anims[ 64 ];
		void init( );
		inline bool pre_stop;

		inline vec3_t ang;
		inline lagcomp::records* b_rec = nullptr;
		inline int hb = -1;
		inline base_entity* ent;
	}
	namespace visuals
	{
		bool get_playerbox( base_entity* ent , box& in );

		struct ctx_t {
			box bbox;
		};
		inline ctx_t context;
		void show_impacts( );
		inline bool paint_data_valid[ 65 ];
		inline box paint_bbox[ 65 ];
		inline weapon_info_t* paint_weapondata[ 65 ];
		inline c_weapon* paint_weapon[ 65 ];
		void main( );
		void draw_box( base_entity* entity , int x , int y , int w , int h , color clr );
		void draw_name( base_entity* entity , int x , int y , int w , int h , const char* info , color clr );
		void draw_health( base_entity* entity , int x , int y , int w , int h , color clr );
		void draw_weapon_name( base_entity* entity , int x , int y , int w , int h , color clr );
		void draw_flags( base_entity* entity , int x , int y , int w , int h );
		void remove_smoke( );
		void remove_flash( );
		void color_modulate_world( );
	}

	namespace chams {
		void run( decltype( &hook::draw_model_execute ) ofunc , void* ctx , void* state , const mdlrender_info_t& info , matrix_t* bone_to_world );
		void init( );
		inline material_t* textured;
		inline material_t* flat;
		inline material_t* bubble;
		inline material_t* glow_cover;
	}

	namespace misc
	{
		inline std::array< c_nadepoint , 500 >	_points{ };
		inline bool		 _predicted;
		void predict( usercmd* user_cmd );
		bool detonated( c_weapon* weapon , float time , trace_t& trace );
		void trace( usercmd* user_cmd );
		void draw( );
		bool in_clmove( );
		void fix_movement( usercmd* cmd , vec3_t wish_angles );
		void doubletap( usercmd* cmd );
		void clamp_move( usercmd* ucmd , float max_speed );
		void autostop( usercmd* cmd );
		void fakelag( );
		void small_movement( );
		void bunnyhop( usercmd* cmd );
		void strafer( usercmd* cmd );
		void fake_duck( usercmd* cmd );
		void thirdperson( );
		inline int lag_amt;
	}

	namespace events
	{
		struct event_t {
			std::string        msg; //where we will store our message
			float            erase1; //when we should DELET the message
			float            erase2; //when we should DELET the message
			color            clr; //fancy thing
			float x1 = -335;
			float x2 = -340;
			bool max_pos = false;
		};

		inline const char* hitgroup_name( int hitgroup ) {
			switch ( hitgroup ) {
			case hitgroup_head:
				return "head";
			case hitgroup_leftleg:
				return "left leg";
			case hitgroup_rightleg:
				return "right leg";
			case hitgroup_stomach:
				return "stomach";
			case hitgroup_leftarm:
				return "left arm";
			case hitgroup_rightarm:
				return "right arm";
			default:
				return "body";
			}
		}

		class c_hooked_events : public i_game_event_listener2 {
		public:
			void log_events( );
			void fire_game_event( i_game_event* event ) noexcept;
			void buybot_event( i_game_event* event ) noexcept;
			void hitmarker_event( i_game_event* event ) noexcept;
			void impact_event( i_game_event* event ) noexcept;
			void purchase_event( i_game_event* event ) noexcept;
			void weapon_fire( i_game_event* event ) noexcept;
			void setup( ) noexcept;
			void release( ) noexcept;
			int get_event_debug_id( void ) noexcept;

			std::deque<event_t> EventList;

		};
		extern int hitmarker_time;
		extern c_hooked_events events;
		inline std::vector<bullet_info_s>bullet_info;
	}

	namespace resolver
	{
		void resolve( base_entity* e );
		struct {
			bool breaking_lby;
			int lby_delta;
			int hit_gfy;
			int jitter_delta;
			int eye_delta;
			int final_delta;
			bool standing;
		}info[ 64 ];
		struct {
			float hit_abs;
			animstate hit_state, resolved_state;
			float resolved_abs;
	

		}resolver_info[64];
	}

	namespace prediction_sys {
		void start( usercmd* cmd );
		void end( );
	}
}